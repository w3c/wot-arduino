Web of Things server for the Arduino Uno + Ethernet shield (W5100)
==================================================================

News: the Arduino MKR1000 looks like what I'm looking for as the step up from the Arduino Mega!
The Mega has 8KB RAM, 256KB Flash, 4KB EEPROM. The MKR1000 has 32KB RAM, 256KB Flash, no EEPROM, but includes WiFi and a 32 bit Cortex-M0+ MCU and would be interesting to compare with ESP8266. The latter still wins on price!

This is a experimental web of things server for the Arduino Uno plus the Ethernet shield based upon the Wiznet W5100 network controller.  The server supports a lightweight protocol using a binary encoding of JSON messages over TCP/IP. This takes advantage of the native TCP support found in Wiznet chips. Future work will look at other protocols that are better suited to devices that need to run off ambient power or operate for a very long time on batteries.

None of the Arduino boards are ideal since they all include inefficient linear voltage regulators and LEDs. The The MKR1000 is an exception but looks like it won't be cheap! The ESP8266 ESP-01 board doesn't include a voltage regulator, but you need to de-solder the LED. The Arduino mini is another option provided you de-solder the LED and power regulator. The mini lacks the serial chip so an external board is needed to connect it to the computer. A switching voltage regulator will then allow the board to run efficiently off batteries.

My initial goal is to use a fixed IP address and demonstrate interoperability with a server running on my laptop. This involves developing both servers at the same time. I may be able to adapt my NodeJS based server, or I may develop a C++ server that re-uses much of the code from the Arduino project. This is quite attractive as it means I only need to think using one programming language rather than two.  I have already demonstrated how to emulate the Arduino framework on OS X using POSIX.

The desktop server would connect to the Arduino and request the model for a thing, and use this model to construct a proxy for that thing. The Arduino would store the model in Flash and copy it to the socket connection. I envisage using the same abstract message formats as for my NodeJS project. The only difference is the use of a binary encoding to reduce message size. The Arduino has very little RAM, so we need to encode the messages directly to the socket rather than buffering then in RAM.

A symbol table is computed from the data models to encode property names etc. as numeric identifiers. This requires both servers to use the same algorithm to ensure that the mappings are the same.

JSON is represented in memory as nodes assigned from a statically allocated array. This array is also used to allocate nodes for balanced binary trees representing JSON associative arrays and arrays with numeric indices. A hash table is used for the symbol dictionary, but I plan to discard the table after parsing the models in order to free up memory for application scripts.

So far I have the parser and message encoder/decoder done. I need to integrate the network library, and to finish the logic that wires the properties, actions and events to the API exposed to applications scripts.

I then want to look at an API for registering a remote proxy, and also for adding support for discovery, so that the device can discover a hub using a very lightweight multicast socket or the broadcast address (UDP message sent to 255.255.255.255).

I have extended the MessageCoder to work with the F macro for string literals stored in Flash as well as regular string literals. For thing data models defined by this device, it also makes sense to hold them in Flash and avoid the compiler copying them to RAM when initialising local variables. The JSON parser will now work with and without the F macro. I was able to combine the RAM and Program address spaces by adding an offset of 0x8000 to program memory addresses, since the Uno only has 32KB flash. I've introduce a Strings class to support string operations on RAM and flash based strings.

Here is an example sketch:

  Thing *agent, *door, *light; Symbol unlock_sym, on_sym;
  
  void setup() {
    RegisterThing(agent_model, init_agent);
  }
  
  void init_agent(Thing *thing, Names *names) {
    agent = thing;
    door = agent->get_property(names, F("door"));
    light = agent->get_property(names, F("light"));
    unlock_sym = names->symbol(F("unlock"));
    on_sym = names->symbol(F("on"));
    agent->observe(names, F("key"), unlock);
  }
  
  void unlock(JSON *valid) {
    if (JSON_BOOLEAN(valid)) {
      door->invoke(unlock_sym);
      light->set_property(on_sym, JSON_TRUE);
    }
  }
  
  void loop() {
    DispatchEvents();
  }

