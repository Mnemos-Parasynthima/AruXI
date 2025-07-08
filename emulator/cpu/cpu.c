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

#include "signalHandler.h"
#include "emSignal.h"
#include "emSignalHandler.h"
#include "shmem.h"
#define D_COMP "CPU"
#include "diagnostics.h"
#include "core.h"
#include "mem.h"

core_t core;
uint8_t* emMem;
SigMem* sigMem;
opcode_t imap[1<<8];

char* istrmap[] = {
	"OP_NOP",
	"OP_MUL",
	"OP_SMUL",
	"OP_DIV",
	"OP_SDIV",
	"OP_ADD",
	"OP_ADDS",
	"OP_SUB",
	"OP_SUBS",
	"OP_OR",
	"OP_AND",
	"OP_XOR",
	"OP_NOT",
	"OP_LSL",
	"OP_LSR",
	"OP_ASR",
	"OP_CMP",
	"OP_MV",
	"OP_MVN",
	"OP_SXB",
	"OP_SXH",
	"OP_UXB",
	"OP_UXH",
	"OP_LD",
	"OP_LDB",
	"OP_LDBS",
	"OP_LDBZ",
	"OP_LDH",
	"OP_LDHS",
	"OP_LDHZ",
	"OP_STR",
	"OP_STRB",
	"OP_STRH",
	"OP_UB",
	"OP_UBR",
	"OP_B",
	"OP_CALL",
	"OP_RET",
	"OP_ADDF",
	"OP_SUBF",
	"OP_MULF",
	"OP_DIVF",
	"OP_LDF",
	"OP_STRF",
	"OP_MVF",
	"OP_HLT",
	"OP_SI",
	"OP_DI",
	"OP_IRET",
	"OP_ERET",
	"OP_SYSCALL"
};

void handleSIGUSR1(int signum) {
	// Check metadata
	uint8_t sigType = sigMem->metadata.signalType;
	signal_t* sig = GET_SIGNAL(sigMem->signals, sigType);

	uint32_t ints = sig->interrupts;
	int i = 0;

	while (i <= 31) {
		// Loop through each interrupt bit, detecting which one is set, execute that one
		// Note that this only works if only one interrupt is set at a time
		uint8_t bit = (ints >> i) & 0b1;

		if (bit == 0b1) {
			switch (i) {
				case emSIG_FAULT_IDX:
					write(STDERR_FILENO, "CPU detected SIG_FAULT\n", 23);
					munmap(sigMem, SIG_MEM_SIZE);
					munmap(emMem, MEMORY_SPACE_SIZE);
					exit(-1);
					break;
				default:
					break;
			}
		}

		i++;
	}
}

void initIMap() {
	for (int i = 0; i < (1<<8); i++) imap[i] = OP_ERROR;

	imap[0b10000000] = OP_ADD;
	imap[0b10000001] = OP_ADD;
	imap[0b10001000] = OP_ADDS;
	imap[0b10001001] = OP_ADDS;
	imap[0b10010000] = OP_SUB;
	imap[0b10010001] = OP_SUB;
	imap[0b10011000] = OP_SUBS;
	imap[0b10011001] = OP_SUBS;
	imap[0b10100000] = OP_MUL;
	imap[0b10100010] = OP_SMUL;
	imap[0b10101000] = OP_DIV;
	imap[0b10101010] = OP_SDIV;
	imap[0b01000000] = OP_OR;
	imap[0b01000001] = OP_OR;
	imap[0b01000010] = OP_AND;
	imap[0b01000011] = OP_AND;
	imap[0b01000100] = OP_XOR;
	imap[0b01000101] = OP_XOR;
	imap[0b01000110] = OP_NOT;
	imap[0b01000111] = OP_NOT;
	imap[0b01001000] = OP_LSL;
	imap[0b01001001] = OP_LSL;
	imap[0b01001010] = OP_LSR;
	imap[0b01001011] = OP_LSR;
	imap[0b01001100] = OP_ASR;
	imap[0b01001101] = OP_ASR;
	// imap[0b10011000] = OP_CMP; // cmpi aliased as subsi
	// imap[0b10011001] = OP_CMP; // cmpr aliased as subsr
	// imap[0b10000000] = OP_MV; // mvi aliased as addi
	// imap[0b10000001] = OP_MV; // mvr aliased as orr
	// imap[0b10010000] = OP_MVN; // mvni aliased as subi
	// imap[0b10010001] = OP_MVN; // mvnr aliased as subr
	// imap[0b00000000] = OP_SXB;
	// imap[0b00000000] = OP_SXH;
	// imap[0b00000000] = OP_UXB;
	// imap[0b00000000] = OP_UXH;
	imap[0b00010100] = OP_LD;
	imap[0b00110100] = OP_LDB;
	imap[0b01010100] = OP_LDBS;
	imap[0b01110100] = OP_LDBZ;
	imap[0b10010100] = OP_LDH;
	imap[0b10110100] = OP_LDHS;
	imap[0b11010100] = OP_LDHZ;
	imap[0b00011100] = OP_STR;
	imap[0b00111100] = OP_STRB;
	imap[0b01011100] = OP_STRH;
	imap[0b11000000] = OP_UB;
	imap[0b11000010] = OP_UBR;
	imap[0b11000100] = OP_B;
	imap[0b11000110] = OP_CALL;
	imap[0b11001000] = OP_RET;
	// imap[0b10000000] = OP_NOP; // aliased as addi
	// imap[0b00000000] = OP_SYSCALL;
	imap[0b10111100] = OP_HLT;
	// imap[0b00000000] = OP_SI;
	// imap[0b00000000] = OP_DI;
	// imap[0b00000000] = OP_IRET;
	// imap[0b00000000] = OP_ADDF;
	// imap[0b00000000] = OP_SUBF;
	// imap[0b00000000] = OP_MULF;
	// imap[0b00000000] = OP_DIVF;
	// imap[0b00000000] = OP_LDF;
	// imap[0b00000000] = OP_STRF;
	// imap[0b00000000] = OP_MVF;
}


