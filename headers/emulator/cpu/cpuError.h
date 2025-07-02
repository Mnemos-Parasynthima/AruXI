#ifndef _CPU_ERROR_H
#define _CPU_ERROR_H

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
	ERR_IO,
	ERR_SIGNAL,
	ERR_SHAREDMEM,
	ERR_MEM
} errType;

typedef enum {
	FATAL,
	WARNING
} sevType;

void handleError(errType err, sevType sev, const char* fmsg, ...);

/** **/

void debug(const char* fmsg, ...);

#endif