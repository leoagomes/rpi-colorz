luv = require 'luv' if not luv

class Timer
	new: (interval, action) =>
		@timer = luv.new_timer()
		@timer\start 0, interval, action
	
	stop: =>
		@timer\stop!

	set_interval: (interval) =>
		@timer\set_repeat interval

	get_repeat: =>
		@timer\get_interval!
