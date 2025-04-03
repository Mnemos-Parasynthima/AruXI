CC = gcc
CFLAGS = -Wall


all: arxsm emu kernel

arxsm:
	(cd assembler && make $@)

emu:


kernel:


clean:
	(cd assembler && make $@)
	rm -f *.o
	rm -f out/arxsm