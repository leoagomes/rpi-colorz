require 'json'
require 'sinatra'

require './modules/device_client'

require './routes/animations_api'
require './routes/strip_api'

require './modules/env_loader'

env_defaults 'XPC_DEVICE_ADDRESS' => 'raspberrypi.local',
             'XPC_DEVICE_PORT' => '7890',
             'ANIMATION_ROUTER_HOST' => 'tcp://localhost:5555'

class ApiServer < Sinatra::Base
  use AnimationsApi
  use StripApi

  not_found do
    json :error => 'Not found', :status => response.status
  end

  error Sinatra::BadRequest do
    json :error => 'Bad Request', :status => response.status
  end
end
