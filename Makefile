CC = gcc
CFLAGS = -Wall


all: arxsm emu kernel shell

windows:
	(cd assembler && make $@)

arxsm:
	(cd assembler && make $@)

emu:


kernel:


shell:


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