This maps names to symbols, which are used to index the balanced binary trees that internally represent the name/value pairs for the thing's properties. The name to symbol map is discarded when the setup method returns. You will need to look up and save symbols for later use. Messages identify the object and this requires a map from the id to the memory node. The messages use symbols in place of names.

If we want to enable setting/unsetting observers after the setup, we will need the symbol for  "events", likewise for "properties" and "actions". For now this isn't supported.

How do deal with objects that are passed as values? The message format would use symbols, but the app developer wouldn't know what symbols to use to access or update their properties. This could be avoided when the developer knows about the object in advance, i.e. declares it as a thing. Otherwise, we need some form of reflection.  In other words, the ability to ask the object for its description.

If you declare a thing, you either have its description or you have a link to it that you can dereference. We thus need a means to find the link for objects passed as values. This could be part of a registration process that binds an object id to its description.

Memory Allocation
=================

I am avoiding the use of new and free on the advice that these cause problems for microcontrollers. Instead, I use static allocation with arrays. This implies the need to monitor the usage levels of the various arrays.

NodePool: An array for allocating JSON nodes and AVL balanced binary tree nodes. On the ATmega328P, these both take 6 bytes. WOT_NODE_POOL_SIZE sets the array size and is defined in NodePool.h. The pool is allocated statically in WebThings.cpp. Balanced binary trees are used for associative and numerically indexed arrays.  JSON nodes include a union for their different types.

Names: this defines a hash table that maps string to numeric symbols. The hash table is dynamically assigned as a local variable in WebThings:thing() and WebThings::proxy();  The table holds strings with a pointer to the string plus its length. The ATmega328P uses the Harvard memory architecture which separates data and code into distinct address spaces. I allow for static strings to be held in the code address space to save RAM. The Strings class provides an abstraction layer that hides where strings are stored.

CoreThings: Things and Proxies are derived from the CoreThings class, and both take 10 bytes on the ATmega328P. This allows them to allocated from the things_pool buffer in WebThings.cpp.

Stale: this is the set of references that were lost when updating the value of a property for a thing or proxy. This is used by the garbage collector when sweeping for nodes that aren't reachable from the roots, and is needed because we can't distinguish JSON and AvlNodes except by how they are referenced. That prevents a sweep algorithm from simply iterating through the node pool.

Garbage Collection
==================

When overriding a item in a JSON object or array, the reference to the former value becomes stale and a candidate for garbage collection if there are no other references to it. The garbage collector is invoked when the stale set becomes full, or the node allocator runs out of free nodes. The process involves two stages. The first is to mark all JSON nodes reachable from the roots. The second is to sweep the nodes reachable from the set of stale references to find nodes that aren't marked and hence can be freed. The mark uses a bit in the first byte of the JSON node, and the sense alternates between successive calls to the garbage collector. New nodes are given the mark state that reachable nodes were left in by the last garbage collection.

JSON nodes may reference things and proxies. This results in a mark or sweep of the properties for the referenced thing or proxy. The AVL tree for a JSON object or array is cleaned when that object is freed. The node pool could be improved by maintaining free nodes in a linked list. I also need a way to recover from memory exhaustion, e.g. note problem in EEPROM then restart the server.  Likewise for when the stale set is exhausted.

Thing Properties
================

These are part of the thing data model and implemented with a JSON associative array to map the property name to its JSON value. Values can be null, true, false, floats, signed and unsigned integers, strings, associative or indexed arrays, and in future, streams, things and enumerations. Future work is anticipated on integrity constraints acting on individual properties, across properties belonging to the same thing, and across things. This is important for robust operation in the presence of faults.

Properties are identified with numeric symbols assigned per thing based upon the thing's data model. This allows for compact message encodings that avoid the need to send string identifiers.  Things are likewise identified by numeric ids which are scoped to the server originating the message. Servers are therefore required to maintain a mapping to the thing's model and the memory object for the thing.

