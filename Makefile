CC = gcc
CFLAGS = -Wall


all: arxsm cpu ruemu kernel shell

windows:
	(cd assembler && make $@)

arxsm:
	(cd assembler && make $@)

ruemu:
	(cd emulator && make $@)

cpu:
	(cd emulator/cpu && make $@)

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
	(cd emulator && make $@)
	(cd shell && make $@)
	rm -f *.o
	rm -f *.so
	rm -f out/arxsm
	rm -f out/*.ark