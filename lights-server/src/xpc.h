/**
 * @brief XPC function prototypes and macros.
 * 
 * @file xpc.h
 * @author Leonardo G.
 */
#pragma once

#include <uv.h>

#define DEFAULT_XPC_PORT 7890 /*!< The default value for the XPC */

void xpc_packet_parse(uv_stream_t* stream, uv_buf_t* buf);
void on_xpc_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
void on_xpc_connection(uv_stream_t* connection, int status);
void xpc_init();