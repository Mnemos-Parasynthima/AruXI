#ifndef _SIGNAL_HEAP_H_
#define _SIGNAL_HEAP_H_

typedef unsigned int sigsize_t;

void sinit(void* _sigHeapPtr);

void* smalloc(sigsize_t size);

void sfree(void* ptr);

#endif