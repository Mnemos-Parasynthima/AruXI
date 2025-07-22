#include <unistd.h>

#include "backend-os.h"


extern void* emulatedMemory;


void ruWrite(const char *buffer, uint32_t count) {
	write(STDOUT_FILENO, "Writing from ruWrite!\n", 22);
}

void ruRead(char *buffer, uint32_t count) {
}