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
#include "aoef.h"
#include "emulatorError.h"
#include "ipc.h"


#define KERN_START 0xA0080000 
#define KERN_DATA 0xA0080000
#define KERN_TEXT 0xB8080000
#define KERN_HEAP 0xD0080000
#define KERN_STACK 0xF0080000

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

static void setupKernel(uint8_t* memory, uint8_t* kernimg, signal_t* sigMem) {
	AOEFFheader* header = (AOEFFheader*) kernimg;
	uint32_t kernEntry = header->hEntry;

	AOEFFSectHeader* sectHdrs = kernimg + header->hSectOff;
	uint32_t sectHdrsSize = header->hSectSize;

	// Set the data and text information
	for (uint32_t i = 0; i < sectHdrsSize; i++) {
		AOEFFSectHeader* sectHdr = &(sectHdrs[i]);

		if (strncmp(".data", sectHdr->shSectName, 8) == 0) {
			uint8_t* dataStart = memory + KERN_DATA; // beginning of emulated memory kernel data
			uint8_t* kernimgData = kernimg + sectHdr->shSectOff; // beginning of binary image kernel data section

			memcpy(dataStart, kernimgData, sectHdr->shSectSize);
		} else if (strncmp(".text", sectHdr->shSectName, 8) == 0) {
			uint8_t* textStart = memory + KERN_TEXT;
			uint8_t* kernimgText = kernimg + sectHdr->shSectOff;

			debug("Start of text section in kernel image: %p::Start of kernel text section in emulated memory:%p\n", kernimgText, textStart);

			memcpy(textStart, kernimgText, sectHdr->shSectSize);
		}
	}

	// Even though this is the emulator, exec signal is only available for shell-cpu
	signal_t* shellCPUSignal = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	execprog_md metadata = {
		.entry = kernEntry
	};
	int ret = setExecSignal(shellCPUSignal, &metadata);
	if (ret == -1) handleError(ERR_SIGNAL, FATAL, "No access for exec signal!\n");
}

static void redirectOut(const char* filename) {
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) handleError(ERR_IO, FATAL, "Could not open logfile!\n");

	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
	close(fd);
}

static pid_t openShell(char* shellExe, bool log) {
	pid_t pid = fork();
	if (pid == -1) {
		return -1;
	} else if (pid == 0) {
		char* args[] = {shellExe, NULL};
		execv(shellExe, args);
		perror("fail to exec shell");
		exit(1);
	}

	return pid;
}

static pid_t runCPU(char* cpuExe, bool log) {
	pid_t pid = fork();
	if (pid == -1) {
		return -1;
	} else if (pid == 0) {
		redirectOut("cpu.log");
		char* args[] = {cpuExe, NULL};
		execv(cpuExe, args);
		perror("fail to exec cpu");
		exit(1);
	}

	return pid;
}

int main(int argc, char const* argv[]) {
	char* kernimgFilename = NULL;
	char* cpuimg = "iaru0";
	char* shell = "ash";
	bool log = false;

	// Get options
	struct option longOpts[] = {
		{"cpu", optional_argument, 0, 0},
		{"shell", optional_argument, 0, 0},
		{0, 0, 0, 0}
	};

	int opt;
	int optIdx = 0;
	while ((opt = getopt_long(argc, argv, ":l", longOpts, &optIdx)) != -1) {
		switch (opt)	{
			case 'l':
				log = true;
				break;
			case 0:
				if (strcmp("cpu", longOpts[optIdx].name) == 0) cpuimg = optarg;
				else if (strcmp("shell", longOpts[optIdx].name) == 0) shell = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s inputfile [--cpu cpuimg] [--shell shell] [-l]", argv[0]);
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

	// Spawn CPU
	pid_t CPUPID = runCPU(cpuimg, log);
	// Spawn shell
	pid_t shellPID = openShell(shell, log);
	// Shell now takes control of the main stdout/err
	// Emulator is to have its own outstream
	redirectOut("ruemu.log");


	fprintf(stdout, "RUEMU: Setting kernel...\n");
	debug("Start of emulated memory: %p\n", emulatedMemory);
	setupKernel(emulatedMemory, kernimg, signalsMemory);


	int set = setReadySignal(GET_SIGNAL(signalsMemory, UNIVERSAL_SIG));
	if (set == -1) handleError(ERR_SIGNAL, FATAL, "No access for ready signal!\n");
	if (set == 0) handleError(ERR_SIGNAL, FATAL, "Could not set ready signal!\n");


	
	int status;
	waitpid(shellPID, &status, 0);
	waitpid(CPUPID, &status, 0);

	// munmap(kernimg, )
	munmap(emulatedMemory, MEMORY_SPACE_SIZE);
	munmap(signalsMemory, SIG_SIZE*4);
	
	return 0;
}