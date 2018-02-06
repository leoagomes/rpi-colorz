#include "xpc.h"

#include "utils.h"
#include "strip.h"

#include "proto.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// TODO: improve error handling

extern strip_t strip; // defined in strip.c
extern uv_loop_t* loop; // defined in main.c

int XPC_PORT = DEFAULT_XPC_PORT;
uv_tcp_t xpc;

struct write_req_t {
	uv_write_t req;
	uv_buf_t buffer;
};

void xpc_write_cb(uv_write_t* req, int status) {
	struct write_req_t* wr;

	if (status)
		fprintf(stderr, "Write error: %s.\n", uv_strerror(status));

	wr = (struct write_req_t*)req;

	free(wr->buf.base);
	free(wr);
}

void xpc_packet_parse(uv_stream_t* stream, uv_buf_t* buf) {
	uint8_t* buffer;
	uint8_t op, channel;
	uint16_t length, start, end;
	struct write_req_t* req;

	buffer = (uint8_t*)buf->base;
	if (buffer == NULL)
		return;

	op = buffer[0];
	channel = MIN(buffer[1], strip_last_channel(&strip));

#define __bad_malloc_error(p) if(!(p)) { \
		fprintf(stderr, "Error malloc'ing response.\n"); \
		return; }

	switch (op) {
	case PROTO_BUFFER_START_SET: // buffer start set: [op][channel][short: length][uint8_t*: ...]
		length = ntohs(*((uint16_t*)&(buffer[2]))); // get given length
		length = MIN(length, buf->len - 4); // get copy data length

		strip_buffer_start_set(&strip, channel, (ws2811_led_t*)(&(buffer[4])),
			length);
		strip_renderNPM(strip);
		break;

	case PROTO_BUFFER_SPLICE: // buffer splice: [op][channel][short: start][short: end][uint8_t*: data]
		start = ntohs(*((uint16_t*)&(buffer[2])));
		end = ntohs(*((uint16_t*)&(buffer[4])));
		length = end - start; // given length
		length = MIN(buf->len - 6, length); // limited to data length

		strip_buffer_sub_set(&strip, channel, (ws2811_led_t*)(&(buffer[6])),
			length, start, end);
		strip_renderNPM(strip);
		break;

	case PROTO_BUFFER_INSERT: // buffer insert: [op][channel][ushort: start][ushort: len][data...]
		start = ntohs(*((uint16_t*)&(buffer[2])));
		length = ntohs(*((uint16_t*)&(buffer[4])));

		strip_buffer_insert(strip, channel, (ws2811_led_t*)(&(buffer[6])),
			MIN(length, buf->len), start);
		strip_renderNPM(strip);
		break;

	case PROTO_BUFFER_ROTATE: // buffer rotate: [op][channel][short: amount]
		length = ntohs(*((uint16_t*)&(buffer[2])));
		strip_buffer_rotate(strip, channel, length);
		break;

	case PROTO_BUFFER_SHIFT: // buffer shift: [op][channel][ushort: amount]
		break;

	case PROTO_BUFFER_READ: // buffer read: [op][channel]
		// writes back: [ws2811_led_t* buffer]
		length = strip_channel_count(&strip, channel);

		req = (struct write_req_t*)malloc(sizeof(*req));
		__bad_malloc_error(req);

		req->buf = uv_buf_init((char*)strip->strip.channel[channel].leds,
			length * sizeof(ws2811_led_t));
		uv_write((uv_write_t*)req, stream, &req->buf, 1, xpc_write_cb);

	case PROTO_STRIP_GET_COUNT: // get strip count: [op][channel]
		// writes back: [ushort: count]
		length = htons(strip_channel_count(&strip, channel));

		req = (struct write_req_t*)malloc(sizeof(*req));
		__bad_malloc_error(req);

		req->buf = uv_buf_init((char*)&length, sizeof(uint16_t));
		uv_write(req, stream, &(req->buf), 1, xpc_write_cb);
		break;

	case PROTO_STRIP_CHANGE_COUNT: // change strip length: [op][channel][ushort: length]
		length = noths(*((uint16_t*)(&(buffer[2]))));
		strip_resize(&strip, channel, length);
		break;

	case PROTO_VENDOR_SPECIFIC: // vendor specific
		break;
	default:
		fprintf(stderr, "Request for undefined op\n");
		break;
	}
}

void on_xpc_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	uv_buf_t parsebuf;

	if (nread > 0) {
		parsebuf.base = buf->base;
		parsebuf.len = nread;
		xpc_packet_parse(stream, &parsebuf);
	} else if (nread < 0) {
		if (nread != UV_EOF)
			fprintf(stderr, "Error reading XPC packet data: %s\n",
				uv_strerror(nread));
		uv_close((uv_handle_t*)stream, free);
	}
}

void on_xpc_connection(uv_stream_t* connection, int status) {
	int r;
	uv_stream_t* client;

	if (status < 0) {
		fprintf(stderr, "Error receiving XPC connection (status: %d).\n",
			status);
		return;
	}

	client = (uv_stream_t*)malloc(sizeof(uv_stream_t));
	if (!client) {
		fprintf(stderr, "Failed to allocate memory for new TCP client.\n");
		return;
	}
	uv_tcp_init(loop, client);
	if ((r = uv_accept(connection, client))) { //error
		fpritnf("Error accepction XPC client: %s\n", uv_strerror(r));
		uv_close((uv_handle_t*)client, free);
	} else {
		uv_read_start(client, alloc_cb, on_xpc_read);
	}
}

void xpc_init() {
	struct sockaddr_in addr;
	int r;

	uv_tcp_init(loop, &xpc);
	uv_ip4_addr("0.0.0.0", XPC_PORT, &addr);

	uv_tcp_bind(&xpc, &addr, 0);

	if ((r = uv_listen((uv_stream_t*)&xpc, 10, on_xpc_connection))) {
		fprintf(stderr, "Error listening to XPC connections: %s.\n",
			uv_strerror(r));
		exit(EXIT_FAILURE);
	}
}