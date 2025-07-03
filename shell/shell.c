#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include "emSignal.h"
#include "shmem.h"
#include "diagnostics.h"
#include "signalHandler.h"


const char* PROMPT = "ash> ";

int main(int argc, char const* argv[]) {
	initDiagnostics(stdout, "shell.debug");

	dLog(D_NONE, DSEV_INFO, "[ASH]: Loading...");
	// fprintf(stdout, "[ASH]: Loading...\n");

	// shell only has access to signal memory

	int fd = shm_open(SHMEM_SIG, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for signal!");

	void* _sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_sigMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap signal memory!");

	close(fd);

	redefineSignal(SIGINT, &handleInterrupt);

	signal_t* sigMem = (signal_t*) _sigMem;

	// Block until signal
	signal_t* universalSig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	dLog(D_NONE, DSEV_INFO, "[ASH]: Will not wait for read...");
	uint8_t ready = 0x0;
	while (ready != 0x1) {
		ready = SIG_GET(universalSig->interrupts, emSIG_READY_IDX);
	}

	// At this point, stuff has been set up
	// But the shell must run after the kernel has finished running and the cpu is idle
	// This will happen by the cpu setting the acknowledged on the shell-cpu:exec signal

	signal_t* shellCPUSig = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	dLog(D_NONE, DSEV_INFO, "[ASH]: Will now wait for exec acknowledged...");
	uint8_t ackd = 0x0;
	while (ackd != 0x1) {
		ackd = SIG_GET(shellCPUSig->ackMask, emSIG_EXEC_IDX);
	}

	printf("%s", PROMPT);

	char* line = NULL;
	size_t n;
	getline(&line, &n, stdin);

	printf("Got %s", line);
	printf("%s", PROMPT);
	
	munmap(sigMem, SIG_SIZE*4);

	return 0;
}