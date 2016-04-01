#include <Arduino.h>
#include "Strings.h"
#include "WiznetTCP.h"
#include "DHCP.h"

// MULTICAST FOR GATEWAY DISCOVERY

#define DISCOVERY_PORT 54321
#define DISCOVERY_GROUP "225.0.0.37" 

#define MAX_MDNS_RETRY 10
#define MDNS_WAIT_TIME 2000

// COMMON REGISTERS

#define W5100_MODE_REGISTER 0X0000 // 1 byte
#define W5100_GATEWAY 0X0001 // 4 bytes
#define W5100_SUBNET_MASK 0X0005 // 4 bytes
#define W5100_MAC_ADDRESS 0X0009 // 6 bytes
#define W5100_LOCAL_IP_ADDRESS 0X000F // 4 bytes
#define W5100_INTR_REGISTER 0x0015
#define W5100_INTR_MASK_REGISTER 0x0016
#define W5100_RMSR_REGISTER 0X001A // 1 byte
#define W5100_TMSR_REGISTER 0X001B // 1 byte
#define W5100_UIPR0_REGISTER 0X002A // 4 bytes
#define W5100_UPORT0_REGISTER 0X002E // 2 bytes

// SOCKET REGISTER OFFSETS

#define SOCKET_MODE 0x0
#define SOCKET_COMMAND 0x1
#define SOCKET_INTERRUPT 0x2
#define SOCKET_STATUS 0x3
#define SOCKET_SRC_PORT 0x4
#define SOCKET_MAC_ADDRESS 0x6
#define SOCKET_IP_ADDRESS 0xC
#define SOCKET_DEST_PORT 0x10
#define SOCKET_MAX_SEG_SIZE 0x12
#define SOCKET_PROTO 0x14
#define SOCKET_TOS 0x15
#define SOCKET_TTL 0x16
#define SOCKET_TX_FREE_SIZE 0x20
#define SOCKET_TX_READ_PNTR 0x22
#define SOCKET_TX_WRITE_PNTR 0x24
#define SOCKET_RX_RECV_SIZE 0x26
#define SOCKET_RX_READ_PNTR 0x28

// STATUS FLAGs

#define W5100_SEND_OK 16
#define W5100_TIMEOUT 8
#define W5100_RECV 4
#define W5100_DISCON 2
#define W5100_CON 1


// SOCKET BUFER INFO
// 2 sockets each with 4KB for TX and RX buffers
// more generally will need a macro based upon
// number of sockets that have been initialised
// for now only concerned with socket zero

#define BASE_TX_BUFFER 0x4000
#define BASE_RX_BUFFER 0x6000
#define SOCKET_SIZE 0x1000
#define TX_BUFFER_MASK 0xFFF
#define RX_BUFFER_MASK 0xFFF

// OFFSETS

#define SOCKET_ZERO 0x400
#define SOCKET_ONE 0x500
#define SOCKET_TWO 0x600
#define SOCKET_THREE 0x700

// COMMANDs

#define SOCKET_OPEN 0x01
#define SOCKET_LISTEN 0x02
#define SOCKET_CONNECT 0x04
#define SOCKET_DISCONNECT 0x08
#define SOCKET_CLOSE 0x10
#define SOCKET_SEND 0x20
#define SOCKET_SEND_MAC 0x21
#define SOCKET_SEND_KEEP 0x22
#define SOCKET_RECEIVE 0x40

// MODE

#define SOCKET_CLOSED 0x00
#define SOCKET_TCP 0x01
#define SOCKET_UDP 0x02
#define SOCKET_RAW 0x03
#define SOCKET_MULTICAST 0x80
#define IGMP_V1 0x20
#define IGMP_V2 0x00

// SOCKET STATUS defined in include file for use by apps

// forward reference to SPI data transfer functions
inline static uint8_t read_byte(uint16_t addr);
inline static void write_byte(uint16_t addr, uint8_t data);
static uint16_t read_word(uint16_t addr);
static void write_word(uint16_t addr, uint16_t word);
static void write32(uint16_t addr, uint8_t *p);
static void write48(uint16_t addr, uint8_t *p);
static void write32(uint16_t addr, uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3);
static void write48(uint16_t addr, uint8_t n0, uint8_t n1, uint8_t n2,
                                   uint8_t n3, uint8_t n4, uint8_t n5);


WiznetTCP::WiznetTCP()
{
    // wait until W5100 has started up and is ready for commands
    delayMicroseconds(500);
    spi_init();

    write_byte(W5100_MODE_REGISTER, 0x80); // reset
    
    // 2 sockets with 8KB each, i.e. 4KB max packet size
    // but for 4 sockets with 4KB use value of 0x55
    // if we only use 1 socket we can use 2 * 4KB for RAM
    write_byte(W5100_RMSR_REGISTER, 0xAA);
    write_byte(W5100_TMSR_REGISTER, 0xAA);
}

WiznetTCP::~WiznetTCP()
{
    // close W5100 socket
}

void WiznetTCP::begin(uint16_t port)
{
    delay(200); // not needed, here for luck
    
    // ask server for IP address, subnet mask and gateway
    if (!run_DHCP_client()) {
        // Couldn't get an IP address so use defaults
        Serial.println(F("Using default network configuration as fall back"));
        write32(W5100_GATEWAY, 192, 168, 1, 254);
        write32(W5100_SUBNET_MASK, 255, 255, 255, 0);
        write32(W5100_LOCAL_IP_ADDRESS, 192, 168, 1, 25);
    }
    
    uint8_t n0, n1, n2, n3;
    get_local_ip(&n0, &n1, &n2, &n3);
    Serial.print(F("IP = "));
    Serial.print(n0);
    Serial.print(F("."));
    Serial.print(n1);
    Serial.print(F("."));
    Serial.print(n2);
    Serial.print(F("."));
    Serial.print(n3);
    Serial.print(F(", port = "));
    Serial.println(port);
    
    write_word(SOCKET_ZERO + SOCKET_SRC_PORT, port);
    open();
}

