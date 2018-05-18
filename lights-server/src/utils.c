/**
 * @brief Utility function implementations.
 * 
 * @file utils.c
 * @author Leonardo G.
 */
#include "utils.h"

#include <stdlib.h>
#include <arpa/inet.h>

/*
 * A callback for LibUV.
 */
void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size); // TODO: where is the free?
	buf->len = suggested_size;
}

/**
 * @brief Changes the byte order in a buffer inline.
 * 
 * This function modifies the buffer passed to it.
 * 
 * @param buffer The buffer to change the order.
 * @param len The length of the buffer.
 * @return ws2811_led_t* The new byte order buffer. (Should be the same address
 * as "buffer".)
 */
ws2811_led_t* inline_ntoh_buffer(ws2811_led_t* buffer, size_t len) {
	size_t i;
	for (i = 0; i < len; i++)
		buffer[i] = (ws2811_led_t)ntohl(buffer[i]);
	return buffer;
}

/**
 * @brief Creates a new buffer with a changed byte order.
 * 
 * This function allocates a new buffer. If malloc fails, it will return NULL.
 * 
 * @param buffer The buffer.
 * @param len The given buffer's length.
 * @return ws2811_led_t* A new buffer with the new byte order.
 */
ws2811_led_t* hton_buffer(ws2811_led_t* buffer, size_t len) {
	size_t i;
	ws2811_led_t* resbuf;

	resbuf = (ws2811_led_t*)malloc(len * sizeof(ws2811_led_t));
	if (!resbuf)
		return NULL;

	for (i = 0; i < len; i++)
		resbuf[i] = (ws2811_led_t)htonl(buffer[i]);

	return resbuf;
}

ws2811_led_t* inline_hton_buffer(ws2811_led_t* buffer, size_t len) {
	size_t i;
	ws2811_led_t* resbuf;

	for (i = 0; i < len; i++)
		buffer[i] = (ws2811_led_t)htonl(buffer[i]);

	return buffer;
}