--[[
By default, animation scripts run in a 'sandboxed' environment, which has:
 * coroutines (`coroutine`)
 * string
 * strip
 * table
 * 
]]
local static = {
	name = "Static",
	description = "Static color.",
	author = "Leonardo Gomes",

	exports = {
		color = {
			name = 'Color',
			description = 'Strip static color.',
			type = 'color',
			default = {
				r = 255,
				g = 255,
				b = 255,
			},
		},
	},

	params = {
		color = nil,
	},
}

function static:load(params)
	strip:fill(params.color)
end

function static:paramupdate(params)
	strip:fill(params.color)
end

return static