void WiznetTCP::begin(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3, uint16_t port)
{
    delay(200); // not needed, here for luck
    
    write32(W5100_GATEWAY, n0, n1, n2, 254);
    write48(W5100_MAC_ADDRESS, 0x61, 0xf8, 0x1d, 0xbc, 0xf4, 0x2f);
    write32(W5100_SUBNET_MASK, 255, 255, 255, 0);
    write32(W5100_LOCAL_IP_ADDRESS, n0, n1, n2, n3);
    write_word(SOCKET_ZERO + SOCKET_SRC_PORT, port);
    open();
}

void WiznetTCP::discover_gateway()
{
    uint16_t length;
    uint8_t found = 0;
    
    uint8_t query[33] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header
        0x04,0x5F,0x77,0x6F,0x74,  // _wot
        0x04,0x5F,0x74,0x63,0x70,  // _tcp
        0x05,0x6C,0x6F,0x63,0x61,0x6C,  // local
        0x00, // end of service type
        0x00, 0x0C, // QTYPE PTR
        0x00, 0x01, // QCLASS IN
    };

    // the gateway is recored as public properties on WiznetTCP class
    // I need a better way to pass these to the transport layer
    gateway_ip = 0;
    gateway_port = 0;

    // save source port to restore it upon return 
    uint16_t port = read_word(SOCKET_ZERO + SOCKET_SRC_PORT);
     
    for (uint8_t i = MAX_MDNS_RETRY; i; --i) {
        uint8_t status = get_socket_status();
        
        if (status == SOCK_INIT || status == SOCK_CLOSED) {
            Serial.println(F("starting mDNS search"));

            write_word(SOCKET_ZERO + SOCKET_DEST_PORT, 5353);
            write_word(SOCKET_ZERO + SOCKET_SRC_PORT, 5353);
            
            // mDNS IPv4 dest address 224.0.0.251 port 5353
            write32(SOCKET_ZERO + SOCKET_IP_ADDRESS, 224, 0, 0, 251);

            // mDNS socket dest mac address 01:00:5E:00:00:FB
            // this is a simple mapping from the IPv4 address
            write48(SOCKET_ZERO + SOCKET_MAC_ADDRESS,
                    0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB);
            
            // The W5100 automatically sends the IGMP join (report)
            // when the multicast socket is opened, and likewise, sends
            // the IGMP leave automatically when the socket is closed
    
            write_byte(SOCKET_ZERO + SOCKET_MODE, SOCKET_UDP|SOCKET_MULTICAST|IGMP_V2);
            write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_OPEN);
        }
    
        if (get_socket_status() == SOCK_UDP) {
            Serial.print(F("querying _wot._tcp.local, "));
	        send((char *)query, sizeof(query));
	    } else {
	        Serial.println(F("socket not open for UDP"));
	        close();
	        continue;
	    }
	    
	    if ((length = receive_available(MDNS_WAIT_TIME)) > 0) {
	        Serial.print(F("got mDNS message wireshark length "));
            Serial.println(length+34);
	        
	        if (!handle_mDNS_response(query))
	            Serial.println(F("error in DNS message"));
	            
	        if (gateway_ip) {
	            found = 1;
	            break;
	        }
	        
	        flush_receive();
	    }
	}

    close();
    
    if (!found)
        Serial.print(F("couldn't find gateway, "));
        
    write_word(SOCKET_ZERO + SOCKET_SRC_PORT, port);
    Serial.println(F("ending mDNS search"));
}

