# Web of Things Framework for Arduino

This will be an experimental implementation of the Web of Things Framework written in the C programming language for the Arduino. This project is a sister of the [NodeJS server for the Web of Things](https://github.com/w3c/web-of-things-framework).

A hundred billion IoT devices are expected to be deployed over the next ten years. There are many IoT platforms, and an increasing number of IoT technologies. However, the IoT products are currently beset by silos and a lack of interoperability, which blocks the benefits of the network effect from taking hold.  At W3C, we are exploring the potential for bridging IoT platforms and devices through the World Wide Web via a new class of Web servers that are connected through a common framework, and available in a variety of scales from microcontrollers, to smart phones and home/office hubs, and cloud-based server farms.

This framework involves virtual objects ("things") as proxies for physical and abstract entities. These things are modelled in terms of metadata, events, properties and actions, with bindings to scripting APIs and a variety of protocols, since no one protocol will fulfil all needs. This server will start with bindings to MQTT and CoAP.

## Technical Details

The Arduino boards like the Uno are very limited when it comes to RAM and Flash memory. There are a range of possibilities for IP connectivity. The Arduino Ethernet Shield is quite resource intensive. The ENC28J60 board provides an alternative for Wired Ethernet. For wireless, the ESP8266 can be used for a low cost WiFi connection. If you don't need IP connectivity, the NRF24L01+ can be used for point to point or mesh networks. Other possibilities include Bluetooth and ZigBee.

A very constrained web of things server can be used with a single sensor or actuator. Assuming IP connectivity, CoAP and MQTT are good choices for protocols.  This project will allow you to write drivers for simple sensors and actuators, without needing to deal with the details of the protocols. Developers will be able to script the sensors and actuators on a more powerful server where the Web of Things Framework provide the glue between the proxy object and the object it acts on behalf of.


## Prerequisites

 *to be deterimined when we have some code that can be built and run*
  
## Installation

The starting point is to install Git, see:

  http://git-scm.com/book/en/v2/Getting-Started-Installing-Git

Next create a copy of this directory and change to it as follows:

```
  git clone https://github.com/w3c/wot-arduino
  cd wot-arduino
```

## Contributing

We welcome contributions. If you find a bug in the source code or a mistake in the documentation, you can help us by submitting an issue to our [GitHub repository](https://github.com/w3c/arduino-wot), and likewise if you have suggestions for new features. Even better you can submit a Pull Request with a fix. We also have a Gitter chat channel, see above link.

We encourage you to join the W3C [Web of Things Community Group](https://www.w3.org/community/wot/) where contribution and discussions happen. Anyone can join and there are no fees.

The amount of time you contribute and the areas in which you contribute is up to you. 

## License

(The MIT License)

Copyright (c) 2015 Angelos Kapsimanis &lt;angelos.kapsimanis@gmail.com&gt;

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
