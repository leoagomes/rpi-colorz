/**
 * @brief Strip operation implementations.
 * 
 * It is important to note that the functions in this file may not check the
 * validity of the parameters passed, so be careful using them.
 * 
 * @file strip.c
 * @author Leonardo G.
 */
#include "strip.h"

#include "utils.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

strip_t strip = { //!< The global strip structure
	.strip = {
		.freq = DEFAULT_FREQ,
		.dmanum = DEFAULT_DMA,
		.channel = {
			[0] = {
				.gpionum = DEFAULT_GPIO,
				.count = DEFAULT_LEDCOUNT,
				.invert = DEFAULT_INVERT,
				.brightness = DEFAULT_BRIGHTNESS,
				.strip_type = DEFAULT_STRIPTYPE
			},
			[1] = {
				.gpionum = 0,
				.count = 0,
				.invert = 0,
				.brightness = 0,
				.strip_type = 0
			}
		}
	},
	.state = {
		[0] = STRIP_OFF,
		[1] = STRIP_OFF
	}
};

/**
 * @brief Sets a strip's state (on/off).
 * 
 * @param strip The strip operand.
 * @param channel The effective channel index.
 * @param state The state to set. OFF = 0, ON = !0
 */
void strip_state_set(strip_t* strip, int channel, int state) {
	ws2811_led_t* aux;

	if (state == strip->state[channel])
		return;

	// set new state
	strip->state[channel] = state;

	// swap led buffers
	aux = strip->alt_buf[channel];
	strip->alt_buf[channel] = strip->strip.channel[0].leds;
	strip->strip.channel[0].leds = aux;

	// render
	ws2811_render(&(strip->strip));
} 

/**
 * @brief Get a strip's state.
 * 
 * @param strip The strip.
 * @param channel The channel.
 * @return int The state. (OFF = 0, ON = !0)
 */
int strip_state_get(strip_t* strip, int channel) {
	return strip->state[channel];
}

/**
 * @brief Sets the strip's data from the start up to a len.
 * 
 * Sets the LEDs from 0 up until blen to whatever is inside the buffer.
 * 
 * @param strip The strip operand.
 * @param channel The effective channel to set the data.
 * @param buffer The buffer containing what the strip should contain.
 * @param blen The length of the buffer.
 */
void strip_buffer_start_set(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen) {
	ws2811_led_t* buf;
	int length;

	// get right led buffer
	buf = strip_crightbuf(strip, channel);

	// get real length
	length = MIN(blen, strip_channel_count(strip, channel));

	// memcpy it
	memcpy(buf, buffer, length * sizeof(ws2811_led_t));
}

/**
 * @brief Sets a subset of the strip to whatever's in buffer.
 * 
 * Sets the contents of the strip from _start_ until _end_ to the contents
 * of _buffer_. This function will correctly set the boundaries: if a buffer
 * shorter than _end - start_ is given, then it will set from _start_ until
 * _start + blen_ to the contents provided in buffer.
 * 
 * @param strip The strip operand.
 * @param channel The effective channel to use.
 * @param buffer The buffer containing the new values.
 * @param blen The length of the given buffer.
 * @param start Starting index.
 * @param end End index.
 */
void strip_buffer_sub_set(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen, int start, int end) {
	ws2811_led_t* buf;
	int length;

	start = MAX(0, start);
	end = MAX(0, end);
	end = MIN(strip_channel_count(strip, channel), end);

	buf = strip_crightbuf(strip, channel);

	length = MIN(blen, end - start);
	length = MIN(length, strip_channel_count(strip, channel) - start);
	if (length < 0)
		return;

	memcpy(&(buf[start]), buffer, length * sizeof(ws2811_led_t));
}

/**
 * @brief Inserts into the strips the colors provided in buffer.
 * 
 * This function inserts the contents of buffer into the strip, effectively
 * moving whatever was at the position the buffer was inserted to right after
 * the buffer's end. Inserting WWW into a 7-colored strip containing ABCDEFG at
 * index 2 would result in the contents of the strip being ABWWWCD.
 * 
 * @param strip The strip operand.
 * @param channel The effective channel.
 * @param buffer The buffer containing the data to insert.
 * @param blen The length of the given buffer.
 * @param start The index to insert the buffer into.
 */
