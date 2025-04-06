#include <stdlib.h>
#include <string.h>

#include "SymbolTable.h"
#include "assemblerError.h"


SymbolTable* initSymbTable() {
	SymbolTable* symbtable = (SymbolTable*) malloc(sizeof(SymbolTable));
	if (!symbtable) handleError(ERR_MEM, FATAL, "Could not allocate memory for SymbolTable!\n");

	symbtable->entries = (symb_entry_t**) malloc(sizeof(symb_entry_t*) * 10);
	symbtable->capacity = 10;
	symbtable->size = 0;

	return symbtable;
}

symb_entry_t* initSymbEntry(char* name, char* expr, int32_t value, uint32_t flags) {
	symb_entry_t* symbEntry = (symb_entry_t*) malloc(sizeof(symb_entry_t));
	if (!symbEntry) handleError(ERR_MEM, FATAL, "Could not allocate memory for Symbol Entry!\n");

	size_t nameLen = strlen(name);
	symbEntry->name = (char*) malloc(sizeof(char) * nameLen + 1);
	strcpy(symbEntry->name, name);

	if (expr) symbEntry->expr = expr;
	else symbEntry->value = value;

	symbEntry->flags = flags;

	return symbEntry;
}

void addSymbEntry(SymbolTable* symbTable, symb_entry_t* symbEntry) {
	if (symbTable->size == symbTable->capacity) {
		symbTable->capacity *= 2;

		symb_entry_t** temp = (symb_entry_t**) realloc(symbTable->entries, symbTable->capacity * sizeof(symb_entry_t*));
		if (!temp) handleError(ERR_MEM, FATAL, "Could not reallocate memory for new entries!\n");

		symbTable->entries = temp;
	}

	int idx = symbTable->size;
	symbTable->entries[idx] = symbEntry;
	symbTable->size++;	
}

void updateSymbEntry(symb_entry_t* symbEntry, uint32_t value, uint32_t flags) {
	free(symbEntry->expr);
	symbEntry->value = value;
	// symbEntry->flags &= ~(1 << 7); // setting 0 to E flag
	symbEntry->flags = flags;
}

symb_entry_t* getSymbEntry(SymbolTable* symbTable, char* name) {
	for (int i = 0; i < symbTable->size; i++) {
		if (strcmp(symbTable->entries[i]->name, name) == 0) return symbTable->entries[i];
	}

	return NULL;
}

void deleteSymbTable(SymbolTable* symbTable) {
	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		free(entry->name);
		// Assuming expr has been freed before
		// Should be guaranteed by updateSymbEntry()
	}
	free(symbTable->entries);
	free(symbTable);
}