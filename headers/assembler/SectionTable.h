#ifndef _SECTION_TABLE_H_
#define _SECTION_TABLE_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct SectEntry {
	uint32_t lp; // Location pointer
	uint32_t size; // Size of section (in bytes)
	bool present; // Whether this section exists (note that it could technically be checked if size is 0)
} section_entry_t;

typedef struct SectionTable {
	section_entry_t entries[4]; // 0 for text, 1 for data, 2 for const, 3 for bss
	uint8_t activeSection; // 0 for text, 1 for data, 2 for const, 3 for bss
} SectionTable;

SectionTable* initSectionTable();

void deleteSectionTable(SectionTable* sectTable);

#endif