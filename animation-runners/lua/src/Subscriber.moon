zmq = require 'lzmq' if not zmq

class Subscriber
	new: (pub_address) =>
		@context = zmq.context!
		@socket = @context\socket zmq.SUB
		@address = pub_address
	
	connect: (address=@address) =>
		@socket\connect address

	poll: (timeout=0) =>
		@socket\poll timeout

	receive: =>
		@socket\recv!

