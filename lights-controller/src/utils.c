#include "utils.h"

#include <stdlib.h>

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*)malloc(suggested_size); // TODO: where is the free?
	buf->len = suggested_size;
}