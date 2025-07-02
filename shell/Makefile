CC = gcc
CFLAGS = -Wall
INCLUDES = -I../headers/shell -I../headers

SRCS = shell.c shellError.c ../shared/emSignal.c
TARGET = ../out/ash

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


debug: CFLAGS += -g -O0 -DDEBUG
debug: shell

# lib: $(SRCS)
# $(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o
	rm -f ../shared/*.o
	rm -f ../out/ash