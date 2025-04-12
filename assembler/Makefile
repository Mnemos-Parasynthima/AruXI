CC = gcc
CFLAGS = -Wall
INCLUDES = -I../headers/assembler

SRCS = assembler.c assemblerError.c DataTable.c encoder.c evaluator.c InstructionStream.c SectionTable.c SymbolTable.c preprocessor.c lexer-parser.c
TARGET = ../out/arxsm

OBJS = $(SRCS:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: arxsm

arxsm: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

debug: CFLAGS += -g -O0
debug: arxsm

lib: $(SRCS)
	$(CC) -fPIC -shared -o ../out/libarxsm.so $(SRCS) $(INCLUDES)

clean:
	rm -f *.o