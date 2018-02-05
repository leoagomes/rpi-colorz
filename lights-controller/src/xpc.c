#include "xpc.h"

#include "utils.h"
#include "strip.h"

#include "proto.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// TODO: improve error handling

extern strip_t strip; // defined in strip.c
extern uv_loop_t* loop; // defined in main.c

int XPC_PORT = DEFAULT_XPC_PORT;
uv_tcp_t xpc;

void xpc_packet_parse(uv_buf_t* buf) {
	uint8_t* buffer;
	uint8_t op, channel;
	uint16_t length, start, end;

	buffer = (uint8_t*)buf->base;
	if (buffer == NULL)
		return;

	op = buffer[0];
	channel = MIN(buffer[1], strip_last_channel(&strip));

	switch (op) {
	case PROTO_BUFFER_START_SET: // buffer start set: [op][channel][short: length][uint8_t*: ...]
		length = *((uint16_t*)&(buffer[2])); // get given length
		length = MIN(length, buf->len - 4); // get copy data length

		strip_buffer_start_set(&strip, channel, (ws2811_led_t*)(&(buffer[4])),
			length);
		strip_renderNPM(strip);
		break;
	case PROTO_BUFFER_SPLICE: // buffer splice: [op][channel][short: start][short: end][uint8_t*: data]
		start = *((uint16_t*)&(buffer[2]));
		end = *((uint16_t*)&(buffer[4]));
		length = end - start; // given length
		length = MIN(buf->len - 6, length); // limited to data length

		strip_buffer_sub_set(&strip, channel, (ws2811_led_t*)(&(buffer[6])),
			length, start, end);
		strip_renderNPM(strip);
		break;

	case PROTO_STRIP_GET_COUNT: // get strip count: [op][channel]
		break;
	case PROTO_STRIP_CHANGE_COUNT: // change strip length: [op][channel]
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
		xpc_packet_parse(&parsebuf);
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