#ifndef _IPC_H_
#define _IPC_h_


#include <stdbool.h>

#define SHMEM_SIG "sig.mem"
#define SHMEM_MEM "adr.mem"
#define PAGESIZE 4096
#define MEMORY_SPACE_SIZE (4L * 1024 * 1024 * 1024) // 4GB
// #define SIG_SPACE_SIZE


void* attachSharedMemory(char* name, int size);

bool detachSharedMemory(void* memory);

void destorySharedMemory(char* name);

#endif