Thing Events
============

These are part of the thing data model and not to be confused with platform events for responding to hardware interrupts in a safe way.

I am using a JSON associative array to map the thing event name to its handler. This assumes that there is only one handler per event, which should be sufficient. In principle, I could generalise to JSON array of functions if really needed. The event is delivered to the handler which is defined as:

    void (*EventFunc)(Symbol event, Thing *thing, ...)
    
Events are raised by calling:

    void raise_event(Symbol event, ...)
    
If varargs is too painful to describe in the data model and for the messages, I could allow an optional single argument.

Thing Actions
=============

JSON associative array are used to to map the action name to its handler. There may be zero, one or more responses for action invokation, and in principle a response may itself be a Thing. The method for invoking an action on a proxy is defined as

    int invoke(Symbol action, ResponseFunc response, ...)

Pass null should for response in no response is needed. The following args are optional and used to pass data along with the action. The invoke method returns a positive id that is later delivered to the response handler whose function is defined as:

    void (*ResponseFunc)(unsigned int id, Thing *thing, ...)

If varargs is too painful to describe in the data model and for the messages, I could allow an optional single argument.

For Things, I need a way to bind the action name to the application supplied handler. The method signature could be something like:

    void (*ActionFunc)(ProxyId proxy_id, ActionId action_id, ...)
    
Where the proxy_id identifies the proxy that initiated the action, and the action_id is used by that proxy to associate responses with invocations.

Discovery
=========

Battery or ambient powered devices will need to sleep a lot to conserve power. It seems reasonable for these devices to discover a powered gateway that is always listening. My first idea was to create a very simple discovery protocol using multicast packets. However, it seems that the MacBook Pro blocks all multicast sockets except for mDNS, so I am therefore looking to exploit mDNS for gateway discovery. This will also allow me to use tools like the Bonjour Browser to check that the gateway is discoverable. 

    dns-sd -R "Web of Things Gateway" _wot._tcp . 12345
    
This times out and needs to be Ctrl-C and redone. I don't know how to increase the lease time. This could be a problem for demos! The work around is to reregister the service at the same time as starting the Arduino - not so easy in practice. Perhaps there is a way to force the MacBook to respond to a request?

Another idea would be to use socket 1 for a background mDNS service and to track messages announcing a new gateway and when a gateway has been unregistered.

see http://grouper.ieee.org/groups/1722/contributions/2009/Bonjour%20Device%20Discovery.pdf

Wireshark shows my mDNS queries, but OS X only sends the responses within a short interval after registering the service. Unregistering the service causes OS X to send a resource record with a TTL of zero. This compares with TTL of 120 for the SRV record and TTL of 4500 for the TXT record. I need to learn more about how this is supposed to work! See RFC6762.

RFC1035 states that Resource Records have the following format:

NAME  variable length
TYPE 2 bytes
CLASS 2 bytes
TTL 4 bytes
RDLENGTH 2 bytes
RDATA which is RDLENGTH bytes long

The NAME format is a sequence of labels preceded by a byte with the length in bytes for the label and terminated by a null length. If the length > 0xC0 (192) you need to jump to the address given by msgstart + (p[i] - 0xC0)<<8 + p[i+1]. This requires random access into the read message buffer on the W5100, i.e. to peek at the message data and only update the read pointer when I've finished parsing.

DNS provides a "SRV" record that binds a server to a port and name, but not IP. "A" records bind a name to an IPv4 address, e.g.

"SRV" record for "Web of Things Gateway._wot._tcp.local" 
  port 12345
  name "Daves-MacBook-Pro.local"
  
"A" record for  "Daves-MacBook-Pro.local"
  IP 192.168.1.133
  
mDNS uses PTR (pointer) records to map service types to instances of that service, e.g.

   _printer._tcp._local. 28800 PTR PrintsAlot._printer._tcp._local.
   
