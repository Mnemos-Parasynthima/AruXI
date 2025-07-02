CC = gcc
CFLAGS = -Wall


all: arxsm ruemu kernel shell

windows:
	(cd assembler && make $@)

arxsm:
	(cd assembler && make $@)

ruemu:
	(cd emulator && make $@)

kernel: arxsm
	(cd kernel && make $@)

shell:
	(cd shell && make $@)

debug:
	(cd assembler && make $@)

lib:
	(cd assembler && make $@)

test:
	(cd testsuite/assembler && go test)


clean:
	(cd assembler && make $@)
	rm -f *.o
	rm -f *.so
	rm -f out/arxsm
	rm -f out/*.ark