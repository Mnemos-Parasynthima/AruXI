#ifndef _RELOCATION_TABLE_H_
#define _RELOCATION_TABLE_H_

#include <stdint.h>


typedef struct RelocationEntry {
	uint8_t section; // The section that the entry is for
	uint32_t offset; // The offset (lp)
	char* symbol; // The symbol name associated with
	uint8_t type; // The type of relocation
} reloc_entry_t;

typedef struct RelocationTable {
	reloc_entry_t** entries;
	uint32_t size;
	uint32_t capacity;
} RelocationTable;


/**
 * 
 * @return 
 */
RelocationTable* initRelocTable();

/**
 * 
 * @param section 
 * @param offset 
 * @param symbol 
 * @param type 
 * @return 
 */
reloc_entry_t* initRelocEntry(uint8_t section, uint32_t offset, char* symbol, uint8_t type);

/**
 * 
 * @param relocTable 
 * @param relocEntry 
 */
void addRelocEntry(RelocationTable* relocTable, reloc_entry_t* relocEntry);

/**
 * 
 * @param relocTable 
 * @param symbol
 * @return 
 */
reloc_entry_t* getRelocEntry(RelocationTable* relocTable, char* symbol);

/**
 * 
 * @param relocTable 
 */
void displayRelocTable(RelocationTable* relocTable);

/**
 * 
 * @param relocTable 
 */
void deleteRelocTable(RelocationTable* relocTable);

#endif