// query is the mDNS query message that triggered the response
uint8_t WiznetTCP::handle_mDNS_response(uint8_t *query)
{
    uint32_t ip;
    uint16_t port, offset = 0, name_offset, srv_link = 0;
    uint8_t buffer[64];
    uint8_t match;
    uint16_t length = receive_available();
    
    // first 8 bytes are for IPv4 address, port and length
    // these are followed by the 12 byte DNS message header
    if (peek(offset, (char *)buffer, 20) != 20)
        return 0;

    offset += 20;
    uint16_t id    = buffer[8]; id <<= 8; id |= buffer[9];
    uint16_t flags = buffer[10]; flags <<= 8; flags |= buffer[11];
    uint16_t qdcnt = buffer[12]; qdcnt <<= 8; qdcnt |= buffer[13];
    uint16_t ancnt = buffer[14]; ancnt <<= 8; ancnt |= buffer[15];
    uint16_t nscnt = buffer[16]; nscnt <<= 8; nscnt |= buffer[17];
    uint16_t arcnt = buffer[18]; arcnt <<= 8; arcnt |= buffer[19];
    uint8_t rrcount = ancnt + nscnt + arcnt;

    Serial.print(F(" id = 0x")); Serial.println(id, HEX);
    Serial.print(F(" flags = 0x")); Serial.println(flags, HEX);
    Serial.print(F(" Questions: ")); Serial.println(qdcnt);
    Serial.print(F(" Answer RRs: ")); Serial.println(ancnt);
    Serial.print(F(" Authority RRs: ")); Serial.println(nscnt);
    Serial.print(F(" Additional RRs: ")); Serial.println(arcnt);

    // Parse questions which may come from other clients
        
    while (qdcnt--) {
        Serial.print(F("QN: "));
        
        if (!(offset = parse_name(offset, buffer, query+12, &match)))
            return 0;
        
        // skip over qtype and qclass
        if (peek(offset, (char *)buffer, 4) != 4)
                return 0;
                
        offset += 4;
        uint16_t qtype = buffer[0]; qtype <<= 8; qtype |= buffer[1];
        uint16_t qclass = buffer[2]; qclass <<= 8; qclass |= buffer[3];
        Serial.print(F(" type = 0x")); Serial.println(qtype, HEX);
        Serial.print(F(" class = 0x")); Serial.println(qclass, HEX);
    }

    // Parse resource records 
    
    while (rrcount--) {
        // read DNS NAME and check it is as expected
        // NAMES are <= 255 bytes including length fields
        // LABELS are 63 bytes max
        // *** please implement the checking algorithm
                
        Serial.print(F("RN: "));
        name_offset = offset; // note for later match
        
        if (!(offset = parse_name(offset, buffer, query+12, &match)))
            return 0;
        
        // TYPE 2 bytes with resource record type.
        // CLASS 2 bytes indicating the class of the data
        // TTL a 32 bit unsigned integer giving expiry time in seconds
        // RDLENGTH 16 bits indicating the length of the RDATA field in bytes.
        // RDATA varies depending on the type and class of the resource record.

        // read the resource record header
        if (peek(offset, (char *)buffer, 10) != 10)
            return 0;

        offset += 10;
        uint16_t rtype = buffer[0]; rtype <<= 8; rtype |= buffer[1];
        uint16_t rclass = buffer[2]; rclass <<= 8; rclass |= buffer[3];
        uint32_t time2live = buffer[4];
        time2live <<= 8; time2live |= buffer[5];
        time2live <<= 8; time2live |= buffer[6];
        time2live <<= 8; time2live |= buffer[7];
        uint16_t rdlength = buffer[8]; rdlength <<= 8; rdlength |= buffer[9];
        
        Serial.print(F("Rtype: "));
        Serial.println(rtype);
        Serial.print(F("Rclass: 0x"));
        Serial.println(rclass, HEX);
        Serial.print(F("Rttl: "));
        Serial.println(time2live);
        Serial.print(F("Rlength: "));
        Serial.println(rdlength);
        
        // followed by rdlength bytes of data
        
        if (rtype == 33) {
            // DNS SRV record - server port
            if (peek(offset, (char *)buffer, 6) != 6)
                return 0;
                
            offset += 6;
            uint16_t srv_port = buffer[4];
            srv_port <<= 8;
            srv_port |= buffer[5];
            srv_link = offset;
            rdlength -= 6;
            
            Serial.print(F("Got port "));
            Serial.println(srv_port);    
                  
            if (match)
                gateway_port = srv_port;
        } else if (rtype == 1) {
            // DNS A record - IPv4 address
            if (peek(offset, (char *)buffer, 4) != 4)
                return 0;

            offset += 4;
            uint32_t srv_ip = buffer[0];
            srv_ip <<= 8; srv_ip |= buffer[1];
            srv_ip <<= 8; srv_ip |= buffer[2];
            srv_ip <<= 8; srv_ip |= buffer[3];
            rdlength -= 4;
            
            Serial.print(F("Got IP "));
            Serial.print((srv_ip>>24)&255);
            Serial.print(F("."));
            Serial.print((srv_ip>>16)&255);
            Serial.print(F("."));
            Serial.print((srv_ip>>8)&255);
            Serial.print(F("."));
            Serial.print(srv_ip&255);
            Serial.println();
            
            uint16_t link = get_link(name_offset);
            
            if (link < 0xFFFF && link == srv_link) {
                Serial.println(F("Matching IP record"));
                gateway_ip = srv_ip;
            }
        }
        
        offset += rdlength;
    }
    
    // found gateway?
    
    if (gateway_ip && gateway_port) {
        Serial.print(F("Gateway IP = "));
        Serial.print((gateway_ip>>24)&255);
        Serial.print(F("."));
        Serial.print((gateway_ip>>16)&255);
        Serial.print(F("."));
        Serial.print((gateway_ip>>8)&255);
        Serial.print(F("."));
        Serial.print(gateway_ip&255);
        Serial.print(F(", port = "));
        Serial.println(gateway_port);
    }
                
    return 1;
}

// link to first NAME is 12, but need to add
// 8 for the preceding IPv4, port and length
uint16_t WiznetTCP::get_link(uint16_t offset)
{
    uint8_t buffer[2];
    uint16_t link;
    
    if (peek(offset++, (char *)buffer, 2) != 2)
            return 0xFFFF;
            
    link = buffer[0];
    
    if (link < 0xC0)
        return 0xFFFF;
        
    link -= 0xC0;
    link <<= 8;
    link |= buffer[1];
    return link + 8;
}

// sn is the service name used in the mDNS query
// buffer is large enough for all valid labels
// offset is position to read within received message
uint16_t WiznetTCP::parse_name(uint16_t offset, uint8_t *buffer,
                                uint8_t *sn, uint8_t *matched)
{
    uint16_t saved = 0;
    uint8_t i = 0;
    uint8_t match = 1;
    *matched = 0;
    
    for (;;) {
        // get number of characters in label
        if (peek(offset++, (char *)buffer, 1) != 1)
            return 0;

        uint8_t label_len = buffer[0];
            
        // 0 signals end of NAME
        if (!label_len) {
            if (saved)
                offset = saved;
            break;
        }
        
        // check for link to NAME earlier in message
        if (label_len >= 0xC0) {
            if (peek(offset++, (char *)buffer+1, 1) != 1)
                return 0;
                    
            if (!saved)
                saved = offset;
                    
            offset = buffer[0] - 0xC0;
            offset <<= 8;
            offset |= buffer[1];
            offset += 8;
            continue;
        }
        
        // RFC 1025: labels should be 63 octets or less
        if (label_len > 63)
            return 0;
            
        // read name part into buffer
        if (peek(offset, (char *)buffer, label_len) != label_len)
            return 0;
                
        offset += label_len;
        buffer[label_len] = '\0';
        Serial.print((char *)buffer);
        Serial.print(F("."));
        
        // check for incremental match to query service name
        if (match) {
            if (*sn == label_len &&
             !strncmp((const char *)sn+1, (const char *)buffer, (size_t)label_len))
                sn += 1 + label_len;
            else if (i > 0)
                 match = 0;
        }
        
        ++i;
    }
        
    Serial.println();
    
    if (match)
        Serial.println(F("record matches query"));
        
    *matched = match;
    return offset;
}

