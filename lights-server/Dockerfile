FROM alpine

# default directory values
ARG WS281X_DIR=/root/rpi_ws281x
ARG SERVER_DIR=/root/lights-server

# copy the server files to the image
COPY src/* $SERVER_DIR/

# the default extended pixel control port
EXPOSE 7890

# update and upgrade
RUN apk update && apk upgrade

# install packages
RUN apk add bash git build-base cmake linux-headers curl luajit python scons \
        libuv libuv-dev make gcc

# clone ws2811 driver, compile it and copy the shared library to the controller
# directory
RUN git clone https://github.com/leoagomes/rpi_ws281x.git $WS281X_DIR && \
    cd $WS281X_DIR && \
    scons

# build the server
RUN cd $SERVER_DIR && \
    export WS281X_DIR=$WS281X_DIR && \
    make

# Set command to server execution
CMD cd $SERVER_DIR && \
    make run