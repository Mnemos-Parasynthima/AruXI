#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "assemblerError.h"

static char buffer[150];

static char* errnames[ERR_INVALID_INSTRUCTION+1] = {
	"MEMORY ERROR",
	"IO ERROR",
	"UNDEFINED SYMBOL ERROR",
	"REDEFINED SYMBOL ERROR",
	"INVALID SYNTAX ERROR",
	"INVALID EXPRESSION ERROR",
	"INVALID LABEL ERROR",
	"NO ENTRYPOINT ERROR",
	"ENTRYPOINT NOT GLOBAL ERROR",
	"NO TEXT SECTION ERROR",
	"NOT A VALID DIRECTIVE ERROR",
	"DIRECTIVE NOT ALLOWED ERROR",
	"INSTRUCTION NOT IN TEXT SECTION ERROR",
	"INVALID REGISTER ERROR",
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
	} else if (sev == WARNING) {
		fprintf(stdout, YELLOW "%s: %s" RESET, errnames[err], buffer);
		exit(-1);
	}
}