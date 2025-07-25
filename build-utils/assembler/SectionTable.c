#include <stdio.h>
#include <stdlib.h>

#include "SectionTable.h"
#include "assemblerError.h"


SectionTable* initSectionTable() {
	SectionTable* sectTable = (SectionTable*) malloc(sizeof(SectionTable));
	if (!sectTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for section table!\n");

	for (int i = 0; i < IVT+1; i++) {
		sectTable->entries[i].lp = 0x00000000;
		sectTable->entries[i].size = 0x00000000;
		sectTable->entries[i].present = false;
	}
	sectTable->activeSection = 0;

	return sectTable;
}

void displaySectionTable(SectionTable* sectTable) {
	debug("Section Table (active section: %d):\n", sectTable->activeSection);

	debug("\tData Section (0):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[DATA].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[DATA].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[DATA].present ? "true" : "false"));

	debug("\tConst Section (1):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[CONST].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[CONST].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[CONST].present ? "true" : "false"));

	debug("\tBss Section (2):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[BSS].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[BSS].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[BSS].present ? "true" : "false"));

	debug("\tText Section (3):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[TEXT].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[TEXT].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[TEXT].present ? "true" : "false"));

	debug("\tEVT Section (4):\n");
	debug("\t\tLocation Pointer: 0x%x\n", sectTable->entries[EVT].lp);
	debug("\t\tSize: 0x%x bytes\n", sectTable->entries[EVT].size);
	debug("\t\tIs present: %s\n", (sectTable->entries[EVT].present ? "true" : "fa;se"));
}

void deleteSectionTable(SectionTable* sectTable) {
	free(sectTable);
}