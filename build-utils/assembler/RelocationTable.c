#include <stdlib.h>
#include <string.h>

#include "RelocationTable.h"
#include "assemblerError.h"


RelocationTable* initRelocTable() {
	RelocationTable* relocTable = (RelocationTable*) malloc(sizeof(RelocationTable));
	if (!relocTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for relocation table!\n");

	relocTable->entries = (reloc_entry_t**) malloc(sizeof(reloc_entry_t*) * 10);
	if (!relocTable->entries) handleError(ERR_MEM, FATAL, "Could not allocate memory for relocation entries!\n");
	relocTable->capacity = 10;
	relocTable->size = 0;

	return relocTable;
}

reloc_entry_t* initRelocEntry(uint8_t section, uint32_t offset, char* symbol, uint8_t type) {
	reloc_entry_t* relocEntry = (reloc_entry_t*) malloc(sizeof(reloc_entry_t));
	if (!relocEntry) handleError(ERR_MEM, FATAL, "Could not allocate memory for relocation entry!\n");

	size_t symbolLen = strlen(symbol);
	relocEntry->symbol = (char*) malloc(sizeof(char) * symbolLen + 1);
	if (!relocEntry->symbol) handleError(ERR_MEM, FATAL, "Could not allocate memory for relocation symbol!\n");
	strcpy(relocEntry->symbol, symbol);

	relocEntry->section = section;
	relocEntry->offset = offset;
	relocEntry->type = type;

	return relocEntry;
}

void addRelocEntry(RelocationTable* relocTable, reloc_entry_t* relocEntry) {
	if (relocTable->size == relocTable->capacity) {
		relocTable->capacity *= 2;

		reloc_entry_t** temp = (reloc_entry_t**) realloc(relocTable->entries, relocTable->capacity * sizeof(reloc_entry_t*));
		if (!temp) handleError(ERR_MEM, FATAL, "Could not reallocate memory for new relocation entries!\n");

		relocTable->entries = temp;
	}

	int idx = relocTable->size;
	relocTable->entries[idx] = relocEntry;
	relocTable->size++;
}

reloc_entry_t* getRelocEntry(RelocationTable* relocTable, char* symbol) {
	for (int i = 0; i < relocTable->size; i++) {
		if (strcmp(relocTable->entries[i]->symbol, symbol) == 0) return relocTable->entries[i];
	}

	return NULL;
}

void displayRelocTable(RelocationTable* relocTable) {
}

void deleteRelocTable(RelocationTable* relocTable) {
	for (int i = 0; i < relocTable->size; i++) {
		reloc_entry_t* entry = relocTable->entries[i];
		free(entry->symbol);
		free(entry);
	}
	free(relocTable->entries);
	free(relocTable);
}