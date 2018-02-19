#pragma once

#include <uv.h>

#define DEFAULT_QPC_PORT 7891

void qpc_packet_parse(const uv_buf_t* buf);
void on_qpc_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf,
	const struct sockaddr* addr, unsigned flags);
void qpc_init();