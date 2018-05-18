require 'msgpack'
require 'ffi-rzmq'

module Colors
  class ModeRouter
    # TODO: better handle disconnections

    def initialize(address)
      # initialize zeromq
      @context = ZMQ::Context.new
      @publisher = @context.socket(ZMQ::PUB)
      @publisher.bind address
    end

    def send_mode(mode)
      begin
        @publisher.send_string({:event => 'mode', :data => mode}.to_msgpack)
        true
      rescue
        false
      end
    end

    def send_params(params)
      begin
        @publisher.send_string({:event => 'params', :data => params}.to_msgpack)
        true
      rescue
        false
      end
    end
  end
end