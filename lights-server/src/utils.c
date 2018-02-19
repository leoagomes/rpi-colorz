#include "utils.h"

#include <stdlib.h>

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size); // TODO: where is the free?
	buf->len = suggested_size;
}

ws2811_led_t* ntoh_buffer(ws2811_led_t* buffer, size_t len) {
	size_t i;
	for (i = 0; i < len; i++)
		buffer[i] = (ws2811_led_t)ntohl(buffer[i]);
	return buffer;
}