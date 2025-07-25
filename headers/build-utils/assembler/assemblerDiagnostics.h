#ifndef _ASMBLR_DIAGNOSTICS_H
#define _ASMBLR_DIAGNOSTICS_H

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define PURPLE "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"

typedef enum {
	ERR_INTERNAL,
	ERR_MEM,
	ERR_IO,
	ERR_ENTRY_NOT_GLOBAL,
	ERR_UNDEFINED,
	ERR_REDEFINED,
	ERR_INVALID_SYNTAX,
	ERR_INVALID_EXPRESSION,
	ERR_INVALID_LABEL,
	ERR_INVALID_REGISTER,
	ERR_INVALID_INSTRUCTION,
	ERR_INVALID_SIZE,
	ERR_NO_ENTRY,
	ERR_NO_TEXT,
	ERR_NO_VT,
	ERR_DIRECTIVE_NOT_FOUND,
	ERR_DIRECTIVE_NOT_ALLOWED,
	ERR_CONDITION_NOT_FOUND,
	ERR_INSTR_NOT_IN_TEXT,
	ERR_MISALIGNMENT
} errType;

typedef enum {
	WARN
} warnType;

typedef struct LineData {
	char* source;
	int linenum;
} linedata_ctx;


void emitError(errType err, linedata_ctx* linedata, const char* fmsg, ...);
void emitWarning(warnType warn, linedata_ctx* linedata, const char* fmsg, ...);

typedef enum {
	DEBUG_BASIC,
	DEBUG_DETAIL,
	DEBUG_TRACE
} debugLvl;

void debug(debugLvl lvl, const char* fmsg, ...);

#endif