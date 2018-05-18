# Lights Server

This hosts the XPC protocol. It is currently written in C using libuv, but maybe
a change to Rust or C++ is coming.

There is a weird bug where libuv just dies and I'm not entirely sure why.

In order to compile this, you'll need the
[rpi_ws281x code](https://github.com/leoagomes/rpi_ws281x), originally written
by [jgarff](https://github.com/jgarff/rpi_ws281x). Clone it somewhere, `export`
"WS281X_DIR" to your environment as where you cloned it, `cd src` and `make` it.

The defaults are explicited when you ask for `--help`.