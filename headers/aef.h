#ifndef _AEF_H_
#define _AEF_H_

#include <stdint.h>


typedef struct AEF {
	uint8_t ID[4];
	uint32_t entry;
	uint32_t constOff;
	uint32_t constSize;
	uint32_t dataOff;
	uint32_t dataSize;
	uint32_t textOff;
	uint32_t textSize;
} AEFheader;

typedef struct AEF_BIN {
	AEFheader header;
	uint8_t* constSect;
	uint8_t* dataSect;
	uint32_t* textSect;
} AEFbin;


#endif