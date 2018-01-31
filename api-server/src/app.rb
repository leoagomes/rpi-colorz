require 'json'
require 'sinatra'
require 'sinatra/reloader' if development?
require './device'

device = Device.new

get '/device' do
  device.to_json
end

post '/device/turn' do
  device.state = 'on'
end

get '/' do
  'Hello'
end
