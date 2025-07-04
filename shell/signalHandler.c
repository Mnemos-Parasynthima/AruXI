#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "signalHandler.h"
#include "emSignal.h"


extern signal_t* sigMem;


void handleInterrupt(int signum) {
	write(STDOUT_FILENO, "Interrupted", 12);
	return;
	signal_t* sig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	setShutdownSignal(sig);
}

handler_t* redefineSignal(int signum, handler_t* handler) {
	struct sigaction action, prevAction;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART;

	sigaction(signum, &action, &prevAction);

	return prevAction.sa_handler;
}