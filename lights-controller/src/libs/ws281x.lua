ffi = require 'ffi'
bit = require 'bit'

-- ffi function and type definitions
ffi.cdef[[
typedef struct {
	uint32_t type;
	uint32_t hwver;
	uint32_t periph_base;
	uint32_t videocore_base;
	char *desc;
} rpi_hw_t;
typedef uint32_t ws2811_led_t;
typedef struct
{
	int gpionum;
	int invert;
	int count;
	int strip_type;
	ws2811_led_t *leds;
	uint8_t brightness;
	uint8_t wshift;
	uint8_t rshift;
	uint8_t gshift;
	uint8_t bshift;
	uint8_t *gamma;
} ws2811_channel_t;

typedef struct
{
	uint64_t render_wait_time;
	struct ws2811_device *device;
	const rpi_hw_t *rpi_hw;
	uint32_t freq;
	int dmanum;
	ws2811_channel_t channel[2];
} ws2811_t;

typedef enum {
	WS2811_SUCCESS = 0,
	WS2811_ERROR_GENERIC = -1,
	WS2811_ERROR_OUT_OF_MEMORY = -2,
	WS2811_ERROR_HW_NOT_SUPPORTED = -3,
	WS2811_ERROR_MEM_LOCK = -4,
	WS2811_ERROR_MMAPX = -5,
	WS2811_ERROR_MAP_REGISTERS = -6,
	WS2811_ERROR_GPIO_INIT = -7,
	WS2811_ERROR_PWM_SETUP = -8,
	WS2811_ERROR_MAILBOX_DEVICE = -9,
	WS2811_ERROR_DMA = -10,
	WS2811_ERROR_ILLEGAL_GPIO = -11,
	WS2811_ERROR_PCM_SETUP = -12,
	WS2811_ERROR_SPI_SETUP = -13,
	WS2811_ERROR_SPI_TRANSFER = -14,
	WS2811_RETURN_STATE_COUNT
} ws2811_return_t;

ws2811_return_t ws2811_init(ws2811_t *ws2811);
void ws2811_fini(ws2811_t *ws2811);
ws2811_return_t ws2811_render(ws2811_t *ws2811);
ws2811_return_t ws2811_wait(ws2811_t *ws2811);
const char * ws2811_get_return_t_str(const ws2811_return_t state);

void* memcpy (void* destination, const void* source, size_t num);
]]

-- load the native ws library
nws = ffi.load('./libws2811.so')

local ws = {}

function ws:init(freq, dmanum, gpionum, count, strip_type)
	-- initialize the main structure
	self._t = ffi.new("ws2811_t", {
		freq = freq,
		dmanum = dma,
		channel = {
			{
				gpionum = gpionum,
				count = count,
				invert = 0,
				brightness = 255,
				strip_type = strip_type
			}
		}
	})
	-- call init function
	return nws.ws2811_init(self._t)
end

function ws:render()
	-- call render function
	return nws.ws2811_render(self._t)
end

function ws:fini()
	-- call finalizing function
	nws.ws2811_fini(self._t)
end

function ws:wait()
	-- call native function
	nws.ws2811_wait(self._t)
end

function ws:ledset(strip, index, r, g, b, w)
	local value

	-- if there is only 'r', then r is already in the right format
	if r ~= nil and g == nil and b == nil and w == nil then
		value = r
	else
		-- if no white was provided, set to 0x00
		if w == nil then
			w = 0
		end

		value = bit.bor(
			bit.lshift(w, 8 * 3),
			bit.lshift(r, 8 * 2),
			bit.lshift(g, 8 * 1),
			b)
	end

	self._t.channel[strip].leds[index] = value
end

function ws:ledget(strip, index)
	local r, g, b, w, value

	-- get the raw pixel value
	value = self._t.channel[strip].leds[index]

	-- convert the value to wrgb
	w = bit.rshift(bit.band(value, 0xFF000000), 8 * 3)
	r = bit.rshift(bit.band(value, 0x00FF0000), 8 * 2)
	g = bit.rshift(bit.band(value, 0x0000FF00), 8 * 1)
	b = bit.band(value, 0x000000FF)

	return r, g, b, w
end

function ws:ledbset(strip, amount, values)
	if type(values) == 'table' then
		-- if an array of values was given set the first <amount> leds to values
		if type(values[1]) == 'number' then
			for i=1,amount do
				self:ledset(strip, i, values[i])
			end
		elseif type(values[1]) == 'table' then
			for i=1,amount do
				local v = values[i]
				self:ledset(strip, i, v.r, v.g, v.b, v.w)
			end
		end
	elseif type(values) == 'string' then
		-- if a buffer with values was given just memcpy the buffer into
		-- the led buffer
		local cpyamount = math.min(amount,#values)
		ffi.C.memcpy(self._t.channel[strip].leds, values, cpyamount)
	elseif type(values) == 'number' then
		-- if a single value was given, fill the buffer with this value
		for i=1,amount do
			self:ledset(strip, i, values)
		end
	end
end

function ws:ledsset(strip, start, finish, values)
	-- TODO: implement led value splicing
end

return ws