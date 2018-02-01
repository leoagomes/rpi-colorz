FROM alpine

# copy the server files to the image
COPY src/* /root/light-runner/

# the default extended pixel control port
EXPOSE 2345

# update and upgrade
RUN apk update && apk upgrade

# install packages
RUN apk add bash git build-base cmake linux-headers curl luajit python scons \
        lua lua-dev luarocks5.1 && \
    luarocks-5.1 install luv luasocket

# Set command to server execution
CMD cd /root/light-runner && \
    luajit main.lua