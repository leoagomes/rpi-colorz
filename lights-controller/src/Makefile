EXECUTABLE=lc
SOURCES=qpc.c strip.c utils.c xpc.c
INCLUDE_FLAGS=-I$(WS281X_DIR)
CFLAGS=$(INCLUDE_FLAGS) `pkg-config --cflags libuv`

LFLAGS=`pkg-config --libs libuv`

CC ?= gcc

all: $(EXECUTABLE)

debug: CFLAGS += -g
debug: all

$(EXECUTABLE): main.c $(SOURCES:.c = .o)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

%.o: %.c
	$(CC) -c %< -o $@ $(CFLAGS)

clean:
	find -name "*.o" -type f -delete
	find -name "$(EXECUTABLE)" -type f -delete

run:
	$(EXECUTABLE)

.PHONY: all clean run debug