CC = gcc
CFLAGS = -Wall
ROOT = ../..
OUT = $(ROOT)/out

INCLUDES = -I$(ROOT)/headers/build-utils/assembler -I$(ROOT)/headers/build-utils -I$(ROOT)/headers

SRCS = assembler.c assemblerDiganostics.c DataTable.c encoder.c evaluator.c generator.c \
	InstructionStream.c SectionTable.c SymbolTable.c preprocessor.c lexer-parser.c RelocationTable.c
TARGET = $(OUT)/arxsm

ifeq ($(MAKECMDGOALS),windows)
SRCS := $(SRCS) getline.c
TARGET = $(OUT)/arxsm.exe
endif

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: arxsm

arxsm: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

windows: CC = zig cc
windows: CFLAGS += --target=x86_64-windows
windows: arxsm

debug: CFLAGS += -g -O0 -DDEBUG
debug: arxsm

lib: $(SRCS)
	$(CC) -fPIC -shared -o $(OUT)/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o