// to be scrapped as will waste flash space after debugging is done
#if 0
void WiznetTCP::dump_buffer(uint8_t *buffer, uint16_t length)
{
    uint8_t i, c;
    
    Serial.print(F("Dump of "));
    Serial.print(length);
    Serial.println(F(" bytes"));
    
    while (length) {
        for (i = 0; i < 8; ++i) {
            if (!length)
                break;
            
            c = *buffer++;
        
            if (c < 16)
                Serial.print(F("0"));
            
            Serial.print(c, HEX);
            Serial.print(F(" "));
            --length;
        }
    
        Serial.print(F(" "));
        
        for (; i < 16; ++i) {
            if (!length)
                break;
            
            c = *buffer++;
        
            if (c < 16)
                Serial.print(F("0"));
            
         Serial.print(c, HEX);
            Serial.print(F(" "));
            --length;
        }
    
        Serial.println();
    }
    
    Serial.println();
}

void WiznetTCP::print_mac_address()
{
    uint8_t m0 = read_byte(W5100_MAC_ADDRESS);
    uint8_t m1 = read_byte(W5100_MAC_ADDRESS+1);
    uint8_t m2 = read_byte(W5100_MAC_ADDRESS+2);
    uint8_t m3 = read_byte(W5100_MAC_ADDRESS+3);
    uint8_t m4 = read_byte(W5100_MAC_ADDRESS+4);
    uint8_t m5 = read_byte(W5100_MAC_ADDRESS+5);
    Serial.print(F("mac address: "));
    Serial.print(m0, HEX);
    Serial.print(F(":"));
    Serial.print(m1, HEX);
    Serial.print(F(":"));
    Serial.print(m2, HEX);
    Serial.print(F(":"));
    Serial.print(m3, HEX);
    Serial.print(F(":"));
    Serial.print(m4, HEX);
    Serial.print(F(":"));
    Serial.print(m5, HEX);
    Serial.println();
}
#endif

void WiznetTCP::open()
{
    write_byte(SOCKET_ZERO + SOCKET_MODE, SOCKET_TCP);
    write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_OPEN);
}

bool WiznetTCP::listen()
{
    if (get_socket_status() == SOCK_INIT)
    {
        write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_LISTEN);
    
        // wait for command to be processed
        while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
        
        if (get_socket_status() == SOCK_LISTEN)
            return true;
            
        close();
    }
    
    return false;
}

bool WiznetTCP::connect(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3, uint16_t port)
{
    if (get_socket_status() == SOCK_INIT)
    {
        set_ip(n0, n1, n2, n3);
        set_port(port);
        
        write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_CONNECT);
    
        // wait for command to be processed
        while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
        return true;
    }
    
    return false;
}

void WiznetTCP::close()
{
    write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_CLOSE);
    
    // wait for command to be processed
    while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));

    // clear interrupts
    write_byte(SOCKET_ZERO + SOCKET_INTERRUPT, 0xFF);
}

void WiznetTCP::disconnect()
{
    write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_DISCONNECT);
    
    // wait for command to be processed
    while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));

    // clear interrupts
    write_byte(SOCKET_ZERO + SOCKET_INTERRUPT, 0xFF);
}

void WiznetTCP::set_ip(uint32_t ip)
{
    // ip is little endian but W5100 expects big endian
    write32(SOCKET_ZERO + SOCKET_IP_ADDRESS,
         (ip>>24) & 255, (ip>>16) & 255, (ip>>8) & 255, ip & 255);
}

// use ip(192, 43, 244, 18) for "192.43.244.18"
void WiznetTCP::set_ip(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)
{
    write32(SOCKET_ZERO + SOCKET_IP_ADDRESS, n0, n1, n2, n3);
}

void WiznetTCP::get_ip(uint8_t *n0, uint8_t *n1, uint8_t *n2, uint8_t *n3)
{
    *n0 = read_byte(SOCKET_ZERO + SOCKET_IP_ADDRESS);
    *n1 = read_byte(SOCKET_ZERO + SOCKET_IP_ADDRESS + 1);
    *n2 = read_byte(SOCKET_ZERO + SOCKET_IP_ADDRESS + 2);
    *n3 = read_byte(SOCKET_ZERO + SOCKET_IP_ADDRESS + 3);
}

void WiznetTCP::get_local_ip(uint8_t *n0, uint8_t *n1, uint8_t *n2, uint8_t *n3)
{
    *n0 = read_byte(W5100_LOCAL_IP_ADDRESS);
    *n1 = read_byte(W5100_LOCAL_IP_ADDRESS + 1);
    *n2 = read_byte(W5100_LOCAL_IP_ADDRESS + 2);
    *n3 = read_byte(W5100_LOCAL_IP_ADDRESS + 3);
}

void WiznetTCP::set_port(uint16_t port)
{
    write_word(SOCKET_ZERO + SOCKET_DEST_PORT, port);
}

// does this get the local or remote port?
// W5100 spec is ambiguous with use of Source and Destination
// for IP address, socket dest IP is the IP of the remote device
// but for ports, the question is how this varies from TX to RX
uint16_t WiznetTCP::get_port()
{
    return read_word(SOCKET_ZERO + SOCKET_DEST_PORT);
}

uint16_t WiznetTCP::get_local_port()
{
    return read_word(SOCKET_ZERO+SOCKET_SRC_PORT);;
}

uint8_t WiznetTCP::get_socket_status()
{
    return read_byte(SOCKET_ZERO + SOCKET_STATUS);
}

uint16_t WiznetTCP::send_available()
{
    return read_word(SOCKET_ZERO + SOCKET_TX_FREE_SIZE);
}

uint16_t WiznetTCP::receive_available()
{
    return read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
}

