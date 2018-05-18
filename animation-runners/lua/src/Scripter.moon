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

		@update, err = pcall @chunk

		if @update == nil
			return false, error

		return true

	has_update: =>
		return @update != nil

	update: (...) =>
		if @update != nil
			return pcall @update, ...

		return false
