#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "assemblerError.h"

static char buffer[150];

static char* errnames[ERR_MISALIGNMENT+1] = {
	"WARNING",
	"INTERNAL ERROR",
	"MEMORY ERROR",
	"IO ERROR",
	"ENTRYPOINT NOT GLOBAL ERROR",
	"UNDEFINED SYMBOL ERROR",
	"REDEFINED SYMBOL ERROR",
	"INVALID SYNTAX ERROR",
	"INVALID EXPRESSION ERROR",
	"INVALID LABEL ERROR",
	"INVALID REGISTER ERROR",
	"INVALID INSTRUCTION ERROR",
	"INVALID NUMBER SIZE ERROR",
	"NO ENTRYPOINT ERROR",
	"NO TEXT SECTION ERROR",
	"NOT A VALID DIRECTIVE ERROR",
	"DIRECTIVE NOT ALLOWED ERROR",
	"NOT A VALID CONDITION ERROR",
	"INSTRUCTION NOT IN TEXT SECTION ERROR",
	"MISALIGNED ADDRESS ERROR"
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