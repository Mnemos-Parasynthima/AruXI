CC = gcc
CFLAGS = -Wall
INCLUDES = -I../headers/emulator -I../headers

SRCS = emulator.c emulatorError.c ../shared/emSignal.c
TARGET = ../out/ruemu

# ifeq ($(MAKECMDGOALS),windows)
# SRCS := $(SRCS) getline.c
# TARGET = ../out/arxsm.exe
# endif

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: ruemu

ruemu: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)


debug: CFLAGS += -g -O0 -DDEBUG
debug: ruemu

# lib: $(SRCS)
# $(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o
	rm -f ../shared/*.o