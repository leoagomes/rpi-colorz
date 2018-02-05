#include "qpc.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern in QPC_PORT;
extern ws2811_t strip;
extern uv_loopt_t* loop;

uv_udp_t qpc;

void qpc_packet_parse(const uv_buf_t* buf) {
	uint8_t* buffer;
	uint8_t op, channel;
	uint16_t length, start, end;

	buffer = (uint8_t*)buf->base;
	if (buffer == NULL)
		return;

	op = buffer[0];
	channel = buffer[1];

	switch (op) {
	case 0: // buffer start set: [op: 0][short: length][uint8_t*: ...]
		length = *((uint16_t*)&(buffer[2])); // get given length
		length = MIN(length, buf->len - 4); // get copy data length
		length = MIN(length, strip.channel[channel].count); // get real copy len

		memcpy(strip.channel[channel].leds, &(buffer[4]), length);
		ws2811_render(&strip);
		break;
	case 1: // buffer splice: [op: 1][short: start][short: end][uint8_t*: data]
		start = *((uint16_t*)&(buffer[2]));
		end = *((uint16_t*)&(buffer[4]));
		length = end - start; // given length
		length = MIN(buf->len - 6, length); // limited to data length
		length = MIN(length, strip.channel[channel].count - start);

		if (length > 0)
			memcpy(strip.channel[channel].leds, &(buffer[6]), length);

		ws2811_render(&strip);
	case 255: // vendor specific
		break;
	default:
		fprintf("request for undefined op\n");
		break;
	}
}

void on_qpc_read(uv_handle_t* handle, ssize_t nread, const uv_buf_t* buf,
	const struct sockaddr* addr, unsigned flags) {
	uv_udp_t* req;

	req = (uv_udp_t*)handle;

	if (addr == NULL)
		return;

	if (nread == -1) {
		fprintf(stderr, "Error reading received QPC data.");
		uv_close(handle, NULL);
		free(buf->base);
		return;
	}

	qpc_packet_parse(buf);

	uv_udp_recv_stop(req);
	free(buf->base);
}

void qpc_init() {
	uv_udp_init(loop, &qpc);
	struct sockaddr_in recv_address = uv_ip4_addr("0.0.0.0", QPC_PORT);
	uv_udp_bind(&qpc, &recv_address, 0);
	uv_udp_recv_start(&qpc, alloc_cb, on_qpc_read);
}