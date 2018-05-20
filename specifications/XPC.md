# Extended Pixel Control

**NOTE: this is currently undergoing refactoring.** I don't intend on changing
the functionality described in this file, but I do plan on changing some of the
names of some of the operations. If you feel like you could contribute to any
of this, please send a PR.

XPC is based on OPC, and xtends its operations by adding replacing, shifting,
rotating, etc. There is also the QPC, which is a Quicker alternative to XPC,
relying on UDP for quick remote-control-like actions that require no reply.
QPC message formats are the same as XPC's.

Throughout the description of the supported messages, the following types will
appear.

| Doc Name | C Name       | Brief Description
| -------- | ------------ | -----------------
| byte     | uint8_t      | 8-bit unsigned integer
| char     | char         | 8-bit singed integer
| (u)short | (u)int16_t   | 16-bit (un)signed integer
| (u)int   | (u)int32_t   | 32-bit (un)signed integer
| led_t    | ws2811_led_t | 32-bit unsigned integer used for color values

The types above should be self-explanatory, except led_t, led_t is handled in
the application as a 32-bit integer, the most significant byte of which
represents the White value, followed by Red, then Green, and then Blue. It may
be described simply as 0xWWRRGGBB. This means color intensity values range from
0 to 255. This handling of color values as a combined 32-bit integer means that
when sending data, the user should keep in mind both hardware and network order.
Since the network is big-endian, sending [0xAA, 0xBB, 0xCC, 0xDD] means W = AA,
R = BB, G = CC, B = DD.

## Message Format

A message follows the basic format: `[byte: op][byte: channel](args..)`, where 
`[op]` is the opcode byte, `[channel]` is the strip indexing byte (starting at
zero) and `(args...)` are whatever arguments the operations supports.

The `op` byte tells the message parser which operation to run over a strip,
while `channel` tells which strip to take as operand. For now, setting channel
to a number greater than or equal to the number of strips supported by a device
means using the last indexable strip. There is a plan for the future, however,
to support sending the same message to multiple strips at a time by sending it
to strip 256 (`channel = 255 (or 0xFF)`).

### Available Operations:

The available possible operations, along with their opcode values and format are
provided below. Keep in mind the format's first two fields `[op]` and `[channel]`
are omitted to keep this document visually clean, but should be provided in a
real message.

| Operation  | Opcode | Format
| ---------- | ------ | ------
| start set  | 0x00   | `[ushort: length][led_t[length]: buffer]`
| replace    | 0x01   | `[ushort: start][ushort: length][led_t[length]: buffer]`
| insert     | 0x02   | `[ushort: start][ushort: length][led_t[length]: buffer]`
| rotate     | 0x03   | `[short: amount]`
| shift      | 0x04   | `[short: amount][led_t: in]`
| fill       | 0x05   | `[led_t: color]`
| set index  | 0x06   | `[ushort: index][led_t: color]`
| read       | 0x10   | ``
| get count  | 0x20   | ``
| set count  | 0x21   | `[short: new_count]`
| get state  | 0x22   | ``
| set state  | 0x23   | `[byte: state]`
| get #strips| 0xFE   | `* (see documentation)`
| reserved   | 0xFF   | `(args...)`

#### `start set`

Sets the `length` first leds to what's in `buffer`.

#### `replace`

Substitutes led colors index `start` through `start + length` by the ones in
`buffer`.

#### `insert`

Inserts `length` colors from `buffer` at index `start`, shifting everything
that originally was from `start` to `start + length`.

#### `rotate`

Rotates the strip by `amount` colors. Notice that `amount` is signed and
changing the sign, changes the direction of the rotation.

#### `shift`

Shifts the leds by `amount`, inserting the color `in` in the void extreme.

#### `fill`

Fills the strip with a `color`.

#### `set index`

Sets the color of the led at `index` to `color`.

#### `read`

Returns a strip's `led_t` buffer.

#### `get count`

Gets the length of a strip. (Returns a `ushort`)

#### `set count`

Resizes the strip to `length`.

#### `get state`

Gets a strip's state. (Returns a `byte`)

#### `set state`

Sets a strip's state. (`state = 0 is off`)

#### `get #strips`

Gets the number of strips supported by this device. Even though it makes no
sense, it is mandatory to send a `[channel]` with this operation, its value
will be ignored, though.

#### `reserved`

This is not a standard and its reserved for application specific operations.