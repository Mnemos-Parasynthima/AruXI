#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "assemblerDiagnostics.h"


static char buffer[150];

static char* errnames[ERR_MISALIGNMENT+1] = {
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
	"NO VECTOR TABLE ERROR",
	"NOT A VALID DIRECTIVE ERROR",
	"DIRECTIVE NOT ALLOWED ERROR",
	"NOT A VALID CONDITION ERROR",
	"INSTRUCTION NOT IN TEXT SECTION ERROR",
	"MISALIGNED ADDRESS ERROR"
};

static char* warnnames[WARN+1] = {
	"WARNING"
};

static void formatMessage(const char* fmsg, va_list args) {
	vsnprintf(buffer, 150, fmsg, args);
}

void emitError(errType err, linedata_ctx* linedata, const char* fmsg, ...) {
	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	if (linedata) fprintf(stderr, RED "[%s] at `%s` (%d): %s" RESET, errnames[err], linedata->source, linedata->linenum, buffer);
	else fprintf(stderr, RED "[%s]: %s" RESET, errnames[err], buffer);
	exit(-1);
}

void emitWarning(warnType warn, linedata_ctx* linedata, const char* fmsg, ...) {
	// TODO: Allow filtering of warning types

	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	if (linedata) fprintf(stderr, YELLOW "[%s] at `%s` (%d): %s", RESET, warnnames[warn], linedata->source, linedata->linenum, buffer);
	else fprintf(stderr, YELLOW "[%s]: %s" RESET, warnnames[warn], buffer);
}

void debug(debugLvl lvl, const char* fmsg, ...) {
#ifdef DEBUG
	va_list args;
	va_start(args, fmsg);

	formatMessage(fmsg, args);

	char* color;
	switch (lvl)	{
		case DEBUG_BASIC: color = CYAN; break;
		case DEBUG_DETAIL: color = BLUE; break;
		case DEBUG_TRACE: color = PURPLE; break;
		default: color = CYAN; break;
	}

	fprintf(stderr, "%s%s" RESET, color, buffer);
#endif
}