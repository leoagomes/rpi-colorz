#include "strip.h"

#include "utils.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

strip_t strip = {
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

int strip_state_get(strip_t* strip, int channel) {
	return strip->state[channel];
}

void strip_buffer_start_set(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen) {
	ws2811_led_t* buf;
	int length;

	// get right led buffer
	buf = strip_crightbuf(strip, channel);

	// get real length
	length = MIN(blen, strip_channel_count(strip, channel));

	// memcpy it
	memcpy(buf, buffer, length);
}

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

	memcpy(&(buf[start]), buffer, length);
}

void strip_buffer_insert(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen, int start) {
	int length;

	length = 
}

void strip_buffer_rotate(strip_t* strip, int channel, int amount) {

}

void strip_buffer_shift(strip_t* strip, int channel, int amount,
	ws2811_led_t insert) {

}

void strip_render(strip_t* strip) {
	ws2811_render(&(strip->strip));
}

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

void strip_fini(strip_t* strip) {
	int i;

	for (i = 0; i < RPI_PWM_CHANNELS; i++)
		free(strip->alt_buf[i]);

	ws2811_fini(&(strip->strip));
}

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