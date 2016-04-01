# Web of Things Framework for Arduino

This will be an experimental implementation of the Web of Things Framework written in the C++ programming language for the Arduino. This project is a sister of the [NodeJS server for the Web of Things](https://github.com/w3c/web-of-things-framework).

A hundred billion IoT devices are expected to be deployed over the next ten years. There are many IoT platforms, and an increasing number of IoT technologies. However, the IoT products are currently beset by silos and a lack of interoperability, which blocks the benefits of the network effect from taking hold.  At W3C, we are exploring the potential for bridging IoT platforms and devices through the World Wide Web via a new class of Web servers that are connected through a common framework, and available in a variety of scales from microcontrollers, to smart phones and home/office hubs, and cloud-based server farms.

This framework involves software objects ("things") on behalf of physical and abstract entities. These things are modelled in terms of metadata, properties, actions and events, with bindings to scripting APIs and a variety of protocols, given that no one protocol will fulfil all needs. This server will start with bindings to TCP, based upon the native TCP support in the Arduino Ethernet Shield, thanks to the Wiznet W5100 network controller.

## Technical Details

The Arduino boards like the Uno are extremely limited when it comes to RAM and Flash memory. There are a range of possibilities for connectivity. The initial focus is on IP connectivity via the Arduino Ethernet Shield. Future work is expected on other technologies e.g. sensor networks using the Nordic nRF24L01+, and Bluetooth using the Bluetooth Bee V2.0 module.

The server implements memory management for JSON nodes along with average length (AVL) binary trees for arrays and name/value sets. These are allocated from statically declared buffers to avoid issues that can arise with malloc and free.  A mark/sweep garbage collector is used to reclaim unused nodes when memory is running low. There is an encoder/decoder for binary encoding of JSON messages. This uses numeric symbols in place of property names where the symbols are deterministically generated from the thing data models.

A constrained web of things server can be used with one or more sensors or actuators. This project will allow you to write drivers for simple sensors and actuators, without needing to deal with the details of the protocols. Developers will be able to script the sensors and actuators on a more powerful server where the Web of Things Framework provide the glue between the proxy object and the object it acts on behalf of.

When starting up the server can use a fixed IP address or it can use DHCP for a dynamically assigned address. You can likewise use a fixed IP address and port for the gateway, or make use of multicast DNS for its discovery. The server will then register the things defined by the sketch with the gateway.

Every attempt is being made to reduce the memory footprint. If you have suggestions for further improvments, please let us know!

n.b. The readme.txt is generally more upto date when it comes to recent work on the server design. This is a work in progress and not yet complete!

## Prerequisites

* An Arduino Uno and Wiznet W5100 based Ethernet Shield. These can be purchased very cheaply on ebay.
* A wired Ethernet connection to your router, e.g. a broadband device like BT's Homehub.
* The Arduino IDE - note that many Arduino clones use the CH340G chip that isn't supported directly by the Arduino IDE. There are free drivers and installation instructions for different host platforms.
* A knowledge of C++ and an appreciation of how to design for microcontrollers.
  
## Installation

The starting point is to install Git, see:

  http://git-scm.com/book/en/v2/Getting-Started-Installing-Git

Next create a copy of this directory and change to it as follows:

```
  git clone https://github.com/w3c/wot-arduino
  cd wot-arduino
```

## Contributing

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/w3c/web-of-things-framework?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

We welcome contributions. If you find a bug in the source code or a mistake in the documentation, you can help us by submitting an issue to our [GitHub repository](https://github.com/w3c/arduino-wot), and likewise if you have suggestions for new features. Even better you can submit a Pull Request with a fix. We also have a Gitter chat channel, see above link.

We encourage you to join the W3C [Web of Things Community Group](https://www.w3.org/community/wot/) where contribution and discussions happen. Anyone can join and there are no fees.

The amount of time you contribute and the areas in which you contribute is up to you. 

### Acknowledgements

This work has been funded in part (through October 2015) by the European Union's 7th Research Framework Programme (FP7/ 2013-2015) under grant agreement nº317862 - Compose.

## License

(The MIT License)

Copyright (c) 2015-2016 Dave Raggett &lt;dsr@w3.org&gt;

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
