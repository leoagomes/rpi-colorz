FROM ruby:2.5-alpine

# copy files to the image
COPY src/* /root/api-server

# install needed gems
RUN gem install bundler && \
    cd /root/api-server && \
    bundle install

CMD cd /root/api-server && ruby app.rb