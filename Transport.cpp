#include <stdarg.h>
#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"
#include "Names.h"
#include "JSON.h"
#include "WSEvent.h"
#include "WiznetTCP.h"
#include "WebThings.h"
#include "Transport.h"

void Transport::start()
{
  delay(1000);
  tcp.begin(192,168,1,127, 1234);
  //tcp.begin(1234); // using DHCP for IP config
  tcp.discover_gateway();
  Serial.println(F("started server"));
}

void Transport::stop()
{
    tcp.close();
}

#define TCP_BUF_LEN 256

void Transport::serve()
{
  unsigned int n;
  
  switch (tcp.get_socket_status()) {
    case SOCK_INIT:
      tcp.listen();
      break;
      
    case SOCK_CLOSED:
      tcp.open();
      break;
      
    case SOCK_LISTEN:
        break;
      
    case SOCK_ESTABLISHED:
      n = tcp.receive_available();
    
      if (n) {
        char buffer[TCP_BUF_LEN];

        if (n > TCP_BUF_LEN - 1)
          n = TCP_BUF_LEN - 1;
          
        n = tcp.receive(buffer, n);
        buffer[n] = '\0';
        
        Serial.print("received ");
        Serial.print(n);
        Serial.print(" bytes: \"");
        Serial.print(buffer);
        Serial.println("\"");
            
        tcp.send(buffer, n);
        
        // assume no further requests
        tcp.disconnect();
      }
      
      break;
      
    case SOCK_FIN_WAIT:
    case SOCK_CLOSING:
    case SOCK_TIME_WAIT:
    case SOCK_CLOSE_WAIT:
    case SOCK_LAST_ACK:
      // force socket to close
      tcp.close();
      break;
      
    default:
      break;
  }
}
