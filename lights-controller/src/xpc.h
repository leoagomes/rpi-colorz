#pragma once

#include <uv.h>

void xpc_packet_parse(uv_buf_t* buf);
void on_xpc_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void on_xpc_connection(uv_stream_t* connection, int status);
void xpc_init();