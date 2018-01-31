signal = require 'posix.signal'
ws = require 'libs/ws281x'
server = require 'server'

local DEFAULTS = {
	XPC_PORT = 7890,
	DEF_PORT = 1123,
}

function main()
	-- initialize ws281x library
	ws:init()
	-- trap sigterm to finalize the ws library
	signal.signal(signal.SIGTERM, sighandler);

	-- start the server
	server:start(DEFAULTS.XPC_PORT, DEFAULTS.DEF_PORT)
end

function sighandler(signum)
	if signum == signal.SIGTERM then
		ws:fini()
	end
end

main()