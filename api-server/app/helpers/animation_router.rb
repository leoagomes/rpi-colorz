require 'ffi-rzmq'

module Colors
  module Helpers
    def error_check(result)
      if ZMQ::Util.resultcode_ok? result
        false
      else
        STDERR.puts "ZMQ operation failed. Error #{ZMQ::Util.errno}, \"#{ZMQ::Util.error_string}\""
        caller(1).each { |callstack| STDERR.puts callstack }
        true
      end
    end

    class AnimationRouter
      def initialize
        @context = ZMQ::Context.new
        @socket = @context.socket ZMQ::PUB
        if error_check @socket.setsockopt ZMQ::LINGER, 1
          raise Exception.new
        end
      end

      def start(host)
        error_check @socket.bind host
      end

      def send_animation(animation, params)
        message = {
            :action => 'animation',
            :data => {
                :animation => animation,
                :params => params
            }
        }.to_msgpack

        error_check @socket.sendmsg ZMQ::Message.new message.bytes
      end

      def send_params(params)
        message = {
            :action => 'params',
            :data => {
                :params => params
            }
        }.to_msgpack

        error_check @socket.sendmsg ZMQ::Message.new message.bytes
      end
    end
  end
end
