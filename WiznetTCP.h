#ifndef	_WIZNET_TCP
#define	_WIZNET_TCP

/*
   TCP driver for Wiznet W5100 chip via SPI interface
   
   This supports client and server connections. For server, the
   listen method waits until a client connects to the socket.
   The server thus can only process one connection at a time.
*/

// W5100 Socket status

#define SOCK_CLOSED 0
#define SOCK_INIT 19
#define SOCK_LISTEN 20
#define SOCK_SYNSENT 21
#define SOCK_SYNRECV 22
#define SOCK_ESTABLISHED 23
#define SOCK_FIN_WAIT 24
#define SOCK_CLOSING 26
#define SOCK_TIME_WAIT 27
#define SOCK_CLOSE_WAIT 28
#define SOCK_LAST_ACK 29
#define SOCK_UDP 34
#define SOCK_IPRAW 50
#define SOCK_MACRAW 66
#define SOCK_PPPOE 95

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

class WiznetTCP
{
    private:
        uint32_t gateway_ip;
        uint16_t gateway_port;
        
        void spi_init();
        void get_data(uint16_t ptr, uint8_t *buffer, uint16_t length);
        void put_data(uint16_t ptr, uint8_t *buffer, uint16_t length);
        uint8_t run_DHCP_client(void);
        uint8_t handle_mDNS_response(uint8_t *query);
        uint16_t get_link(uint16_t offset);
        uint16_t parse_name(uint16_t offset, uint8_t *buffer,
                             uint8_t *sn, uint8_t *matched);
        void dump_buffer(uint8_t *buffer, uint16_t length);
        
    public:
        WiznetTCP();
        ~WiznetTCP();
        void begin(uint16_t port);
        void begin(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3, uint16_t port);
        bool listen();
        void disconnect();
        bool connect(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3, uint16_t port);
        void open();
        void close();
        void discover_gateway();
        
        void print_mac_address();
        
        uint8_t get_socket_status();
        uint16_t send_available();
        uint16_t send(char *buffer, uint16_t length);
        uint16_t send_mac(char *buffer, uint16_t length);
        uint16_t receive_available();
        uint16_t receive_available(uint16_t ms);
        uint16_t flush_receive();
        uint16_t skip(uint16_t length);
        uint16_t peek(uint16_t offset, char *buffer, uint16_t length);
        uint16_t receive(char *buffer, uint16_t length);
        uint16_t receive(char *buffer, uint16_t length, uint32_t *ip, uint16_t *port);
        
        void set_ip(uint32_t ip);
        void set_ip(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3);
        void get_ip(uint8_t *n0, uint8_t *n1, uint8_t *n2, uint8_t *n3);
        void get_local_ip(uint8_t *n0, uint8_t *n1, uint8_t *n2, uint8_t *n3);
        void set_port(uint16_t port);
        uint16_t get_port();
        uint16_t get_local_port();
};

#endif