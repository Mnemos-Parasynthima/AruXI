CC = gcc
CFLAGS = -Wall
INCLUDES = -I../../headers/emulator/cpu -I../../headers/emulator -I../../headers

SRCS = cpu.c cpuError.c ../../shared/emSignal.c
TARGET = ../../out/iaru0

# ifeq ($(MAKECMDGOALS),windows)
# SRCS := $(SRCS) getline.c
# TARGET = ../out/arxsm.exe
# endif

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: cpu

cpu: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

debug: CFLAGS += -g -O0 -DDEBUG
debug: cpu

# lib: $(SRCS)
# $(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o
	rm -f ../../shared/*.o
	rm -f ../../out/iaru0