int main(int argc, char const* argv[]) {
	initDiagnostics(stdout, "cpu.debug");

	dLog(D_NONE, DSEV_INFO, "Setting up...");

	int fd = shm_open(SHMEM_SIG, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for signal!");

	void* _sigMem = mmap(NULL, SIG_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_sigMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap signal memory!");

	close(fd);

	fd = shm_open(SHMEM_MEM, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for emulation!");

	void* _emMem = mmap(NULL, MEMORY_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_emMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap emulated memory!");

	close(fd);

	sigMem = (SigMem*) _sigMem;
	emMem = (uint8_t*) _emMem;

	redefineSignal(SIGUSR1, &handleSIGUSR1);
	initCore();
	initIMap();

	// Block until signal
	signal_t* universalSig = GET_SIGNAL(sigMem->signals, UNIVERSAL_SIG);
	dDebug(DB_DETAIL, "After get: Universal interrupts as 0x%x", universalSig->interrupts);
	dLog(D_NONE, DSEV_INFO, "Will now wait for ready...");
	uint8_t ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(universalSig->interrupts, emSIG_READY_IDX);
	dDebug(DB_DETAIL, "After get sig ready: Universal interrupts as 0x%x", universalSig->interrupts);

	// CPU has access to signal memory and emulated memory
	dLog(D_NONE, DSEV_INFO, "CPU now has access to signal memory and emulated memory.");

	// Get entry
	signal_t* shellCPUSignal = GET_SIGNAL(sigMem->signals, SHELL_CPU_SIG);
	ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(shellCPUSignal->interrupts, emSIG_EXEC_IDX);
	uint32_t entry = shellCPUSignal->metadata.execprog.entry;

	// Set up
	core.IR = entry;
	core.SP = KERN_STACK_LIMIT;

	dLog(D_NONE, DSEV_INFO, "Setting up..running kernel at 0x%x...", entry);

	runCore();

	dLog(D_NONE, DSEV_INFO, "Now in idle state, ack exec sig");

	int acked = ackExecSignal(shellCPUSignal);
	if (acked == -1) dFatal(D_ERR_SIGNAL, "No access for exec signal!");
	if (acked == 0) dFatal(D_ERR_SIGNAL, "Could not ack exec signal!");

	// Do normal run
	// ....

	dLog(D_NONE, DSEV_INFO, "Wating for shutdown...");
	dDebug(DB_DETAIL, "Before get shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(universalSig->interrupts, emSIG_SHUTDOWN_IDX);
	dDebug(DB_DETAIL, "After get shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	
	acked = ackShutdownSignal(universalSig);
	if (acked != 1) dFatal(D_ERR_SIGNAL, "Could not ack shutdown for CPU!");
	dDebug(DB_DETAIL, "After ack shutdown: Universal interrupts as 0x%x", universalSig->interrupts);
	munmap(_sigMem, SIG_MEM_SIZE);
	munmap(_emMem, MEMORY_SPACE_SIZE);

	dLog(D_NONE, DSEV_INFO, "CPU exiting");

	return 0;
}