#ifndef _ASMBLR_ERROR_H
#define _ASMBLR_ERROR_H

#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define PURPLE "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m" 

typedef enum {
	WARN,
	ERR_INNER,
	ERR_MEM,
	ERR_IO,
	ERR_ENTRY_NOT_GLOBAL,
	ERR_UNDEFINED_SYMBOL,
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
	FATAL,
	WARNING
} sevType;

void handleError(errType err, sevType sev, const char* fmsg, ...);

/** **/

void debug(const char* fmsg, ...);

#endif