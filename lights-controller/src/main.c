#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <getopt.h>

#include "xpc.h"
#include "qpc.h"
#include "strip.h"

extern int XPC_PORT; // defined in xpc.c
extern int QPC_PORT; // defined in qpc.c
extern strip_t strip; // defined in strip.c

int VERBOSE = 0;
uv_loop_t* loop;

void print_help(char* argv0);
void print_usage(char* argv0);
void parse_parameters(int argc, char** argv);

int main(int argc, char* argv[]) {
	// set uv loop
	loop = uv_default_loop();

	// parse command parameters
	parse_parameters(argc, argv);

	// initialize the strip
	ws2811_init(&strip);

	// initialize Quick Pixel Control handlers
	qpc_init();

	// run the event loop
	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}

void print_help(char* argv0) {
	printf("help: %s [options]\n"
	       "\t--xpc,-x <port>         -- set XPC port (default 7890)\n"
	       "\t--qpc,-q <port>         -- set QPC port (default 7891)\n"
	       "\t--verbose, -v           -- set verbose mode (default off)\n"
	       "\t--help, -h              -- display this help text\n"
	       "\t--freq,-f <freq>        -- set controller frequency (default 1200000)\n"
	       "\t--dma,-d <num>          -- set dma channel (default 10)\n"
	       "\t--ledcount,-c <count>     -- set the number of leds (default 50)\n"
	       "\t--gpio,-g <pin>         -- set the gpio pin to use (default 10 -- SPI)\n"
	       "\t--strip-type,-t <mode>  -- set strip mode (default BGR)\n"
	       "\t--invert,-i             -- will invert the output at gpio\n"
	       "\t--brightness,-b <value> -- set the brightness (default 255)\n"
	       "\nremarks:\n"
	       "\t* <port> are valid tcp/udp port values\n"
	       "\t* <freq> is in Hz, so 800kHz would be 800000\n"
	       "\t* be careful with dma channels, 10 seems to work just fine.\n"
	       "\t* changing <pin> changes the method used by the server:\n"
	       "\t  which means different permissions will be needed by the server\n"
	       "\t  depending on the interface used, so keep that in mind.\n"
	       "\t* valid <mode> values are: 'rgb', 'rbg', 'grb', 'gbr', 'brg', 'bgr'\n"
	       "\t  'rgbw' and 'grbw'.\n", argv0);
}

void print_usage(char* argv0) {
	printf("usage: %s [[-x <port>] [-q <port>] [-v] [-h]]\n", argv0);
}

void parse_parameters(int argc, char** argv) {
	int c, option_index;
	static struct option long_options[] = {
		{"xpc", required_argument, NULL, 'x'},
		{"qpc", required_argument, NULL, 'q'},
		{"verbose", no_argument, &VERBOSE, 1},
		{"help", no_argument, NULL, 'h'},
		{"freq", required_argument, NULL, 'f'},
		{"dma", required_argument, NULL, 'd'},
		{"ledcount", required_argument, NULL, 'c'},
		{"gpio", required_argument, NULL, 'g'},
		{"strip-type", required_argument, NULL, 't'},
		{"invert", no_argument, &(strip.strip.channel[0].invert), 1},
		{"brightness", required_argument, NULL, 'b'},
		{0,0,0,0}
	};

	while (1) {
		option_index = 0;
		c = getopt_long(argc, argv, "x:q:vhf:d:l:g:m:ib:", long_options,
			&option_index);

		if (c == -1)
			break;

		switch (c) {
		case 'x':
			XPC_PORT = atoi(optarg);
			break;
		case 'q':
			QPC_PORT = atoi(optarg);
			break;
		case 'v':
			VERBOSE = 1;
			break;
		case 'h':
			print_help(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'f':
			strip.strip.freq = atoi(optarg);
			break;
		case 'd':
			strip.strip.dmanum = atoi(optarg);
			break;
		case 'c':
			strip.strip.channel[0].count = atoi(optarg);
			break;
		case 'g':
			strip.strip.channel[0].gpionum = atoi(optarg);
			break;
		case 't':
			if (!strcasecmp(optarg, "rgb"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_RGB;
			else if (!strcasecmp(optarg, "rbg"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_RBG;
			else if (!strcasecmp(optarg, "grb"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_GRB;
			else if (!strcasecmp(optarg, "gbr"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_GBR;
			else if (!strcasecmp(optarg, "bgr"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_BGR;
			else if (!strcasecmp(optarg, "brg"))
				strip.strip.channel[0].strip_type = WS2811_STRIP_BRG;
			else if (!strcasecmp(optarg, "rgbw"))
				strip.strip.channel[0].strip_type = SK6812_STRIP_RGBW;
			else if (!strcasecmp(optarg, "grbw"))
				strip.strip.channel[0].strip_type = SK6812_STRIP_GRBW;
			else {
				printf("invalid strip type: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'i':
			strip.strip.channel[0].invert = 1;
			break;
		case 'b':
			strip.strip.channel[0].brightness = (uint8_t)atoi(optarg);
			break;
		}
	}
}
