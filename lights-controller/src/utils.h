#pragma once

#include <stdint.h>
#include <uv.h>

#include "ws2811.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* type conversion helpers */
#define as(T,p) (*((T*)(p)))
#define v_as(T,v) (*((T*)(&(v))))

#define as_uint(p)      as(uint32_t, p)
#define as_ushort(p)    as(uint16_t, p)
#define as_byte(p)      as(uint8_t, p)
#define as_int(p)       as(int, p)
#define as_short(p)     as(int16_t, p)
#define as_char(p)      as(char, p)

#define v_as_uint(p)    v_as(uint32_t, p)
#define v_as_ushort(p)  v_as(uint16_t, p)
#define v_as_byte(p)    v_as(uint8_t, p)
#define v_as_int(p)     v_as(int, p)
#define v_as_short(p)   v_as(int16_t, p)
#define v_as_char(p)    v_as(char, p)

void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

ws2811_led_t* ntoh_buffer(ws2811_led_t* buffer, size_t len);