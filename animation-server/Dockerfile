FROM alpine

ARG RUNNER_DIR=/root/light-runner

# copy the server files to the image
COPY src/* $RUNNER_DIR/

# the default extended pixel control port
EXPOSE 2345

# update and upgrade
RUN apk update && apk upgrade

# install packages
RUN apk add bash git build-base cmake linux-headers curl luajit lua lua-dev \
        luarocks5.1 && \
    luarocks-5.1 install luv && \
    luarocks-5.1 install luasocket

# Set command to server execution
CMD cd $RUNNER_DIR && \
    luajit main.lua