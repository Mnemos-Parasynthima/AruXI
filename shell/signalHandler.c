#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "signalHandler.h"

void handleInterrupt() {
	write(1, "Interrupted\n", 13);
	exit(1);
}

handler_t *redefineSignal(int signum, handler_t *handler)
{
	struct sigaction action, prevAction;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART;

	sigaction(signum, &action, &prevAction);

	return prevAction.sa_handler;
}