void strip_buffer_insert(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen, int start) {
	int length, striplen;
	ws2811_led_t* buf;

	buf = strip_crightbuf(strip, channel);

	striplen = strip_channel_count(strip, channel);
	start = MIN(start, striplen);
	length = MIN(blen, striplen - start);

	memmove(&(buf[start + length]), &(buf[start]),
		(striplen - (start + length)) * sizeof(ws2811_led_t));
	memcpy(&(buf[start]), buffer, blen * sizeof(ws2811_led_t));
}

/**
 * @brief Rotates a strip's buffer a given amount.
 * 
 * Rotates a strip's buffer a given amount at a given direction. If the amount
 * is positive, the buffer is rotated towards the end. If the amount is negative
 * then it is rotated towards the beginning. An amount of -3, for example, would
 * transform the ABCDEFG strip into DEFGABC, while an amount of 3 would
 * transform the same ABCDEFG strip into EFGABCD.
 * 
 * @param strip The strip operand.
 * @param channel The effective channel to use.
 * @param amount The amount to rotate. (eg: -3 or 10)
 */
void strip_buffer_rotate(strip_t* strip, int channel, int amount) {
	ws2811_led_t *buf, *tmp;
	int direction, striplen;

	buf = strip_crightbuf(strip, channel);
	striplen = strip_channel_count(strip, channel);

	direction = (amount < 0) ? -1 : 1;
	amount = abs(amount);
	amount = amount % striplen;
	amount = MIN(amount, striplen);

	if (amount > striplen / 2) {
		direction *= -1;
		amount = striplen - amount;
	}

	tmp = (ws2811_led_t*)malloc(sizeof(ws2811_led_t) * amount);
	if (!tmp) {
		fprintf(stderr, "Couldn't allocate strip rotation buffer.\n");
		return;
	}

	if (direction > 0) { // rotate right
		// copy the last amount numbers to tmp
		memcpy(tmp, &(buf[striplen - amount]), amount * sizeof(ws2811_led_t));

		// memmove the first (striplen - amount) numbers to the end
		memmove(&(buf[amount]), buf, (striplen - amount) * sizeof(ws2811_led_t));

		// copy last amount numbers to the beginning of buf
		memcpy(buf, tmp, amount * sizeof(ws2811_led_t));

	} else { // rotate left
		// copy first amount numbers to a temp location
		memcpy(tmp, &(buf[amount]), amount * sizeof(ws2811_led_t));

		// memmove the last (striplen - amount) to the beginnig
		memmove(buf, &(buf[amount]),
			(striplen - amount) * sizeof(ws2811_led_t));
		
		// copy tmp back into the end of buf
		memcpy(&(buf[striplen - amount]), tmp, amount * sizeof(ws2811_led_t));
	}
}

/**
 * @brief Shifts a strip's buffer a given amount.
 * 
 * This function shifts a strip's buffer _amount_ LEDs inserting into the void
 * space the _insert_ color. The shifting direction is provided by the 
 * _amount_'s sign in a behavior similar to rotate's. Shifting 'A' into the
 * ABCDEFG strip with the amount of -3 would result into DEFGAAA. The same shift
 * using 3 as amount would yield AAAABCD.
 * 
 * @param strip The strip operand.
 * @param channel The channel to use.
 * @param amount The amount to shift.
 * @param insert The color to insert.
 */
