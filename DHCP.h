/*
   Basic DHCP client to configure device's local IP address
   With thanks to Nabeel Ahmad (nbl14@hotmail.com) who
   in turn adapted it from code by Wiznet
*/

#ifndef _DHCP_H_
#define _DHCP_H_

// DHCP state machine
#define	STATE_DHCP_DISCOVER 1
#define	STATE_DHCP_REQUEST 2
#define	STATE_DHCP_LEASED 3
#define	STATE_DHCP_REREQUEST 4
#define	STATE_DHCP_RELEASE 5

#define	MAX_DHCP_RETRY 10   // maximum number of retries
#define	DHCP_WAIT_TIME 2000 // milliseconds between retries

#define DHCP_FLAGSBROADCAST 0x0080

// UDP port numbers for DHCP
#define	DHCP_SERVER_PORT 67	// from server to client
#define DHCP_CLIENT_PORT 68	// from client to server

// DHCP message OP code
#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTREPLY 2

// DHCP message type
#define	DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define	DHCP_REQUEST 3
#define	DHCP_DECLINE 4
#define	DHCP_ACK 5
#define DHCP_NAK 6
#define	DHCP_RELEASE 7
#define DHCP_INFORM 8

// DHCP RETRANSMISSION TIMEOUT (microseconds)
#define DHCP_INITIAL_RTO (4*1000000)
#define DHCP_MAX_RTO (64*1000000)

#define DHCP_HTYPE10MB 1
#define DHCP_HTYPE100MB 2

#define DHCP_HLENETHERNET 6
#define DHCP_HOPS 0
#define DHCP_SECS 0

#define MAGIC_COOKIE 0x63825363

#define DEFAULT_LEASETIME 0xffffffff // infinite lease time
#define MAX_DHCP_options 16

// DHCP option and value (cf. RFC1533) 

enum
{
	padOption					=	0,
	subnetMask					=	1,
	timerOffset					=	2,
	routersOnSubnet				=	3,
	timeServer					=	4,
	nameServer					=	5,
	dns							=	6,
	logServer					=	7,
	cookieServer				=	8,
	lprServer					=	9,
	impressServer				=	10,
	resourceLocationServer		=	11,
	hostName					=	12,
	bootFileSize				=	13,
	meritDumpFile				=	14,
	domainName					=	15,
	swapServer					=	16,
	rootPath					=	17,
	extentionsPath				=	18,
	IPforwarding				=	19,
	nonLocalSourceRouting		=	20,
	policyFilter				=	21,
	maxDgramReasmSize			=	22,
	defaultIPTTL				=	23,
	pathMTUagingTimeout			=	24,
	pathMTUplateauTable			=	25,
	ifMTU						=	26,
	allSubnetsLocal				=	27,
	broadcastAddr				=	28,
	performMaskDiscovery		=	29,
	maskSupplier				=	30,
	performRouterDiscovery		=	31,
	routerSolicitationAddr		=	32,
	staticRoute					=	33,
	trailerEncapsulation		=	34,
	arpCacheTimeout				=	35,
	ethernetEncapsulation		=	36,
	tcpDefaultTTL				=	37,
	tcpKeepaliveInterval		=	38,
	tcpKeepaliveGarbage			=	39,
	nisDomainName				=	40,
	nisServers					=	41,
	ntpServers					=	42,
	vendorSpecificInfo			=	43,
	netBIOSnameServer			=	44,
	netBIOSdgramDistServer		=	45,
	netBIOSnodeType				=	46,
	netBIOSscope				=	47,
	xFontServer					=	48,
	xDisplayManager				=	49,
	dhcpRequestedIPaddr			=	50,
	dhcpIPaddrLeaseTime			=	51,
	dhcpOptionOverload			=	52,
	dhcpMessageType				=	53,
	dhcpServerIdentifier		=	54,
	dhcpParamRequest			=	55,
	dhcpMsg						=	56,
	dhcpMaxMsgSize				=	57,
	dhcpT1value					=	58,
	dhcpT2value					=	59,
	dhcpClassIdentifier			=	60,
	dhcpClientIdentifier		=	61,
	endOption					=	255
};

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

// for the DHCP message see http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm

typedef struct _RIP_MSG
{
	uint8_t  	op; 
	uint8_t  	htype; 
	uint8_t  	hlen;
	uint8_t  	hops;
	uint32_t	xid;
	uint16_t	secs;
	uint16_t	flags;
	uint8_t  	ciaddr[4];
	uint8_t  	yiaddr[4];
	uint8_t  	siaddr[4];
	uint8_t  	giaddr[4];
	uint8_t  	chaddr[16];
	uint8_t  	sname[64];
	uint8_t  	file[128];
	uint8_t  	options[312];
} RIP_MSG;

#endif