/**
 * @brief QPC implementation file.
 * 
 * @file qpc.c
 * @author Leonardo G.
 */
#include "qpc.h"

#include "utils.h"
#include "strip.h"

#include "proto.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// TODO: improve error handling

extern strip_t strip;
extern uv_loop_t* loop;

int QPC_PORT = DEFAULT_QPC_PORT; //!< The Port QPC should listen on
uv_udp_t qpc; //!< The global QPC UDP LibUV handle

/**
 * @brief Parses a packet sent to the QPC port.
 * 
 * This function parses and executes packets sent according to QPC definitions
 * (which follow somewhat closely the XPC definitions).
 * 
 * @param buf The buffer which contains the packet's data.
 */
void qpc_packet_parse(const uv_buf_t* buf) {
	uint8_t* buffer;
	uint8_t op, channel;
	uint16_t length, start, end;

	buffer = (uint8_t*)buf->base;
	if (buffer == NULL)
		return;

	op = buffer[0];
	channel = MIN(buffer[1], strip_last_channel(&strip));

	// TODO: implement other operations
	switch (op) {
	case PROTO_BUFFER_START_SET: // buffer start set: [op: 0][channel][short: length][uint8_t*: ...]
		length = as_ushort(&(buffer[2])); // get given length
		length = MIN(length, buf->len - 4); // get copy data length

		strip_buffer_start_set(&strip, channel, (ws2811_led_t*)(&(buffer[4])),
			length);
		strip_renderNPM(strip);
		break;
	case PROTO_BUFFER_SPLICE: // buffer splice: [op: 1][short: start][short: end][uint8_t*: data]
		start = as_ushort(&(buffer[2]));
		end = as_ushort(&(buffer[4]));

		length = end - start; // given length
		length = MIN(buf->len - 6, length); // limited to data length

		strip_buffer_sub_set(&strip, channel, (ws2811_led_t*)(&(buffer[6])),
			length, start, end);
		strip_renderNPM(strip);
		break;
	case 255: // vendor specific
		break;
	default:
		fprintf(stderr, "Request for undefined op\n");
		break;
	}
}

/*
 * Handler callback for LibUV's recv.
 */
void on_qpc_read(uv_udp_t* req, ssize_t nread, const uv_buf_t* buf,
	const struct sockaddr* addr, unsigned flags) {

	if (addr == NULL)
		return;

	if (nread == -1) {
		fprintf(stderr, "Error reading received QPC data.");
		uv_close((uv_handle_t*)req, NULL);
		free(buf->base);
		return;
	}

	qpc_packet_parse(buf);

	uv_udp_recv_stop(req);
	free(buf->base);
}

/**
 * @brief Initializes QPC callbacks.
 */
void qpc_init() {
	struct sockaddr_in recv_address;
	uv_udp_init(loop, &qpc);
	uv_ip4_addr("0.0.0.0", QPC_PORT, &recv_address);
	uv_udp_bind(&qpc, (struct sockaddr*)&recv_address, 0);
	uv_udp_recv_start(&qpc, alloc_cb, on_qpc_read);
}