// wait for data until ms milliseconds then return 0
uint16_t WiznetTCP::receive_available(uint16_t ms)
{
    unsigned long last_time = 0, now = millis();
    
    while (ms) {
        uint16_t size = read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
        
        if (size)
            return size;
            
        now = millis();
        
        if (now != last_time)
            --ms;
        
        last_time = now;
    }

    return 0;
}

uint16_t WiznetTCP::flush_receive()
{
    return skip(read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE));
}

uint16_t WiznetTCP::skip(uint16_t length)
{
    uint16_t size = read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
    
    if (size) {
        uint16_t ptr = read_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR);
    
        if (size > length)
            size = length;
        
        // update socket's read pointer
        write_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR, ptr+size);
    
        // re-enable receiving on this socket
        write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_RECEIVE);
    
        // wait for command to be processed
        while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
    }
    return size;
}

uint16_t WiznetTCP::peek(uint16_t offset, char *buffer, uint16_t length)
{
    uint16_t size = read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
    
    if (size > offset) {
        uint16_t ptr = offset + read_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR);
    
        if (size > length)
            size = length;
        
        get_data(ptr, (uint8_t *)buffer, size);
    }
    else
        size = 0;
        
    return size;
}

// returns what's currently available
uint16_t WiznetTCP::receive(char *buffer, uint16_t length)
{
    uint16_t size = read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
    
    if (size) {
        uint16_t ptr = read_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR);
    
        if (size > length)
            size = length;
        
        get_data(ptr, (uint8_t *)buffer, size);
    
        // update socket's read pointer
        write_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR, ptr+size);
    
        // re-enable receiving on this socket
        write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_RECEIVE);
    
        // wait for command to be processed
        while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
    }
    return size;
}

uint16_t WiznetTCP::receive(char *buffer, uint16_t length, uint32_t *ip, uint16_t *port)
{
    uint16_t size = read_word(SOCKET_ZERO + SOCKET_RX_RECV_SIZE);
    
    if (size) {
        uint8_t header[8];
        uint16_t ptr = read_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR);
        get_data(ptr, header, 8); // IP4, port, length
    
        // ip is little endian but W5100 provides big endian
        *ip = header[0];
        *ip <<= 8; *ip |= header[1];
        *ip <<= 8; *ip |= header[2];
        *ip <<= 8; *ip |= header[3];
        *port = 256 * header[4] + header[5];
        size = 256 * header[6] + header[7];
    
        if (size > length)
            size = length;
        
        ptr += 8; // past header
        get_data(ptr, (uint8_t *)buffer, size);
    
        // update socket's read pointer
        write_word(SOCKET_ZERO + SOCKET_RX_READ_PNTR, ptr+size);
    
        // re-enable receiving on this socket
        write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_RECEIVE);
    
        // wait for command to be processed
        while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
    }
    return size;
}

uint16_t WiznetTCP::send(char *buffer, uint16_t length)
{
    uint16_t size = send_available();
    
    // send up to the available space
    if (length < size)
        size = length;
    
    uint8_t mode = read_byte(SOCKET_ZERO + SOCKET_MODE);
    uint16_t ptr = read_word(SOCKET_ZERO + SOCKET_TX_WRITE_PNTR);
    put_data(ptr, (uint8_t *)buffer, size);
    
    // update socket's send pointer
    write_word(SOCKET_ZERO + SOCKET_TX_WRITE_PNTR, ptr+size);

    // ask W5100 to send the packet
    write_byte(SOCKET_ZERO + SOCKET_COMMAND,
         (mode & SOCKET_MULTICAST ? SOCKET_SEND_MAC : SOCKET_SEND));
    
    // wait for command to be processed
    while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
    
    // wait for data to be sent or for a timeout
    while ((read_byte(SOCKET_ZERO + SOCKET_INTERRUPT) & W5100_SEND_OK) != W5100_SEND_OK);
    
    if (read_byte(SOCKET_ZERO + SOCKET_INTERRUPT) & W5100_TIMEOUT)
        Serial.println(F("send timeout"));
        
    // clear flags by writing high values as per W5100 datasheet
    write_byte(SOCKET_ZERO + SOCKET_INTERRUPT, (W5100_SEND_OK|W5100_TIMEOUT));
    
    Serial.print(F("sent "));
    Serial.print(size);
    Serial.println(F(" bytes"));
    return size;
}

#if 0
uint16_t WiznetTCP::send_mac(char *buffer, uint16_t length)
{
    uint16_t size = send_available();
    
    // send up to the available space
    if (length < size)
        size = length;
        
    uint16_t ptr = read_word(SOCKET_ZERO + SOCKET_TX_WRITE_PNTR);
    put_data(ptr, (uint8_t *)buffer, size);
    
    // update socket's send pointer
    write_word(SOCKET_ZERO + SOCKET_TX_WRITE_PNTR, ptr+size);

    // ask W5100 to send the packet
    write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_SEND_MAC);
    
    // wait for command to be processed
    while (read_byte(SOCKET_ZERO + SOCKET_COMMAND));
    
    // wait for data to be sent or for a timeout
    while ((read_byte(SOCKET_ZERO + SOCKET_INTERRUPT) & W5100_SEND_OK) != W5100_SEND_OK);
    
    if (read_byte(SOCKET_ZERO + SOCKET_INTERRUPT) & W5100_TIMEOUT)
        Serial.println(F("send timeout"));
        
    // clear flags by writing high values as per W5100 datasheet
    write_byte(SOCKET_ZERO + SOCKET_INTERRUPT, (W5100_SEND_OK|W5100_TIMEOUT));
    
    Serial.print(F("sent "));
    Serial.print(size);
    Serial.println(F(" bytes"));
    return size;
}
#endif

