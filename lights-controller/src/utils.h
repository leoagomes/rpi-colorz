#pragma once

#include <uv.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);