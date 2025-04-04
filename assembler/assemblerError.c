#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "assemblerError.h"

static char buffer[150];

static char* errnames[ERR_INVALID_INSTRUCTION+1] = {
	"MEMORY ERROR",
	"UNDEFINED SYMBOL ERROR",
	"INVALID SYNTAX ERROR",
	"NO ENTRYPOINT ERROR",
	"ENTRYPOINT NOT GLOBAL ERROR",
	"NO TEXT SECTION ERROR",
	"NOT A VALID DIRECTIVE ERROR",
	"INSTRUCTION NOT IN TEXT SECTION ERROR",
	"INVALID INSTRUCTION ERROR"
};

static void formatMessage(const char* fmsg, va_list args) {
	vsnprintf(buffer, 150, fmsg, args);
}

void handleError(errType err, sevType sev, const char* fmsg, ...) {
	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	if (sev == FATAL) {
		fprintf(stdout, RED "%s: %s" RESET, errnames[err], buffer);
		exit(-1);
	}
}