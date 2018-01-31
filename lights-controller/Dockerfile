FROM alpine

# copy the server files to the image
COPY src/* /root/lights-controller/

# the default extended pixel control port
EXPOSE 7890

# update and upgrade
RUN apk update && apk upgrade

# install packages
RUN apk add bash git build-base cmake linux-headers curl luajit python scons \
        lua lua-dev luarocks5.1 && \
    luarocks-5.1 install luaposix luv 

# clone ws2811 driver, compile it and copy the shared library to the controller
# directory
RUN cd && \
    git clone https://github.com/leoagomes/rpi_ws281x.git && \
    cd rpi_ws281x && \
    scons && \
    cp libws2811.so /root/lights-controller/libs

# Set command to server execution
CMD cd /root/lights-controller && \
    luajit main.lua