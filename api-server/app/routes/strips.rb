require 'sinatra'
require 'sinatra/json'
require 'json'

require_relative '../helpers/device_client'

module Colors
  module Routes

    class Strips < Sinatra::Base
      attr_accessor :device

      def initialize(app)
        super
        @device = DeviceClient.new ENV['XPC_DEVICE_ADDRESS'], ENV['XPC_DEVICE_PORT']
      end

      def strip_obj(strip)
        { :state => @device.get_state(strip), :length => @device.get_length(strip) }
      end

      get '/strips' do
        strip_count = @device.get_strip_count

        strips = (0...strip_count).map do | strip |
          strip_obj strip
        end

        json :strips => strips
      end

      get '/strips/:id' do
        id = params['id'].to_i
        json strip_obj id
      end

      put '/strips/:id' do
        strip = params['id'].to_i
        request.body.rewind
        data = JSON.parse request.body.read

        @device.set_state(strip, data['state']) unless data['state'].nil?
        @device.set_length(strip, data['length']) unless data['length'].nil?
      end

      get '/strips/:id/contents' do
        json :contents => @device.read(params['id'].to_i)
      end

      put '/strips/:id/contents' do
        strip = params['id'].to_i
        request.body.rewind
        data = JSON.parse request.body.read
        contents = data['contents']

        if contents.is_a? Array
          @device.start_set(strip, contents)
        elsif contents.is_a? Integer
          @device.fill(strip, contents)
        else
          raise Sinatra::BadRequest
        end
        response.status = 200
      end
    end

  end
end
