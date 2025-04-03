CC = gcc
CFLAGS = -Wall
INCLUDES = -I../headers/assembler

SRCS = assembler.c
TARGET = ../out/arxsm

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: arxsm

arxsm: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

debug: CFLAGS += -g -O0
debug: arxsm

clean:
	rm -f *.o