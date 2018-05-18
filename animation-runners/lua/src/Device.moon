socket = require 'socket'

struct = require 'struct'
spack, sunpack = struct.pack, struct.unpack

ops =
	BUFFER_START_SET: 0x0,
	BUFFER_REPLACE: 0x1,
	BUFFER_INSERT: 0x2,
	BUFFER_ROTATE: 0x3,
	BUFFER_SHIFT: 0x4,
	BUFFER_FILL: 0x5,
	BUFFER_SET_INDEX: 0x6,
	BUFFER_READ: 0x10,

	STRIP_GET_LENGTH: 0x20,
	STRIP_SET_LENGTH: 0x21,
	STRIP_GET_STATE: 0x22,
	STRIP_SET_STATE: 0x23,

	DEVICE_GET_STRIP_COUNT: 0xFE,
	VENDOR: 0xFF
	

class Device
	new: (address, port, udp=false) =>
		@address = address
		@port = port
		@udp = udp

	connect: =>
		if @udp
			@sock, other = socket.udp!
			if @sock == nil
				return false, other

			@sock\setpeername @address, @port
		else
			@sock, other = socket.tcp!
			if @sock == nil
				return false, other

			@sock\connect @address, @port

		return true

	disconnect: =>
		if not @udp
			@sock\shutdown 'both'

	send: (data) =>
		if @sock
			@sock\send data

	receive: (amount) =>
		if @sock and not @udp
			return @sock\receive amount
		return false

	lock: =>
		locked = {}
		hidden_self = @

		locked.start_set = (zelf, ...) ->
			hidden_self\start_set ...
		locked.replace = (zelf, ...) ->
			hidden_self\replace ...
		locked.insert = (zelf, ...) ->
			hidden_self\insert ...
		locked.rotate = (zelf, ...) ->
			hidden_self\rotate ...
		locked.shift = (zelf, ...) ->
			hidden_self\shift ...
		locked.fill = (zelf, ...) ->
			hidden_self\fill ...
		locked.set_index = (zelf, ...) ->
			hidden_self\set_index ...
		locked.read = (zelf, ...) ->
			hidden_self\read ...
		locked.set_length = (zelf, ...) ->
			hidden_self\set_length ...
		locked.get_length = (zelf, ...) ->
			hidden_self\get_length ...
		locked.set_state = (zelf, ...) ->
			hidden_self\set_state ...
		locked.get_state = (zelf, ...) ->
			hidden_self\get_state ...

		return locked

	start_set: (strip, buffer) =>
		fmt = '>BBI2' .. string.rep 'I4', #buffer
		data = spack fmt, ops.BUFFER_START_SET, strip, #buffer,
			unpack buffer
		@\send data

	replace: (strip, start, buffer) =>
		fmt = '>BBI2I2' .. string.rep 'I4', #buffer
		data = spack fmt, ops.BUFFER_REPLACE, strip, start,
			#buffer, unpack buffer
		@\send data

	insert: (strip, start, buffer) =>
		fmt = '>BBI2I2' .. string.rep 'I4', #buffer
		data = spack fmt, ops.BUFFER_INSERT, strip, start,
			#buffer, unpack buffer
		@\send data

	rotate: (strip, amount) =>
		data = spack '>BBi2', ops.BUFFER_ROTATE, strip, amount
		@\send data

	shift: (strip, amount, in_color) =>
		data = spack '>BBi2I4', ops.BUFFER_SHIFT, strip, amount,
			in_color
		@\send data

	fill: (strip, color) =>
		data = spack '>BBI4', ops.BUFFER_FILL, strip, color
		@\send data

	set_index: (strip, index, color) =>
		data = spack '>BBI2I4', ops.BUFFER_SET_INDEX, strip,
			index, color
		@\send data

	read: (strip) =>
		data = spack '>BB', ops.BUFFER_READ, strip
		@\send data
		length = sunpack '>I2', @\receive 2

		fmt = '>' .. string.rep 'I4', length

		received_data = @\receive length * 4
		if received_data == nil
			return false

		rd = table.pack sunpack fmt, received_data
		return rd

	get_length: (strip) =>
		data = spack '>BB', ops.STRIP_GET_LENGTH, strip
		@\send data

		received_data = @\receive 1
		if received_data == nil
			return false

		rd = sunpack '>B', received_data
		return rd

	set_length: (strip, length) =>
		data = spack '>BBI2', ops.STRIP_SET_LENGTH, strip, length
		@\send data

	get_state: (strip) =>
		data = spack '>BB', ops.STRIP_GET_STATE, strip
		@\send data
		received_data = @\receive 1
		if received_data == nil
			return false
		rd = sunpack '>B', received_data
		return rd

	set_state: (strip, state) =>
		data = spack '>BBB', ops.STRIP_SET_STATE, strip, state == 'on'
		@\send data
