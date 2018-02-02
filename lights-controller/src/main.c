#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <getopt.h>

#include "ws2811.h"

#define DEFAULT_FREQ 1200000
#define DEFAULT_GPIO 10
#define DEFAULT_DMA 10
#define DEFAULT_LEDCOUNT 50
#define DEFAULT_STRIPTYPE WS2811_STRIP_BGR
#define DEFAULT_INVERT 0
#define DEFAULT_BRIGHTNESS 255

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

int QPC_PORT = 7891;
int XPC_PORT = 7890;
int VERBOSE = 0;

ws2811_t strip = {
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
};

uv_loop_t* loop;
uv_udp_t qpc;


uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size) {
  return uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void qpc_packet_parse(const uv_buf_t* buf) {
	uint8_t* buffer;
	uint8_t op, channel;
	uint16_t length;

	buffer = (uint8_t*)buf->base;
	if (buffer == NULL)
		return;

	op = buffer[0];
	channel = buffer[1];

	switch (op) {
	case 0:
		length = *((uint16_t*)&(buffer[2]));
		memcpy(strip.channel[channel].leds, &(buffer[4]), MIN(buf->len - 4, length));
		ws2811_render(&strip);
		break;
	case 255:
		break;
	default:
		fprintf("request for undefined op\n");
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
	uv_udp_recv_start(&qpc, alloc_buffer, on_qpc_read);
}

void print_help(char* argv0) {
	printf("help: %s [[-x <port>] [-q <port>] [-v] [-h]]\n"
	       "\t-x <port>     -- set XPC port\n"
	       "\t-q <port>     -- set QPC port\n"
	       "\t-v            -- set verbose mode\n"
	       "\t-h            -- display this help text\n", argv0);
}

void print_usage(char* argv0) {
	printf("usage: %s [[-x <port>] [-q <port>] [-v] [-h]]\n", argv0);
}

void parse_parameters(int argc, char** argv) {
	int c;

	while ((c = getopt(argc, argv, "q:x:vh")) != -1) {
		switch (c) {
		case 'q':
			QPC_PORT = atoi(optarg);
			break;
		case 'x':
			XPC_PORT = atoi(optarg);
			break;
		case 'v':
			VERBOSE = 1;
			break;
		case 'h':
			print_help(argv[0]);
			break;
		case '?':
			print_usage(argv[0]);
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	// set uv loop
	loop = uv_default_loop();

	// parse command parameters
	parse_parameters(argc, argv);

	// initialize the strip
	ws2811_init(&strip);

	// initialize Quick Pixel Control handlers
	qpc_init();

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}