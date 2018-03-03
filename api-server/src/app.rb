require 'json'
require 'sinatra'

require './modules/device_client'

require './routes/animations_api'
require './routes/strip_api'

DEFAULT_STRIP_INDEX = 0

$device = DeviceClient.new ENV['XPC_DEVICE_ADDRESS'], ENV['XPC_DEVICE_PORT']
$device.connect

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
