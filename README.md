# rpi-colorz

**NOTE: this repository is a work in progress.** Even more importantly it is
undergoing a refactoring since the code isn't exactly as I want it to be. My
aims are much higher than what is currently available in this repository.
Any help is appreciated, though.

This project aims to provide a set of tools that one can host on a raspberry pi
(originally intended for the Zero W) and use to control a set of WS281X (tested
with the WS2812) compatible LED strips.

This repository aims to provide 3 major tools: an XPC (eXtended Pixel Control)
protocol server, a animation/mode runner and a REST server that glues them
together in a single API.

## XPC Server

### XPC (eXtended Pixel Control) Protocol

XPC is a protocol (somewhat loosely) based on [OPC](openpixelcontrol.org) that
provides a way to precisely control via the network an addressable LED strip
(such as the WS2811). It should be defined better in
[its own documentation file](./lights-server/XPC.md).

### Server

The server is currently developed in C, making extensive use of libuv for the
asynchronous network IO. There currently is a bug where one of libuv's
assertions fail out of (kind of) nowhere. This should be fixed in a later
version. I also am studying the possibility of rewritting the server using Rust
or C++ (and asio).

The reasoning behind writing it in C and not in something like JavaScript (since
I would be using libuv anyway) is because the server is the only program that
needs to run as root if outside a docker container -- and when in a container,
needs to run in privileged mode, since it uses a mix of the DMA and some other
features which I couldn't get to work properly when not running privileged --
and I trust myself better to write reasonably safe C code than to write sane JS.
I also don't like the idea of running JS as root. If anyone wants to provide me
reasons why I'm wrong, they're more than welcome.

This server speaks solely XPC. It starts two listeners, one over TCP and another
over UDP.

The server is located at `lights-server` and peeking at the code should be
interesting (also because I'm not entirely sure on what's happening with libuv
and I wasn't fortunate enough to find much documentation on uv error handling).

## API Server

The API server exposes a REST api over a subset of the operations available with
XPC, like setting and getting the current state ('on'/'off') and number of strips
available. It also saves other data like the device's discovery name and
description. Finally, the API server also serves as an interface for running
animations on the strip, by publishing animation objects on a zmq network.

It is currently implemented in ruby mainly because I wanted this to be able to
answer COAP requests and I've seen there is a layer for Rack that can do that.
Also, learning ruby sounded fun.

Checking out the code at `api-server`, it is in the middle of refactoring, but
should be interesting as well.

## Animation runner

The animation runner exposes an XPC connection to a scripting language (currently
the only one with acceptable code is under development and is Lua). It subscribes
to the API server publisher and handles published 'data' and 'parameter' events.

As usual, check out `animation-runners` the code for more.

## Why?

I originally tried to develop something like what I'm doing here on an ESP8266,
but prototyping was getting difficult either being limited to lua or having to
reimplement everything that was already done with nodemcu in C/C++. I then
bought a Raspberry Pi Zero W and decided to do everything modular enough for me
to be able to quickly swap scripting languages and rest interfaces or coap
support being able to still use all the other code I had written.


## License

Everything under the directories `lights-server` and `api-server` is released
under the MIT based license at `LICENSE`. The code at `animation-runner` is
explicitely not licensed aside from the libraries used as dependencies which
each have their own licenses.