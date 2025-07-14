#include <stdlib.h>
#include <string.h>

#include "sigHeap.h"


/**
 * Heap Block structure
 * |----4 bytes---|
 * |    CANARY 		|
 * |		 size			|
 * |		flags			|
 * | CANARY|NEXT	|
 * |--------------|
 * | 		PAYLOAD 	|
 * | 		PAYLOAD 	|
 * |--------------|
 * 
 * `size` includes the number of bytes that they payload has
 * it does not take into account the metadata
 * `flags`: [b31, ..., b3, b2, b1, b0]
 * 		As of now, flags only contain whether the block has been allocated, signified in b0
 * 		1 if allocated, 0 if free
 * For allocated blocks, `CANARY|NEXT` will contain a canary value to test for overwritten
 * For free blocks, it contains a pointer to the next free block
 */

typedef struct HeapBlock {
	void* canary0;
	sigsize_t size; // Size of the data block, excluding metadata
	uint32_t flags;
	union {
		void* canary1;
		void* next;
	} addr;
} hblock_t;

#define METADATA_SIZE sizeof(hblock_t)
#define CANARY 0xDAEFFAED
#define _PAGESIZE 4096

#define ALIGNMENT 8
#define ALIGN(x) (((x) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#define PTR(x) ((char*)(x))

static void* signalHeap;
static hblock_t START;


static void setupBlock(hblock_t* block, sigsize_t size, bool alloc) {
	block->canary0 = PTR(CANARY);
	block->size = size;
	block->flags = alloc;

	if (alloc) block->addr.canary1 = PTR(CANARY);
	else block->addr.next = NULL;
}

/**
 * Finds a free block with a minimum size provided.
 * @param size 
 * @return 
 */
static hblock_t* findBlock(sigsize_t size) {
	hblock_t* currBlock = (&START)->addr.next;
	hblock_t* currBest = NULL;

	sigsize_t currDiff = 0, currMinDiff = UINT32_MAX;

	while (currBlock) {
		// Size difference between the target and actual data size
		// Need to minimize difference
		currDiff = currBlock->size - size;

		if (currBlock->size >= size && currDiff <= currMinDiff) {
			currMinDiff = currDiff;
			currBest = currBlock;
		}

		currBlock = currBest->addr.next;
	}

	return currBest;
}

static hblock_t* truncate(hblock_t* block, sigsize_t size) {
	hblock_t* temp = block;
	temp++; // Make temp point to the start of payload data

	char* _tempc = (char*) temp;
	_tempc += size; // Create requested memory
	temp = (hblock_t*) _tempc;
	// temp now at the end of requested memory, possibly at start of new free

	// Get the remaining size of the new free block, includes space for metadata
	sigsize_t newFreeBlockSize = block->size - size;


	// Get the previous block for linking purposes
	hblock_t* prev = (&START);
	while (prev->addr.next != block) prev = prev->addr.next;

	// If block to allocate takes up everything, cannot split
	// Else, place a new free block
	if (newFreeBlockSize > (METADATA_SIZE+ALIGNMENT)) {
		// Space for a free block
		// That being, space for free block meaning sufficient for metadata
		//   and minimnum 8 bytes

		setupBlock(temp, newFreeBlockSize-METADATA_SIZE, false);

		prev->addr.next = temp;
		temp->addr.next = block->addr.next;
	} else {
 		// else cannot split, just link
		// Only link previous to next, aka removing from free list
		prev->addr.next = block->addr.next;
	}

	setupBlock(block, size, true);

	return block;
}


void sinit(void* _sigHeapPtr, bool new) {
	signalHeap = _sigHeapPtr;

	// Set up the global starter block
	// No data in this so size is the metadata size itself
	setupBlock(&START, METADATA_SIZE, false);
	if (new) {
		// Set up the actual heap
		setupBlock((hblock_t*) signalHeap, _PAGESIZE - METADATA_SIZE, false);
	}
	(&START)->addr.next = signalHeap;
}

void* smalloc(sigsize_t size) {
	if (size == 0) return NULL;

	// All payload data is to be in multiples of 8
	size = ALIGN(size);

	// Get a block with sufficient size
	hblock_t* block = findBlock(size);

	// Have the acquired block be reduced to the necessary limit
	// And create a new free block
	// Note that this is important on the first allocation as there is only one block
	//  and it would not be good to give the user the entire heap :(
	block = truncate(block, size);

	// Return the start of the actual memory block
	void* retblock = (void*)(block + 1);

	return retblock;
}

void sfree(void* ptr) {
	if (!ptr) return;

	hblock_t* blockToFree = ((hblock_t*)ptr) - 1;

	// Check double free
	if (blockToFree->flags == 0b0) return;

	// Check overwritten metadata
	if (blockToFree->canary0 != PTR(CANARY) && blockToFree->addr.canary1 != PTR(CANARY)) return;

	// TODO: Better error indicating for memory freeing

	// Re-link, aka place it back in free list
	hblock_t* curr = (&START)->addr.next;
	hblock_t* temp = NULL;

	// Iterate until next block of current is past the block to free
	while (curr->addr.next && (((hblock_t*)curr->addr.next) < blockToFree)) curr = curr->addr.next;

	temp = curr->addr.next;

	curr->addr.next = blockToFree;
	blockToFree->addr.next = temp;

	blockToFree->flags = 0b0;

	// merge(blockToFree);
}


uint32_t ptrToOffset(void* ptr, bool* valid) {
	// Make sure the pointer is within bounds
	if (ptr <= signalHeap || ptr >= (signalHeap+_PAGESIZE)) *valid = false;

	// Offset is from the start of the heap to where the pointer is at
	uint32_t offset = ((char*) ptr) - ((char*) signalHeap);

	return offset;
}

void* offsetToPtr(uint32_t offset) {

	char* ptr = ((char*) signalHeap) + offset;

	return (void*) ptr;
}


char* sstrdup(const char* src) {
	size_t len = strlen(src) + 1;
	char* dest = smalloc(len);
	if (!dest) return NULL;
	
	strcpy(dest, src);

	return dest;
}