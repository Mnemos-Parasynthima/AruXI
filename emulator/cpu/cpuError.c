#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cpuError.h"

static char buffer[150];

static char* errnames[ERR_MEM+1] = {
	"WARNING",
	"INTERNAL ERROR",
	"IO ERROR",
	"SIGNAL ERROR",
	"SHARED MEMORY ERROR",
	"MEMORY ERROR"
};

static void formatMessage(const char* fmsg, va_list args) {
	vsnprintf(buffer, 150, fmsg, args);
}

void handleError(errType err, sevType sev, const char* fmsg, ...) {
	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	if (sev == FATAL) {
		fprintf(stderr, RED "%s: %s" RESET, errnames[err], buffer);
		exit(-1);
	} else if (sev == WARNING) {
		fprintf(stderr, YELLOW "%s: %s" RESET, errnames[err], buffer);
	}
}

void debug(const char* fmsg, ...) {
#ifdef DEBUG
	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	fprintf(stderr, YELLOW "%s" RESET, buffer);
#endif
}