Scripter = require 'Scripter'
Device = require 'Device'
Subscriber = require 'Subscriber'

mp = require 'MessagePack'
b64 = require 'base64'

-- change these settings
pub_connection_attempts = 10
pub_connection_sleep = 2

-- do not change these settings unless you know
-- what you're doing
engine_name = "Lua"
engine_till_update = 25

pub_address = os.getenv 'PUB_ADDRESS'
xpc_address = os.getenv 'XPC_DEVICE_ADDRESS'
xpc_port = os.getenv 'XPC_DEVICE_PORT'

local handle_event

main = ->
	-- got connection, create a scripter
	scripter = Scripter!

	-- subscriber connection
	sub = Subscriber pub_address

	if pub_address\len! <= 0
		print "Invalid PUB_ADDRESS: #{pub_address}"
		os.exit 1

	print "Trying to connect to #{pub_address}"

	local ret, err
	for i = 1, pub_connection_attempts
		ret, err = sub\connect!
		if ret != nil
			ok = true
			break
		os.execute "sleep #{pub_connection_sleep}"

	if ret == nil
		print "Failed connecting to #{pub_address}... #{err}."
		os.exit 1

	print "Connected!"

	-- zmq subscriber receive loop
	while true
		local poll_time
		if scripter\has_update!
			poll_time = engine_till_update
		else
			poll_time = -1

		res, ev = sub\poll poll_time

		if res == nil -- error
			print "Error polling subscriber socket #{ev}."
			os.exit 1

		if res == true -- received event
			handle_event sub, scripter
		else -- timeout
			scripter\update!

-- TODO: put this somewhere else
handle_event = (sub, scripter) ->
	message_string = sub\recv!
	message = mp.unpack message_string

	print "Received event! #{message.event}"

	switch message.event
		when 'mode'
			mode = message.data
			data = mode.data

			if mode.engine != engine_name
				return

			local realtime, script
			if 'table' == type data
				script = data.data
				realtime = data.realtime

				if realtime != true
					realtime = false
			elseif 'string' == type data
				script = data
				realtime = false
			else
				print "Invalid data #{data}"
				return false

			device = Device xpc_address, xpc_port, realtime
			ok, err = device\connect!

			if ok == true
				scripter\set_env 'device', device
			else
				print "Error connecting device... #{err}"
				return

			script = b64.decode script

			res, err = scripter\run_script script
			if res == false
				print "Error running script! #{err}"
				return

			scripter\update!

		when 'params'
			data = message.data

			if 'table' != type data
				print "Params is not a table. #{data}"
				return

			scripter\update_params data

		else
			print "invalid ZMQ message (event: #{message.event})"
			print "ZMQ message: #{message}"
			return

-- invoke main
main!
