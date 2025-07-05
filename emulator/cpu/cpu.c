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
#define D_COMP "CPU"
#include "diagnostics.h"


int main(int argc, char const* argv[]) {
	initDiagnostics(stdout, "cpu.debug");

	dLog(D_NONE, DSEV_INFO, "Setting up...");

	int fd = shm_open(SHMEM_SIG, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for signal!");

	void* _sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_sigMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap signal memory!");

	close(fd);

	fd = shm_open(SHMEM_MEM, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for emulatedion!");

	void* _emMem = mmap(NULL, MEMORY_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_emMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap emulated memory!");

	close(fd);

	signal_t* sigMem = (signal_t*) _sigMem;
	uint8_t* emMem = (uint8_t*) _emMem;

	// Block until signal
	signal_t* universalSig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	dDebug(DB_DETAIL, "After get: Universal interrupts as 0x%x", universalSig->interrupts);
	dLog(D_NONE, DSEV_INFO, "Will now wait for ready...");
	uint8_t ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(universalSig->interrupts, emSIG_READY_IDX);
	dDebug(DB_DETAIL, "After get sig ready: Universal interrupts as 0x%x", universalSig->interrupts);

	// CPU has access to signal memory and emulated memory
	dLog(D_NONE, DSEV_INFO, "CPU!");

	// Get entry
	signal_t* shellCPUSignal = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(shellCPUSignal->interrupts, emSIG_EXEC_IDX);
	uint32_t entry = shellCPUSignal->metadata.execprog.entry;

	// Set up

	dLog(D_NONE, DSEV_INFO, "Setting up..running kernel at 0x%x...", entry);




	
	dLog(D_NONE, DSEV_INFO, "Now in idle state, ack exec sig");

	int acked = ackExecSignal(shellCPUSignal);
	if (acked == -1) dFatal(D_ERR_SIGNAL, "No access for exec signal!");
	if (acked == 0) dFatal(D_ERR_SIGNAL, "Could not ack exec signal!");

	uint32_t instr = *((uint32_t*)(emMem + entry));

	dLog(D_NONE, DSEV_INFO, "Instruction at 0x%x: 0x%x", entry, instr);

	dLog(D_NONE, DSEV_INFO, "Wating for shutdown...");
	dDebug(DB_DETAIL, "Before get shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(universalSig->interrupts, emSIG_SHUTDOWN_IDX);
	dDebug(DB_DETAIL, "After get shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	
	acked = ackShutdownSignal(universalSig);
	if (acked != 1) dFatal(D_ERR_SIGNAL, "Could not ack shutdown for CPU!");
	dDebug(DB_DETAIL, "After ack shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	munmap(_sigMem, SIG_SIZE*4);
	munmap(_emMem, MEMORY_SPACE_SIZE);


	dLog(D_NONE, DSEV_INFO, "CPU exiting");

	return 0;
}