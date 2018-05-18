default_environment =
	:assert
	:collectgarbage
	:error
	:setmetatable
	:ipairs
	:next
	:pcall
	:print
	:select
	:tonumber
	:tostring
	:type
	:_VERSION
	:xpcall
	:coroutine
	:string
	:unpack

class Scripter
	new: (environment=default_environment)=>
		@environment = environment
		@script = nil

	run_script: (script) =>
		@script = script
		@chunk, err = loadstring @script

		if @chunk == nil
			return false, err

		setfenv @chunk, @environment

		rt, err = pcall @chunk

		if rt == nil or rt == false
			return false, error

		switch type err
			when 'table'
				@update = err.update
				@update_params = err.update_params
			when 'function'
				@update = err
				@update_params = nil
			else
				return false, "wrong type for data execution return #{err}"

		return true

	has_update: =>
		return @update != nil

	set_env: (key, val) =>
		@environment[key] = val

	update: (...) =>
		if @update != nil
			return pcall @update, ...

		return false

	update_params: (...) =>
