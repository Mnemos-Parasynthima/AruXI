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

void deleteSectionTable(SectionTable* sectTable) {
	free(sectTable);
}