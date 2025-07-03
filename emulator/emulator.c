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
#include "diagnostics.h"
#include "shmem.h"


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
	if (fd < 0) dFatal(D_ERR_IO, "Could not open kernel image %s!", kernimg);

	struct stat statBuffer;
	int rc = fstat(fd, &statBuffer);
	if (rc != 0) dFatal(D_ERR_IO, "Could not stat file descriptor!");

	void* ptr = mmap(0, statBuffer.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED) dFatal(D_ERR_INTERNAL, "Could not map file!");
	close(fd);

	AOEFFheader* header = (AOEFFheader*) ptr;
	
	// Check it is an AOEFF and it is type kernel
	if (header->hID[AHID_0] != AH_ID0 && header->hID[AHID_1] != AH_ID1 && 
			header->hID[AHID_2] != AH_ID2 && header->hID[AH_ID3] != AH_ID3) dFatal(D_ERR_INVALID_FORMAT, "File is not an AOEFF!");

	if (header->hType != AHT_KERN) dFatal(D_ERR_INVALID_FORMAT, "File is not a kernel image!");

	return ptr;
}

static void* createMemory() {
	int fd = shm_open(SHMEM_MEM, O_CREAT | O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for emulated memory!");

	int r = ftruncate(fd, MEMORY_SPACE_SIZE);
	if (r == -1) dFatal(D_ERR_INTERNAL, "Could not ftruncate!");

	void* emMem = mmap(NULL, MEMORY_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (emMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap emulated memory!");

	close(fd);
	return emMem;
}

static signal_t* createSignalMemory() {
	int fd = shm_open(SHMEM_SIG, O_CREAT | O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for signal!");

	int r = ftruncate(fd, SIG_SIZE*4);
	if (r == -1) dFatal(D_ERR_INTERNAL, "Could not ftruncate!");

	void* sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (sigMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap signal memory!");

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

			dDebug(DB_DETAIL, "Start of text section in kernel image: %p::Start of kernel text section in emulated memory:%p", kernimgText, textStart);

			memcpy(textStart, kernimgText, sectHdr->shSectSize);
		}
	}

	// Even though this is the emulator, exec signal is only available for shell-cpu
	signal_t* shellCPUSignal = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	execprog_md metadata = {
		.entry = kernEntry
	};
	int ret = setExecSignal(shellCPUSignal, &metadata);
	if (ret == -1) dFatal(D_ERR_SIGNAL, "No access for exec signal!");
	dLog(D_NONE, DSEV_INFO, "Kernel has been set up. Exec signal has now been set.");
}

static void redirectOut(const char* filename) {
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) dFatal(D_ERR_IO, "Could not open logfile!");

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
	initDiagnostics(stdout, "ruemu.debug");

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
				dLog(D_NONE, DSEV_WARN, "Usage: %s inputfile [--cpu cpuimg] [--shell shell] [-l]", argv[0]);
				break;
		}
	}

	kernimgFilename = (char*) argv[optind];

	if (!kernimgFilename) dFatal(D_ERR_IO, "No kernel image!");
	
	char* ext = strstr(kernimgFilename, ".ark");
	if (!ext) dFatal(D_ERR_IO, "Kernel image does not end in '.ark'!");

	dDebug(DB_BASIC, "Kernel file image is %s", kernimgFilename);

	// Create necessary environment
	dLog(D_NONE, DSEV_INFO, "Creating environment...\n");
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
	int savedOut = dup(STDOUT_FILENO);
	int savedErr = dup(STDERR_FILENO);
	redirectOut("ruemu.log");


	dLog(D_NONE, DSEV_INFO, "Setting kernel...");
	dDebug(DB_DETAIL, "Start of emulated memory: %p", emulatedMemory);
	setupKernel(emulatedMemory, kernimg, signalsMemory);


	int set = setReadySignal(GET_SIGNAL(signalsMemory, UNIVERSAL_SIG));
	if (set == -1) dFatal(D_ERR_SIGNAL, "No access for ready signal!");
	if (set == 0) dFatal(D_ERR_SIGNAL, "Could not set ready signal!");
	dLog(D_NONE, DSEV_INFO, "Ready signal has been set!");

	
	int shellStatus, cpuStatus;
	waitpid(shellPID, &shellStatus, 0);
	waitpid(CPUPID, &cpuStatus, 0);

	// Restore it
	dup2(savedOut, STDOUT_FILENO);
	dup2(savedErr, STDERR_FILENO);
	close(savedOut);
	close(savedErr);

	// if (shellStatus == -1) fprintf(stdout, "Shell process ended")
	if (cpuStatus == -1) dLog(D_NONE, DSEV_WARN, "CPU process ended abnormally. Check the log.");

	// munmap(kernimg, )
	munmap(emulatedMemory, MEMORY_SPACE_SIZE);
	munmap(signalsMemory, SIG_SIZE*4);
	
	return 0;
}