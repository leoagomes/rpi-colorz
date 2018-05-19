require 'sinatra/base'

module Rainbow
  class Base < Sinatra::Base
    configure :development do
      register Sinatra::Reloader
    end

    not_found do
      json :error => 'Not found', :status => response.status
    end

    error Sinatra::BadRequest do
      json :error => 'Bad Request', :status => response.status
    end
  end
end
