SHARED = ../shared
HEADERS = ../headers
OUT = ../out

CC = gcc
CFLAGS = -Wall
INCLUDES = -I$(HEADERS)/emulator -I$(HEADERS)

SRCS = emulator.c loader.c $(SHARED)/diagnostics.c $(SHARED)/emSignal.c $(SHARED)/signalHandler.c
TARGET = $(OUT)/ruemu

# ifeq ($(MAKECMDGOALS),windows)
# SRCS := $(SRCS) getline.c
# TARGET = ../out/arxsm.exe
# endif

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: cpu ruemu

ruemu: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

cpu:
	(cd cpu && make $@)

debug: CFLAGS += -g -O0 -DDEBUG=2
debug: ruemu

# lib: $(SRCS)
# $(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	(cd cpu && make $@)
	rm -f *.o
	rm -f $(OUT)/ruemu