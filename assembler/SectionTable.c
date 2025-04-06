#include <stdio.h>
#include <stdlib.h>

#include "SectionTable.h"
#include "assemblerError.h"


SectionTable* initSectionTable() {
	SectionTable* sectTable = (SectionTable*) malloc(sizeof(SectionTable));
	if (!sectTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for section table!\n");

	for (int i = 0; i < 3; i++) {
		sectTable->entries[i].lp = 0x0;
		sectTable->entries[i].lp = 0;
	}
	sectTable->activeSection = 0;

	return sectTable;
}

void displaySectionTable(SectionTable* sectTable) {
	printf("Section Table (active section: %d):\n", sectTable->activeSection);

	printf("\tData Section (0):\n");
	printf("\t\tLocation Pointer: 0x%x\n", sectTable->entries[0].lp);
	printf("\t\tSize: %x bytes\n", sectTable->entries[0].size);
	printf("\t\tIs present: %s\n", (sectTable->entries[0].present ? "true" : "false"));

	printf("\tConst Section (1):\n");
	printf("\t\tLocation Pointer: 0x%x\n", sectTable->entries[1].lp);
	printf("\t\tSize: %x bytes\n", sectTable->entries[1].size);
	printf("\t\tIs present: %s\n", (sectTable->entries[1].present ? "true" : "false"));

	printf("\tBss Section (2):\n");
	printf("\t\tLocation Pointer: 0x%x\n", sectTable->entries[2].lp);
	printf("\t\tSize: %x bytes\n", sectTable->entries[2].size);
	printf("\t\tIs present: %s\n", (sectTable->entries[2].present ? "true" : "false"));

	printf("\tText Section (3):\n");
	printf("\t\tLocation Pointer: 0x%x\n", sectTable->entries[3].lp);
	printf("\t\tSize: %x bytes\n", sectTable->entries[3].size);
	printf("\t\tIs present: %s\n", (sectTable->entries[3].present ? "true" : "false"));
}

void deleteSectionTable(SectionTable* sectTable) {
	free(sectTable);
}