void strip_buffer_shift(strip_t* strip, int channel, int amount,
	ws2811_led_t insert) {
	ws2811_led_t *buf, *bstart, *bend, *it;
	int striplen, direction;

	buf = strip_crightbuf(strip, channel);
	striplen = strip_channel_count(strip, channel);

	direction = (amount < 0) ? -1 : 1;
	amount = abs(amount);
	amount = amount % striplen;
	amount = MIN(amount, striplen);

	if (amount > striplen / 2) {
		direction *= -1;
		amount = striplen - amount;
	}

	if (direction > 0) { // shift right
		memmove(&(buf[amount]), buf, (striplen - amount) * sizeof(ws2811_led_t));
		bstart = buf;
		bend = &(buf[amount]);
	} else { // shift left
		memmove(buf, &(buf[amount]), (striplen - amount) * sizeof(ws2811_led_t));
		bstart = &(buf[striplen - amount]);
		bend = &(buf[striplen]);
	}

	for (it = bstart; it < bend; it++)
		*it = insert;
}

/**
 * @brief Fills a strip's buffer with a given color.
 * 
 * @param strip The strip operand.
 * @param channel The channel to use.
 * @param insert The color to fill the strip's buffer with.
 */
void strip_buffer_fill(strip_t* strip, int channel, ws2811_led_t insert) {
	int striplen, i;
	ws2811_led_t* buf;

	buf = strip_crightbuf(strip, channel);
	striplen = strip_channel_count(strip, channel);

	for (i = 0; i < striplen; i++)
		buf[i] = insert;
}

/**
 * @brief Sets the LED at a given index to a given color.
 * 
 * @param strip The strip operand.
 * @param channel The channel to use.
 * @param index The index of the led to set.
 * @param in The color to set the led to.
 */
void strip_buffer_set_index(strip_t* strip, int channel, uint16_t index, ws2811_led_t in) {
	ws2811_led_t* buf;

	buf = strip_crightbuf(strip, channel);

	index = MIN(index, strip_channel_count(strip, channel) - 1);

	buf[index] = in;
}

/**
 * @brief Sends the data to the actual LED strip.
 * 
 * @param strip The strip to render.
 */
void strip_render(strip_t* strip) {
	ws2811_render(&(strip->strip));
}

/**
 * @brief Initializes a strip "object".
 * 
 * @param strip The strip object pointer.
 */
void strip_init(strip_t* strip) {
	int i;

	// init ws2811_t
	ws2811_init(&(strip->strip));

	// allocate alt buffers
	for (i = 0; i < RPI_PWM_CHANNELS; i++) {
		strip->alt_buf[i] = (ws2811_led_t*)calloc(strip->strip.channel[i].count,
			sizeof(ws2811_led_t));
	}
}

/**
 * @brief Finalizes the strip "object", freeing all memory used by it.
 * 
 * Keep in mind that this function does not free the strip_t* passed to it,
 * as it should have been allocated by whoever called the function and this will
 * only manage the memory it created. It is also like this so strip "objects"
 * can live on the stack or in global memory.
 * 
 * @param strip The strip "object".
 */
void strip_fini(strip_t* strip) {
	int i;

	for (i = 0; i < RPI_PWM_CHANNELS; i++)
		free(strip->alt_buf[i]);

	ws2811_fini(&(strip->strip));
}

/**
 * @brief Resizes a strip's buffer to a given length.
 * 
 * @param strip The strip operand.
 * @param channel The channel to resize.
 * @param newlen 
 * @return int 
 */
int strip_resize(strip_t* strip, int channel, int newlen) {
	if (newlen < 0)
		return 0;

	if (strip->strip.channel[channel].count == newlen)
		return 1;

	strip->strip.channel[channel].leds = (ws2811_led_t*)realloc(
		strip->strip.channel[channel].leds, newlen * sizeof(ws2811_led_t));
	if (strip->strip.channel[channel].leds == NULL) {
		fprintf(stderr, "Error realloc'ing the strip buffer for channel %d.\n",
			channel);
		exit(EXIT_FAILURE);
	}

	strip->alt_buf[channel] = (ws2811_led_t*)realloc(
		strip->alt_buf[channel], newlen * sizeof(ws2811_led_t));
	if (strip->alt_buf[channel] == NULL) {
		fprintf(stderr, "Error realloc'ing the strip altbuf for channel %d.\n",
			channel);
		exit(EXIT_FAILURE);
	}

	strip->strip.channel[channel].count = newlen;

	return 1;
}