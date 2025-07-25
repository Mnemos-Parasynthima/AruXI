CC = gcc
CFLAGS = -Wall


all: kernel ruemu cpu shell

windows:
	(cd assembler && make $@)

arxsm-legacy:
	(cd assembler-legacy && make $@)

arxsm:
	(cd build-utils && make $@)

arxlnk:
	(cd build-utils && make $@)

ruemu:
	(cd emulator && make $@)

cpu:
	(cd emulator/cpu && make $@)

kernel: arxsm arxlnk
	(cd kernel && make $@)

shell:
	(cd shell && make $@)

debug:
	(cd build-utils && make $@)
	(cd emulator && make $@)
	(cd shell && make $@)

lib:
	(cd assembler && make $@)

test:
	(cd testsuite/assembler && go test)


clean:
	(cd build-utils && make $@)
	(cd emulator && make $@)
	(cd shell && make $@)

clean_logs:
	rm -f out/*.debug
	rm -f out/*.log