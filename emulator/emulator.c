#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "emSignal.h"
#include "aoef.h"
#include "emulatorError.h"
#include "ipc.h"


/**
 * Loads the kernel image binary into an mmap'd space for easy of acess, checking that the binary is in the proper format.
 * @param kernimg 
 * @return The pointer to the mmapd' binary
 */
static void* loadKernel(char* kernimg) {
	int fd = open(kernimg, O_RDONLY);
	if (fd < 0) handleError(ERR_IO, FATAL, "Could not open kernel image %s!\n", kernimg);

	struct stat statBuffer;
	int rc = fstat(fd, &statBuffer);
	if (rc != 0) handleError(ERR_IO, FATAL, "Could not stat file descriptor!\n");

	void* ptr = mmap(0, statBuffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) handleError(ERR_INNER, FATAL, "Could not map file!\n");
	close(fd);

	AOEFFheader* header = (AOEFFheader*) ptr;
	
	// Check it is an AOEFF and it is type kernel
	if (header->hID[AHID_0] != AH_ID0 && header->hID[AHID_1] != AH_ID1 && 
			header->hID[AHID_2] != AH_ID2 && header->hID[AH_ID3] != AH_ID3) handleError(ERR_INVALID_FORMAT, FATAL, "File is not an AOEFF!\n");

	if (header->hType != AHT_KERN) handleError(ERR_INVALID_FORMAT, FATAL, "File is not a kernel image!\n");

	return ptr;
}

static void* createMemory() {
	int fd = shm_open(SHMEM_MEM, O_CREAT | O_RDWR, 0666);
	if (fd == -1) handleError(ERR_SHAREDMEM, FATAL, "Could not open shared memory for emulated memory!\n");

	int r = ftruncate(fd, MEMORY_SPACE_SIZE);
	if (r == -1) handleError(ERR_INNER, FATAL, "Could not ftruncate!\n");

	void* emMem = mmap(NULL, MEMORY_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (emMem == MAP_FAILED) handleError(ERR_MEM, FATAL, "Could not mmap emulated memory!\n");

	close(fd);
	return emMem;
}

static signal_t* createSignalMemory() {
	int fd = shm_open(SHMEM_SIG, O_CREAT | O_RDWR, 0666);
	if (fd == -1) handleError(ERR_SHAREDMEM, FATAL, "Could not open shared memory for signal!\n");

	int r = ftruncate(fd, SIG_SIZE*4);
	if (r == -1) handleError(ERR_INNER, FATAL, "Could not ftruncate!\n");

	void* sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (sigMem == MAP_FAILED) handleError(ERR_MEM, FATAL, "Could not mmap signal memory!\n");

	close(fd);
	return (signal_t*) sigMem;
}

static void setupKernel(void* memory, void* kernimg, signal_t* sigMem) {
	AOEFFheader* header = (AOEFFheader*) kernimg;
	uint32_t kernEntry = header->hEntry;

	// NEED TO WORK ON KERNEL LAYOUT
	// .....

	// Even though this is the emulator, exec signal is only available for shell-cpu
	signal_t* shellCPUSignal = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	execprog_md metadata = {
		.entry = kernEntry
	};
	int ret = setExecSignal(shellCPUSignal, &metadata);
	if (ret == -1) handleError(ERR_SIGNAL, FATAL, "No access for exec signal!\n");
}

static void openShell() {

}

int main(int argc, char const* argv[]) {
	char* kernimgFilename = NULL;
	char* cpuimg = NULL;
	char* shell = NULL;

	// Get options
	struct option longOpts[] = {
		{"cpu", optional_argument, 0, 0},
		{"shell", optional_argument, 0, 0},
		{0, 0, 0, 0}
	};

	int opt;
	int optIdx = 0;
	while ((opt = getopt_long(argc, argv, ":", longOpts, &optIdx)) != -1) {
		switch (opt)	{
			case 0:
				if (strcmp("cpu", longOpts[optIdx].name) == 0) cpuimg = optarg;
				else if (strcmp("shell", longOpts[optIdx].name) == 0) shell = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s inputfile [--cpu cpuimg] [--shell shell]", argv[0]);
				break;
		}
	}

	kernimgFilename = (char*) argv[optind];

	if (!kernimgFilename) {
		fprintf(stderr, "No kernel image!\n");
		exit(-1);
	}
	
	char* ext = strstr(kernimgFilename, ".ark");
	if (!ext) handleError(ERR_IO, FATAL, "Kernel image does not end in '.ark'!\n");

	debug("Kernel file image is %s\n", kernimgFilename);

	printf("Creating environment...\n");
	// Create necessary environment
	void* kernimg = loadKernel(kernimgFilename);
	void* emulatedMemory = createMemory();
	signal_t* signalsMemory = createSignalMemory();
	setupSignals(signalsMemory);

	// Spawn shell
	openShell();
	// Spawn CPU
	runCPU();


	printf("RUEMU: Setting kernel...\n");
	setupKernel(emulatedMemory, kernimg, signalsMemory);

	// munmap(kernimg, )
	munmap(emulatedMemory, MEMORY_SPACE_SIZE);
	munmap(signalsMemory, SIG_SIZE*4);
	
	return 0;
}