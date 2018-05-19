require 'sinatra'
require 'sinatra/json'
require 'json'

require_relative '../services/mode'
require_relative '../helpers/mode_helpers'

module Rainbow
  module Routes

    class Modes < Rainbow::Base
      helpers Rainbow::Helpers::Mode

      def initialize(app)
        super

        @store = PStore.new 'modes.pstore'
        @service = Services::ModeService.new @store
      end

      configure do
        set :store, @store
      end

      get '/modes/?' do
        json @service.all
      end

      post '/modes/?' do
        data = JSON.parse request.body.read

        if valid_mode? data
          @service.save data
          json :status => 'ok'
        else
          status 400
          json :error => "bad mode"
        end
      end

      get '/modes/current/?' do
        json @service.current
      end

      post '/modes/current/?' do
        data = JSON.parse request.body.read

        begin
          id = data["id"]

          unless data["id"].is_a? String and not data["id"].empty?
            raise 'Invalid id'
          end

          if (current = @service.find(id)) or (valid_mode? data and (current = data))
            @service.current = current
            json current
          else
            raise 'Id not found and invalid body.'
          end
        rescue => error
          status 400
          json :error => error.message
        end
      end

      put '/modes/current/params' do
        data = JSON.parse request.body.read

        if @service.current.nil?
          status 404
        else
          begin
            @service.update_current_parameters data
            status 200
            json :status => 'ok'
          rescue => error
            status 500
            json :error => error
          end
        end
      end

      get '/modes/:id' do
        id = params['id']
        mode = @service.find id

        if mode.nil?
          status 404
          json :error => 'Id not found.'
        else
          status 200
          json mode
        end
      end

      put '/modes/:id' do
        id = params['id']
        data = JSON.parse request.body.read

        begin
          if data.key? 'id'
            data.delete 'id'
          end

          if (updated = @service.update id, data)
            json updated
          else
            status 404
            json :error => 'Id not found.'
          end
        rescue
          status 500
          json :error => 'Internal error.'
        end
      end

      delete '/modes/:id' do
        id = params['id']

        if @service.delete id
          status 200
        else
          status 404
        end
      end
    end

  end
end