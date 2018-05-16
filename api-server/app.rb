require 'json'
require 'sinatra'
require 'sinatra/reloader'

require './app/base'

require './app/routes/modes'
require './app/routes/strips'

require './app/helpers/env_loader'

env_defaults 'XPC_DEVICE_ADDRESS' => 'raspberrypi.local',
             'XPC_DEVICE_PORT' => '7890',
             'MODE_PUB_HOST' => 'tcp://*:5555'

module Colors
  class App < Base
    use Routes::Modes
    use Routes::Strips
  end
end
