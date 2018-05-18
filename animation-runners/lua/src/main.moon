Timer = require 'Timer'
Scripter = require 'Scripter'
Device = require 'Device'
Subscriber = require 'Subscriber'

-- change these settings
pub_connection_attempts = 10
pub_connection_sleep = 2

-- do not change these settings unless you know
-- what you're doing
engine_name = "Lua"
engine_till_update = 25

-- got connection, create a scripter
scripter = Scripter!

-- subscriber connection
pub_address = os.getenv 'PUB_ADDRESS'
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

-- zmq subscriber receive loop
while true
	local poll_time
	if scripter\has_update!
		poll_time = engine_till_update
	else
		poll_time = -1

	if sub\poll poll_time
