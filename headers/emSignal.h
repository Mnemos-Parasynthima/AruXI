#ifndef _IPC_SIGNAL_H_
#define _IPC_SIGNAL_H_

#include <stdint.h>
#include <sys/types.h>


#define emSIG_SHUTDOWN 0x80000000
#define emSIG_FAULT 0x40000000
#define emSIG_READY 0x20000000
#define emSIG_CPU_SAVED 0x2000
#define emSIG_CPU_SAVE 0x1000
#define emSIG_LOAD 0x80
#define emSIG_SAVE 0x40
#define emSIG_ERROR 0x8
#define emSIG_EXIT 0x4
#define emSIG_KILL 0x2
#define emSIG_EXEC 0x1

#define emSIG_SHUTDOWN_IDX 31
#define emSIG_FAULT_IDX 30
#define emSIG_READY_IDX 29
#define emSIG_CPU_SAVED_IDX 13
#define emSIG_CPU_SAVE_IDX 12
#define emSIG_LOAD_IDX 7
#define emSIG_SAVE_IDX 6
#define emSIG_ERROR_IDX 3
#define emSIG_EXIT_IDX 2
#define emSIG_KILL_IDX 1
#define emSIG_EXEC_IDX 0

#define SIG_GET(bits, i) ((bits>>i) & 0x1)
#define SIG_SET(bits, i) (bits | (1<<i))

#define UNIVERSAL_SIG 0
#define EMU_SHELL_SIG 1
#define EMU_CPU_SIG 2
#define SHELL_CPU_SIG 3

#define GET_SIGNAL(sigs, sigType) (&sigs[sigType])

typedef struct LoadprogMetadata {
	char* program;
	char** argv;
} loadprog_md;

typedef struct ExecprogMetadata {
	uint32_t entry;
} execprog_md;

typedef struct SyscallreqMetadata {

} syscallReq_md;

typedef struct Signal {
	uint32_t interrupts;
	uint32_t intEnable;
	uint32_t ackMask;
	uint32_t payloadValid;
	union {
		loadprog_md loadprog;
		execprog_md execprog;
		syscallReq_md syscalReq;
	} metadata;
} signal_t;

typedef struct SignalMetadata {
	pid_t emulatorPID;
	pid_t shellPID;
	pid_t cpuPID;
	uint8_t signalType;
} signal_md;

typedef struct SignalMemory {
	signal_md metadata;
	signal_t signals[4];
} SigMem;

#define SIG_MEM_SIZE sizeof(SigMem)


void setupSignals(SigMem* signalMemory);

// Universal signals
int setShutdownSignal(signal_t* signal);
int ackShutdownSignal(signal_t* signal);

int setFaultSignal(signal_t* signal);
int ackFaultSignal(signal_t* signal);

int setReadySignal(signal_t* signal);
int ackReadySignal(signal_t* signal);

// CPU-Emulator signals


// Shell-Emulator signals
int setLoadSignal(signal_t* signal, loadprog_md* metadata);
int ackLoadSignal(signal_t* signal);

// CPU-Shell signals
int setErrorSignal(signal_t* signal);
int ackErrorSignal(signal_t* signal);

int setExitSignal(signal_t* signal);
int ackExitSignal(signal_t* signal);

int setKillSignal(signal_t* signal);
int ackKillSignal(signal_t* signal);

int setExecSignal(signal_t* signal, execprog_md* metadata);
int ackExecSignal(signal_t* signal);

#endif