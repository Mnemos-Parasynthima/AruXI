#ifndef _AOEF_H_
#define _AOEF_H_

#include <stdint.h>


typedef struct AOEFFHdr {
	uint8_t hID[4];
	uint32_t hType;
	uint32_t hEntry;
	uint32_t hSectOff; // offset of the section header table
	uint32_t hSectSize; // number of section header entries
	uint32_t hSymbOff; // offset of the symbol table
	uint32_t hSymbSize; // number of symbol entries
	uint32_t hStrTabOff; // offset of the string table
	uint32_t hStrTabSize; // size (in bytes) of the string table
} AOEFFheader;

#define AH_ID0 0xAE
#define AH_ID1 'A'
#define AH_ID2 'E'
#define AH_ID3 'F'

#define AHID_0 0
#define AHID_1 1
#define AHID_2 2
#define AHID_3 3

#define AHT_EXEC 0
#define AHT_KERN 1
#define AHT_SLIB 3


typedef struct AOEFFSctHdr {
	char shSectName[8]; // name of the section
	uint32_t shSectOff; // offset of the section
	uint32_t shSectSize; // size of the section
} AOEFFSectHeader;


typedef struct AOEFFSymEnt {
	uint32_t seSymbName; // index of the symbol name
	uint32_t seSymbVal; // value of the symbol
	uint8_t seSymbInfo; // symbol information
	uint32_t seSymbSect; // section index the symbol is in
} AOEFFSymbEntry;

#define SE_TYPE(i) (i >> 4)
#define SE_LOC(i) (i & 0xf)
#define SE_INFO(t,l) ((t << 4) | (l & 0xf))

#define SET_NONE 0
#define SET_ABSV 1
#define SET_ADDR 2

#define SE_LOCAL 0
#define SE_GLOBL 1


typedef struct AOEFFStrTab {
	char* stStrs;
} AOEFFStringTab;



typedef struct AOEF_BIN {
	AOEFFheader header;
	AOEFFSectHeader* sectHdrTable;
	AOEFFSymbEntry* symbEntTable;
	AOEFFStringTab stringTable;
	uint8_t* _data;
	uint8_t* _const;
	uint32_t* _text;
} AOEFbin;


#endif