// assumes buffer is large enough for at least size bytes
void WiznetTCP::get_data(uint16_t ptr, uint8_t *buffer, uint16_t size)
{
    uint16_t mask = ptr & RX_BUFFER_MASK;
    uint16_t i, src = BASE_RX_BUFFER + mask;
    
    // will data extend past top of RX buffer?
    
    if (mask + size > SOCKET_SIZE) {
        // data is not contiguous
        
        uint16_t len = SOCKET_SIZE - mask;
        
        for (i = 0; i < len; ++i, ++src)
            buffer[i] = read_byte(src);

        for (src = BASE_RX_BUFFER; i < size; ++i, ++src)
            buffer[i] = read_byte(src);
        
    } else {
        // data is contiguous
           
        for (i = 0; i < size; ++i, ++src)
            buffer[i] = read_byte(src);
    }
}

// assumes that socket transmit buffer has at least size bytes free
void WiznetTCP::put_data(uint16_t ptr, uint8_t *buffer, uint16_t size)
{
    uint16_t mask = ptr & TX_BUFFER_MASK;
    uint16_t i, dst = BASE_TX_BUFFER + mask;
    
    // will data extend past top of TX buffer?
    
    if (size + mask > SOCKET_SIZE) {
        // data is not contiguous
        
        uint16_t len = SOCKET_SIZE - mask;
        
        for (i = 0; i < len; ++i, ++dst)
            write_byte(dst, buffer[i]);

        for (dst = BASE_TX_BUFFER; i < size; ++i, ++dst)
            write_byte(dst, buffer[i]);
        
    } else {
        // data is contiguous
           
        for (i = 0; i < size; ++i, ++dst)
            write_byte(dst, buffer[i]);
    }
}

// SPI code

/*
    PB2 slave select (SS), active low from master
    PB3 master output, slave input (MOSI)
    PB4 master input, slave output (MISO)
    PB5 serial clock generated by master (SCK)
    PC4 slave select for SD Card on Ethernet Shield
*/

void WiznetTCP::spi_init()
{
    // configure SS, MOSI and SCK as output pins
    DDRB = _BV(2) | _BV(3) | _BV(5);
    PORTB |= _BV(2); // set W5100 slave select high
    PORTC |= _BV(4); // activate internal pull up to disable SD Card

    // no interrupt, enable SPI, MCU as master, SPI mode 0, 4 Mhz clock
    SPCR = (1<<SPE)|(1<<MSTR);
    Serial.println(F("initialised SPI"));
}

inline static void setSS()
{
    PORTB &= ~_BV(2);
}

inline static void resetSS()
{
    PORTB |= _BV(2);
}

// single byte transfer via AVR's SPI hardware
inline static uint8_t transfer(uint8_t data)
{
    SPDR = data;
    asm volatile("nop");
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}

// read one byte from SPI
inline static uint8_t read_byte(uint16_t addr)
{
    uint8_t data;
    
    setSS();  
    transfer(0x0F);
    transfer(addr >> 8);
    transfer(addr & 0xFF);
    data = transfer(0);
    resetSS();
    return data;
}

static void write32(uint16_t addr, uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)
{
    write_byte(addr++, n0);
    write_byte(addr++, n1);
    write_byte(addr++, n2);
    write_byte(addr, n3);
}

static void write32(uint16_t addr, uint8_t *p)
{
    for (uint8_t i = 0; i < 4; ++i)
        write_byte(addr++, *p++);
}

static void write48(uint16_t addr, uint8_t n0, uint8_t n1, uint8_t n2,
                     uint8_t n3, uint8_t n4, uint8_t n5)
{
    write_byte(addr++, n0);
    write_byte(addr++, n1);
    write_byte(addr++, n2);
    write_byte(addr++, n3);
    write_byte(addr++, n4);
    write_byte(addr, n5);
}

static void write48(uint16_t addr, uint8_t *p)
{
    for (uint8_t i = 0; i < 6; ++i)
        write_byte(addr++, *p++);
}

// write one bye to SPI
inline static void write_byte(uint16_t addr, uint8_t data)
{
    setSS();  
    transfer(0xF0);
    transfer((addr >> 8) & 255);
    transfer(addr & 255);
    transfer(data);
    resetSS();
}

static uint16_t read_word(uint16_t addr)
{
    uint16_t word = read_byte(addr) << 8;
    word |= read_byte(addr+1);
    return word;
}

static void write_word(uint16_t addr, uint16_t word)
{
    write_byte(addr, word >> 8);
    write_byte(addr+1, word & 255);
}

// DHCP client - ~3KB code including the debug statements
// See http://tools.ietf.org/html/rfc2132 for details

