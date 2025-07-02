#include "emSignal.h"


void setupSignals(signal_t* signalMemory) {
	// At first, there's no distiniction, order can change at any time based on this implementation

	signal_t* universalSignals = &(signalMemory[UNIVERSAL_SIG]);
	signal_t* emuShellSignals = &(signalMemory[EMU_SHELL_SIG]);
	signal_t* emuCPUSignals = &(signalMemory[EMU_CPU_SIG]);
	signal_t* shellCPUSignals = &(signalMemory[SHELL_CPU_SIG]);

	universalSignals->interrupts = 0x0;
	universalSignals->intEnable = emSIG_SHUTDOWN | emSIG_FAULT | emSIG_READY;
	universalSignals->ackMask = 0x0;
	universalSignals->payloadValid = 0x0;
	
	emuShellSignals->interrupts = 0x0;
	emuShellSignals->intEnable = emSIG_SAVE;
	emuShellSignals->ackMask = 0x0;
	emuShellSignals->payloadValid = 0x0;

	emuCPUSignals->interrupts = 0x0;
	emuCPUSignals->intEnable = emSIG_CPU_SAVE | emSIG_CPU_SAVED;
	emuCPUSignals->ackMask = 0x0;
	emuCPUSignals->payloadValid = 0x0;

	shellCPUSignals->interrupts = 0x0;
	shellCPUSignals->intEnable = emSIG_ERROR | emSIG_EXIT | emSIG_KILL | emSIG_EXEC;
	shellCPUSignals->ackMask = 0x0;
	shellCPUSignals->payloadValid = 0x0;
}

int setExecSignal(signal_t* signal, execprog_md* metadata) {
	// Make sure the signal entry allows for exec
	uint8_t enable = SIG_GET(signal->intEnable, emSIG_EXEC_IDX);
	if (enable != 1) return -1;

	signal->interrupts = SIG_SET(signal->interrupts, emSIG_EXEC_IDX);
	signal->payloadValid = SIG_SET(signal->payloadValid, emSIG_EXEC_IDX);

	// Fill out metadata
	signal->metadata.execprog.entry = metadata->entry;

	return 0;
}