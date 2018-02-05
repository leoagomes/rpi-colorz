#include "xpc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

extern int XPC_PORT;
extern ws2811_t strip;
extern uv_loop_t* loop;

uv_tcp_t xpc;

void xpc_packet_parse(uv_buf_t* buf) {

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

	xpc = uv_tcp_init(loop, &xpc);
	uv_ip4_addr("0.0.0.0", XPC_PORT, &addr);

	uv_tcp_bind(&xpc, &addr, 0);

	if ((r = uv_listen((uv_stream_t*)&xpc, 10, on_xpc_connection))) {
		fprintf(stderr, "Error listening to XPC connections: %s.\n",
			uv_strerror(r));
		exit(EXIT_FAILURE);
	}
}