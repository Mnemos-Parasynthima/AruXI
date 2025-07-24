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
	section_entry_t entries[6]; // 0 for data, 1 for const, 2 for bss, 3 for text, 4 for evt, 5 for ivt
	uint8_t activeSection; // 0 for data, 1 for const, 2 for bss, 3 for text, 4 for evt, 5 for ivt
} SectionTable;

SectionTable* initSectionTable();

void displaySectionTable(SectionTable* sectTable);

void deleteSectionTable(SectionTable* sectTable);

#endif