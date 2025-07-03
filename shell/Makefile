SHARED = ../shared
HEADERS = ../headers
OUT = ../out

CC = gcc
CFLAGS = -Wall
INCLUDES = -I$(HEADERS)/shell -I$(HEADERS)

SRCS = shell.c signalHandler.c $(SHARED)/emSignal.c $(SHARED)/diagnostics.c
TARGET = $(OUT)/ash

# ifeq ($(MAKECMDGOALS),windows)
# SRCS := $(SRCS) getline.c
# TARGET = ../out/arxsm.exe
# endif

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: shell

shell: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

debug: CFLAGS += -g -O0 -DDEBUG=1
debug: shell

# lib: $(SRCS)
# $(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o
	rm -f $(SHARED)/*.o
	rm -f $(OUT)/ash