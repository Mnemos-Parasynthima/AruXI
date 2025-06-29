CC = gcc
CFLAGS = -Wall
INCLUDES = -I../headers/assembler -I../headers

SRCS = assembler.c assemblerError.c DataTable.c encoder.c evaluator.c generator.c InstructionStream.c SectionTable.c SymbolTable.c preprocessor.c lexer-parser.c
TARGET = ../out/arxsm

ifeq ($(MAKECMDGOALS),windows)
SRCS := $(SRCS) getline.c
TARGET = ../out/arxsm.exe
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
	$(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o