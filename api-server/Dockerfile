FROM ruby:2.5-alpine

ARG API_SERVER_DIR=/root/api-server

ENV XPC_DEVICE_ADDRESS=lights-server
ENV XPC_DEVICE_PORT=7890

# Web Sever Port
EXPOSE 80

# ZMQ Queues
EXPOSE 5555

# copy files to the image
COPY src/* $API_SERVER_DIR

# install needed gems
RUN gem install bundler rack thin && \
    cd $API_SERVER_DIR && \
    bundle install

CMD cd $API_SERVER_DIR && rackup