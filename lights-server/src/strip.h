/**
 * @brief Strip operating function prototypes.
 * 
 * @file strip.h
 * @author Leonardo G.
 */
#pragma once

#include <stdint.h>
#include "ws2811.h"

#define DEFAULT_FREQ 800000
#define DEFAULT_GPIO 18
#define DEFAULT_DMA 10
#define DEFAULT_LEDCOUNT 50
#define DEFAULT_STRIPTYPE WS2811_STRIP_RGB
#define DEFAULT_INVERT 0
#define DEFAULT_BRIGHTNESS 255

#define MAX_SUPPORTED_STRIPS RPI_PWM_CHANNELS

typedef struct strip_t strip_t;

enum strip_state_t {
	STRIP_OFF,
	STRIP_ON, // Here ON is 1, but to the functions ON is just not zero.
};

struct strip_t {
	ws2811_t strip; /*!< The rpi_ws2811 strip */
	ws2811_led_t* alt_buf[MAX_SUPPORTED_STRIPS]; /*!< Stores on/off buffers */
	int state[MAX_SUPPORTED_STRIPS]; /*!< The state of each channel (on/off) */
};

#define strip_inner_param(s,p) (s)->strip.(p)
#define stripp_inner_param(s,p) (s).strip.(p)

#define strip_channel_count(s,c) ((s)->strip.channel[c].count)
#define strip_crightbuf(s,c) \
	(strip_cis_on((s),(c)) ? (s)->strip.channel[c].leds : (s)->alt_buf[c])

#define strip_cis_on(s,c) ((s)->state[c] == STRIP_ON)

#define strip_renderPM(s) (ws2811_render(&((s)->strip)))
#define strip_renderNPM(s) (ws2811_render(&((s).strip)))

#define strip_last_channel(s) (RPI_PWM_CHANNELS - 1)

void strip_state_set(strip_t* strip, int channel, int state);
int strip_state_get(strip_t* strip, int channel);

void strip_buffer_start_set(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen);
void strip_buffer_sub_set(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen, int start, int end);
void strip_buffer_insert(strip_t* strip, int channel, ws2811_led_t* buffer,
	int blen, int start);
void strip_buffer_rotate(strip_t* strip, int channel, int amount);
void strip_buffer_shift(strip_t* strip, int channel, int amount,
	ws2811_led_t insert);
void strip_buffer_fill(strip_t* strip, int channel, ws2811_led_t insert);
void strip_buffer_set_index(strip_t* strip, int channel, uint16_t index,
	ws2811_led_t in);

void strip_render(strip_t* strip);

void strip_init(strip_t* strip);
void strip_fini(strip_t* strip);

int strip_resize(strip_t* strip, int channel, int newlen);