I currently ignore these records and instead allow for _wot._tcp.local to match to itself and to foo._wot._tcp._local for any foo.  DNS "TXT" records can be used to convey name value pairs with a length byte followed by a string with a key and value separated by "=", or flags as strings without an "=". In principle, this could be used to convey the message encoding for the service and other properties, but a better idea is to express this in the service protocol. I think that a single text record can contain multiple text strings, but need to check this.
  
I therefore need to note the server name and match that to the "A" records! In principle, a single message could convey info for multiple servers, or multiple addresses for the same server name. For now I will assume a single server. The complication comes in matching the name. This is made harder since you can't get the record type until you've parsed the name. The algorithm for matching the name whilst parsing it requires the target to be provided in advance. 

A possible hack would be to re-parse the record name after determining its type, as we could the provide the desired target for the server name. The challenge is that the server name is held in the W5100 buffer and may itself involve links. It might be easier to use a separate algorithm that takes the offsets for both names. This doesn't have to be fast, so we could retrieve one byte a time.

Another idea reduces the match to comparing two 16 bit numbers corresponding to the offsets within the DNS message. This assumes that the SRV records precede the "A" record, and that the DNS server maximally optimises name storage. We could re-read the link value for the NAME field in the "A" record, or we could complicate parse_name to pass it back. The former seems cleaner.  This is now working!

*** to do next

Allow incremental preparation of send buffer for sending messages. This is needed to avoid having to hold entire message in RAM.

Use Flash for mDNS query and switch to Flash safe method for matching LABELs, i.e use Strings::strcmp(s1, len1, s2, len2)

***

TCP based message exchange
==========================

My NodeJS web of things server supports HTTP for accessing thing data models, and Web Sockets for asynchronous message exchange.  HTTP and Web Sockets are expensive to support on low end devices, so it is interesting to look at alternatives.  The Ethernet driver chips such as Wiznet W5100 and the ENC28J60 provide native support for TCP. This makes it attractive to use TCP as this avoids the need for a reliable messaging layer over UDP. Why re-invent a very good wheel!  This works well for the star topology where battery operated IoT devices connect to a permanently powered gateway. Other approaches are better suited to sensor networks and other forms of peer to peer topologies for thing servers.

I should be able to use a similar set of messages to those I defined for Web Sockets. The main differences are: the use of binary encodings and numeric ids in place of strings; and the need for IoT devices to register themselves with the gateway, passing the thing descriptions explicitly or by reference or some combination of the two.

DNS client for external gateway
===============================

At some point I may want to add a DNS name resolver for external gateways, or as a means to download external thing descriptions. This is likely to require larger Flash storage than is available on the ATmega328P. Here is a link to aprogram that I should be able to adapt for the W5100:

   http://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
   
Sharing memory at start up
==========================

The memory needed for DHCP and for mDNS is allocated on the stack, and competes for limited RAM with the statically allocated buffers for JSON, etc.  In principle, I could borrow the statically allocated buffers and only initialise these buffers once the network has been set up.

Initial demo & testing
======================

How to provide a clean interface between the transport later and WebThings?  The serve method gets requests, parses them and then acts on them. It thus needs access to the WebThings wot object. I may also poll sensors for input, possibly via an interrupt server routine tha queues an event. How does the event handler know about web things and the transport layer?  The simplest idea is recognise wot, and transport as external global objects.

Use a fixed IP address and have the laptop connect to the Arduino to set up a proxy for a thing on the Arduino. This involves a message to register a proxy and get the thing id and its description. I should have another message to unregister a proxy




Plans
=====

1. clean up discovery code and interface

2. setup OS X clone of project as a gateway server

3. setup Arduino project as a gateway client that discovers the gateway and registers its things with the gateway

4. implement get_property, set_property, observe and invoke along with a means to handle responses.

5. support raising and handling of events

6. implement the messaging and lifecycle support

7. allow for downloading thing descriptions from another server

8. allow for late bound values

9. support streams as first class types

10. support EEPROM for saving config

11. watch dog timer and restart on errors

12. port to Arduino Mega with 8KB RAM and larger EEPROM and Flash





