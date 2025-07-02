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
#include "shellError.h"
#include "ipc.h"


int main(int argc, char const* argv[]) {

	
	// shell only has access to signal memory

	int fd = shm_open(SHMEM_SIG, O_RDWR, 0666);
	if (fd == -1) handleError(ERR_SHAREDMEM, FATAL, "Could not open shared memory for signal!\n");

	void* _sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_sigMem == MAP_FAILED) handleError(ERR_MEM, FATAL, "Could not mmap signal memory!\n");

	close(fd);

	signal_t* sigMem = (signal_t*) _sigMem;

	// Block until signal
	signal_t* universalSig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	uint8_t ready = 0x0;
	while (ready != 0x1) {
		ready = SIG_GET(universalSig->interrupts, emSIG_READY_IDX);
	}

	printf("sh> ");

	char* line = NULL;
	size_t n;
	getline(&line, &n, stdin);

	printf("Got %s\n", line);
	printf("sh> \n");
	
	munmap(sigMem, SIG_SIZE*4);

	return 0;
}