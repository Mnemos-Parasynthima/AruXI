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
	uint32_t hRelDirOff; // offset of the relocation tables directory
	uint32_t hRelDirSize; // number of relocation tables entries (how many reloc tables)
} AOEFFheader;

#define AH_ID0 0xAE
#define AH_ID1 'A'
#define AH_ID2 'E'
#define AH_ID3 'F'

#define AHID_0 0
#define AHID_1 1
#define AHID_2 2
#define AHID_3 3

#define AHT_EXEC 0 // Executable
#define AHT_KERN 1 // Kernel
#define AHT_DLIB 3 // Dynamic library
#define AHT_AOBJ 4 // Object file
#define AHT_SLIB 5 // Static library


typedef struct AOEFFSctHdr {
	char shSectName[8]; // name of the section
	uint32_t shSectOff; // offset of the section
	uint32_t shSectSize; // size of the section
	uint32_t shSectRel; // index of the relocation table tied to this section
} AOEFFSectHeader;


typedef struct AOEFFSymEnt {
	uint32_t seSymbName; // index of the symbol name
	uint32_t seSymbSize; // size of the data that the symbol is referring to
	uint32_t seSymbVal; // value of the symbol
	uint8_t seSymbInfo; // symbol information ([symbol type, symbol locality])
	uint32_t seSymbSect; // section index the symbol is in, undefined if external
} AOEFFSymbEntry;

#define SE_SECT_UNDEF 0xFFFFFFFF // For external symbols

#define SE_GET_TYPE(i) (i >> 4)
#define SE_GET_LOC(i) (i & 0xf)
#define SE_SET_INFO(t,l) ((t << 4) | (l & 0xf))

#define SE_NONE_T 0
#define SE_ABSV_T 1
#define SE_FUNC_T 2
#define SE_OBJ_T 3

#define SE_LOCAL 0
#define SE_GLOBL 1


typedef struct AOEFFStrTab {
	char* stStrs;
} AOEFFStringTab;


typedef struct AOEFFRelEnt {
	uint32_t reOff; // offset from the start of the section
	uint32_t reSymb; // index of the symbol in symbol table
	uint8_t reType; // type of relocation (RE_ARU32_*)
} AOEFFRelEntry;

typedef struct AOEFFRelTab {
	uint8_t relSect; // which section this relocation table is for
	uint32_t relTabName; // index of relocation table name
	AOEFFRelEntry** relEntries;
	uint32_t relCount; // number of relocation entries
} AOEFFRelTable;

typedef struct AOEFFRelTabDir {
	AOEFFRelTable* reldTables;
	uint8_t reldCount; // number of relocation tables
} AOEFFRelTableDir;

#define RE_ARU32_ABS14 0
#define RE_ARU32_MEM9 1
#define RE_ARU32_IR24 2
#define RE_ARU32_IR19 3


typedef enum AOEF_BIN_FILETYPE {
	AOEF_FT_AOBJ,
	AOEF_FT_EXEC,
	AOEF_FT_DLIB,
	AOEF_FT_SLIB,
	AOEF_FT_KERN
} AOEFbin_ft;

typedef struct AOEF_BIN {
	AOEFbin_ft binarytype;
	AOEFFheader header;
	AOEFFSectHeader* sectHdrTable;
	AOEFFSymbEntry* symbEntTable;
	AOEFFStringTab stringTable;
	AOEFFRelTableDir reltabDir;

	// void* manifest;

	uint8_t* _data;
	uint8_t* _const;
	uint32_t* _text;
	uint8_t* _evt;
	uint8_t* _ivt;
} AOEFbin;

#endif