uint8_t WiznetTCP::run_DHCP_client(void)
{
    #define HOST_NAME	"DAVES-ARDUINO" // host name - should be loaded from EEPROM
                                        // *** FIX ME with EEPROM config module
    uint8_t SRC_MAC_ADDR[6];            // local MAC address
    uint8_t GET_SN_MASK[4];				// subnet mask received from the DHCP server
    uint8_t GET_GW_IP[4];				// gateway ip address received from the DHCP server
    uint8_t GET_SIP[4] = {0, 0, 0, 0};	// local ip address received from the DHCP server
    uint8_t DHCP_SIP[4] = {0, 0, 0, 0}; // DHCP server ip address is discovered
    
    uint32_t DHCP_XID = 0x75269875;
    RIP_MSG* pRIPMSG;				    // pointer for the DHCP message
    RIP_MSG RIPMSG;                     // structure definition
    
    uint16_t recv_msg_size;
    uint8_t *recv_msg_end;
    uint8_t *current_option;
    uint8_t  option_length;
    uint8_t host_name_length;
    
    uint16_t i=0;
    uint16_t port;
    uint32_t ip;
    
    Serial.println(F("running DHCP client"));
    
    // set local IP address to zero
    write32(W5100_LOCAL_IP_ADDRESS, 0, 0, 0, 0);

    
    // MAC address - fixed for now but should be loaded from EEPROM
    SRC_MAC_ADDR[0] = 0x61,
    SRC_MAC_ADDR[1] = 0xf8,
    SRC_MAC_ADDR[2] = 0x1d,
    SRC_MAC_ADDR[3] = 0xbc,
    SRC_MAC_ADDR[4] = 0xf4,
    SRC_MAC_ADDR[5] = 0x2f;
    write48(W5100_MAC_ADDRESS, SRC_MAC_ADDR);

    // set up UDP socket for DHCP client port
    write_word(SOCKET_ZERO + SOCKET_SRC_PORT, DHCP_CLIENT_PORT);
    write_byte(SOCKET_ZERO + SOCKET_MODE, SOCKET_UDP);
    write_byte(SOCKET_ZERO + SOCKET_COMMAND, SOCKET_OPEN);

    // send DHCPDISCOVER
    
	pRIPMSG = &RIPMSG;
	
	memset((void*)pRIPMSG, 0, sizeof(RIP_MSG)); // zero RIPMSG contents

	pRIPMSG->op = DHCP_BOOTREQUEST;
	pRIPMSG->htype = DHCP_HTYPE10MB;
	pRIPMSG->hlen = DHCP_HLENETHERNET;
	pRIPMSG->hops = DHCP_HOPS;
	pRIPMSG->xid = DHCP_XID;
	pRIPMSG->secs = DHCP_SECS;
	pRIPMSG->flags = DHCP_FLAGSBROADCAST;
	pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
	pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
	pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
	pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
	pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
	pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];

	// MAGIC_COOKIE IN NETWORK ORDER
	pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 24)& 0xFF);
	pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 16)& 0xFF);
	pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 8)& 0xFF);
	pRIPMSG->options[i++] = (char)(MAGIC_COOKIE& 0xFF);

	// Option Request Param.
	pRIPMSG->options[i++] = dhcpMessageType;
	pRIPMSG->options[i++] = 0x01;
	pRIPMSG->options[i++] = DHCP_DISCOVER;

	// Client identifier
	pRIPMSG->options[i++] = dhcpClientIdentifier;
	pRIPMSG->options[i++] = 0x07;
	pRIPMSG->options[i++] = 0x01; // Ethernet (10Mb)
	pRIPMSG->options[i++] = SRC_MAC_ADDR[0];
	pRIPMSG->options[i++] = SRC_MAC_ADDR[1];
	pRIPMSG->options[i++] = SRC_MAC_ADDR[2];
	pRIPMSG->options[i++] = SRC_MAC_ADDR[3];
	pRIPMSG->options[i++] = SRC_MAC_ADDR[4];
	pRIPMSG->options[i++] = SRC_MAC_ADDR[5];
	
	// host name
	pRIPMSG->options[i++] = hostName;
	pRIPMSG->options[i++] = host_name_length = strlen(HOST_NAME);
	strcpy((char*)&(pRIPMSG->options[i]),HOST_NAME);
	i+=host_name_length;

	// IP Address Lease Time request
    pRIPMSG->options[i++] = dhcpIPaddrLeaseTime;
    pRIPMSG->options[i++] = 4;
    pRIPMSG->options[i++] = 0xFF;
    pRIPMSG->options[i++] = 0xFF;
    pRIPMSG->options[i++] = 0xFF;
    pRIPMSG->options[i++] = 0xFF;
    
    // Parameter Request List
	pRIPMSG->options[i++] = dhcpParamRequest;
	pRIPMSG->options[i++] = 0x02;
	pRIPMSG->options[i++] = subnetMask;
	pRIPMSG->options[i++] = routersOnSubnet;
	pRIPMSG->options[i++] = endOption;

	// send broadcasting packet
	Serial.println(F("send DHCP DISCOVER"));
	set_ip(255, 255, 255, 255);
	set_port(DHCP_SERVER_PORT);
	
	for (i = MAX_DHCP_RETRY; i; --i) {
	    Serial.print(F("socket status: 0x"));
        Serial.println(get_socket_status(), HEX);
	    send((char *)pRIPMSG, sizeof(RIP_MSG));
	    
	    if (receive_available(DHCP_WAIT_TIME)) {
	        Serial.println(F("got DHCP OFFER"));
	        break;
	    }
	}
	
	if (i == 0) {
	     Serial.println(F("failed to get DHCP OFFER"));
	     return 0;
	}
    
    recv_msg_size = receive((char *)pRIPMSG, sizeof(RIPMSG), &ip, &port);
            
    if (port==DHCP_SERVER_PORT &&
        !memcmp(RIPMSG.chaddr, SRC_MAC_ADDR, 6) &&
        RIPMSG.xid == DHCP_XID)
    {
        // Check options
        recv_msg_end = (uint8_t *)((uint16_t)&(RIPMSG.op) + recv_msg_size);
        current_option = (uint8_t *)((uint16_t)&(RIPMSG.op) + 240);
        
        while(current_option < recv_msg_end)
        {
            switch (*(current_option++))
            {
                case padOption:
                    break;
                        
                case endOption:
                    break;
                        
                case dhcpMessageType:
                    current_option++;
                    
                    if ((*current_option++) != DHCP_OFFER)
                        return 0;
                                
                    break;
                            
                case subnetMask:
                    option_length = *current_option++;
                    memcpy(GET_SN_MASK, current_option, 4);
                    current_option += option_length;
                    break;
                            
                case routersOnSubnet:
                    option_length = *current_option++;
                    memcpy(GET_GW_IP, current_option, 4);
                    current_option += option_length;
                    break;
                            
                case dhcpServerIdentifier:
                    current_option++;
                    memcpy(DHCP_SIP, current_option, 4);
                    current_option += 4;
                    break;
                            
                default:
                    option_length = *current_option++;
                    current_option += option_length;
                    break;
            }
        }
                
        memcpy(GET_SIP, (RIPMSG.yiaddr), 4);

    }
    else {
        close();
        return 0;
    }
    
    // send DHCPREQUEST
    
    memset((void*)pRIPMSG, 0, sizeof(RIP_MSG));

    pRIPMSG->op = DHCP_BOOTREQUEST;
    pRIPMSG->htype = DHCP_HTYPE10MB;
    pRIPMSG->hlen = DHCP_HLENETHERNET;
    pRIPMSG->hops = DHCP_HOPS;
    pRIPMSG->xid = DHCP_XID;
    pRIPMSG->secs = DHCP_SECS;
    pRIPMSG->flags = DHCP_FLAGSBROADCAST;
    pRIPMSG->chaddr[0] = SRC_MAC_ADDR[0];
    pRIPMSG->chaddr[1] = SRC_MAC_ADDR[1];
    pRIPMSG->chaddr[2] = SRC_MAC_ADDR[2];
    pRIPMSG->chaddr[3] = SRC_MAC_ADDR[3];
    pRIPMSG->chaddr[4] = SRC_MAC_ADDR[4];
    pRIPMSG->chaddr[5] = SRC_MAC_ADDR[5];

    i=0;
        
    // MAGIC_COOKIE
    pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 24)& 0xFF);
    pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 16)& 0xFF);
    pRIPMSG->options[i++] = (char)((MAGIC_COOKIE >> 8)& 0xFF);
    pRIPMSG->options[i++] = (char)(MAGIC_COOKIE& 0xFF);

    // option request param
    pRIPMSG->options[i++] = dhcpMessageType;
    pRIPMSG->options[i++] = 0x01;
    pRIPMSG->options[i++] = DHCP_REQUEST;

    // client identifier
    pRIPMSG->options[i++] = dhcpClientIdentifier;
    pRIPMSG->options[i++] = 0x07;
    pRIPMSG->options[i++] = 0x01;
    pRIPMSG->options[i++] = SRC_MAC_ADDR[0];
    pRIPMSG->options[i++] = SRC_MAC_ADDR[1];
    pRIPMSG->options[i++] = SRC_MAC_ADDR[2];
    pRIPMSG->options[i++] = SRC_MAC_ADDR[3];
    pRIPMSG->options[i++] = SRC_MAC_ADDR[4];
    pRIPMSG->options[i++] = SRC_MAC_ADDR[5];
    
    // request IP address
    pRIPMSG->options[i++] = dhcpRequestedIPaddr;
    pRIPMSG->options[i++] = 0x04;
    pRIPMSG->options[i++] = GET_SIP[0];
    pRIPMSG->options[i++] = GET_SIP[1];
    pRIPMSG->options[i++] = GET_SIP[2];
    pRIPMSG->options[i++] = GET_SIP[3];
    
    // server identifier
    pRIPMSG->options[i++] = dhcpServerIdentifier;
    pRIPMSG->options[i++] = 0x04;
    pRIPMSG->options[i++] = DHCP_SIP[0];
    pRIPMSG->options[i++] = DHCP_SIP[1];
    pRIPMSG->options[i++] = DHCP_SIP[2];
    pRIPMSG->options[i++] = DHCP_SIP[3];
    
    // host name
	pRIPMSG->options[i++] = hostName;
	pRIPMSG->options[i++] = host_name_length = strlen(HOST_NAME);
	strcpy((char*)&(pRIPMSG->options[i]),HOST_NAME);
	i+=host_name_length;
    
    // parameter request list
    pRIPMSG->options[i++] = dhcpParamRequest;
    pRIPMSG->options[i++] = 0x02;
    pRIPMSG->options[i++] = subnetMask;
    pRIPMSG->options[i++] = routersOnSubnet;
    pRIPMSG->options[i++] = endOption;

    // send broadcasting packet
    Serial.println(F("send DHCP REQUEST"));
	set_ip(255, 255, 255, 255);
	set_port(DHCP_SERVER_PORT);
	
	for (i = MAX_DHCP_RETRY; i; --i) {
	    send((char *)pRIPMSG, sizeof(RIP_MSG));
	    
	    if (receive_available(DHCP_WAIT_TIME)) {
	        Serial.println(F("got DHCP ACK"));
	        break;
	    }
	}
	
	if (i == 0) {
	     Serial.println(F("failed to get DHCP ACK"));
	     return 0;
	}
    
    recv_msg_size = receive((char *)pRIPMSG, sizeof(RIPMSG), &ip, &port);
            
    if (port==DHCP_SERVER_PORT &&
        !memcmp(RIPMSG.chaddr, SRC_MAC_ADDR, 6) &&
        RIPMSG.xid == DHCP_XID) {
        // Check options
        recv_msg_end = (uint8_t *)((uint16_t)&(RIPMSG.op) + recv_msg_size);
        current_option = (uint8_t *)((uint16_t)&(RIPMSG.op) + 240);
        
        while(current_option < recv_msg_end) {
            switch (*(current_option++)) {
                case padOption:
                break;
                        
                case endOption:
                    break;
                        
                case dhcpMessageType:
                    current_option++;
                    if ((*current_option++) != DHCP_ACK)
                        return 0;
                        
                    break;
                        
                case subnetMask:
                    option_length = *current_option++;
                    memcpy(GET_SN_MASK, current_option, 4);
                    current_option += option_length;
                    break;
                        
                case routersOnSubnet:
                    option_length = *current_option++;
                    memcpy(GET_GW_IP, current_option, 4);
                    current_option += option_length;
                    break;
                        
                default:
                    option_length = *current_option++;
                    current_option += option_length;
                    break;
            }
        }
                
        memcpy(GET_SIP, (RIPMSG.yiaddr), 4);
        memcpy(DHCP_SIP, (RIPMSG.siaddr), 4);
    }
    else {
        close();
        return 0;
    }
    
    // set network parameters
    write32(W5100_GATEWAY, GET_GW_IP);        
    write32(W5100_SUBNET_MASK, GET_SN_MASK);    
    write32(W5100_LOCAL_IP_ADDRESS, RIPMSG.yiaddr);
    
    // close the socket used to talk to DHCP server
    close();
    
    Serial.println(F("Got IP address via DHCP"));
    return 1;
}
