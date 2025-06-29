#include <stdio.h>
#include <stdlib.h>

#include "SectionTable.h"
#include "assemblerError.h"


SectionTable* initSectionTable() {
	SectionTable* sectTable = (SectionTable*) malloc(sizeof(SectionTable));
	if (!sectTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for section table!\n");

	for (int i = 0; i < 4; i++) {
		sectTable->entries[i].lp = 0x0;
		sectTable->entries[i].size = 0x0;
		sectTable->entries[i].present = false;
	}
	sectTable->activeSection = 0;

	return sectTable;
}

void displaySectionTable(SectionTable* sectTable) {
	debug("Section Table (active section: %d):\n", sectTable->activeSection);

	debug("\tData Section (0):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[0].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[0].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[0].present ? "true" : "false"));

	debug("\tConst Section (1):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[1].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[1].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[1].present ? "true" : "false"));

	debug("\tBss Section (2):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[2].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[2].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[2].present ? "true" : "false"));

	debug("\tText Section (3):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[3].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[3].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[3].present ? "true" : "false"));
}

void deleteSectionTable(SectionTable* sectTable) {
	free(sectTable);
}