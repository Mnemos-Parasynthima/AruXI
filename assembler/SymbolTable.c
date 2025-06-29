#include <stdlib.h>
#include <stdio.h>
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

	if (expr && GET_EXPRESSION(flags) == 1) symbEntry->expr = expr;
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
	if (!symbTable) return NULL;

	for (int i = 0; i < symbTable->size; i++) {
		if (strcmp(symbTable->entries[i]->name, name) == 0) return symbTable->entries[i];
	}

	return NULL;
}

void displaySymbTable(SymbolTable* symbTable) {
	debug("Symbol Table with %d entries:\n", symbTable->size);
	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		uint32_t flags = entry->flags;
		uint8_t EXPR = GET_EXPRESSION(flags);
		uint8_t SECT = GET_SECTION(flags);
		uint8_t TYPE = GET_TYPE(flags);
		uint8_t LOCALITY = GET_LOCALITY(flags);
		uint8_t REFERENCE = GET_REFERENCE(flags);
		uint8_t DEFINED = GET_DEFINED(flags);
		
		debug("%s: ", entry->name);
		if (EXPR == 0b1) printf("(%s)\t", entry->expr);
		else printf("(0x%x)\t", entry->value);

		char sectStr[6];
		if (SECT == 0b00) sprintf(sectStr, "DATA");
		else if (SECT == 0b01) sprintf(sectStr, "CONST");
		else if (SECT == 0b10) sprintf(sectStr, "BSS");
		else sprintf(sectStr, "TEXT");

		// EXPR: (0|1); SECT: (DATA|CONST|BSS|TEXT); TYPE: (ADDR|SET); LOCALITY: (LOC|GLOB); REFERENCE: (0|1); DEFINED: (0|1)
		debug("EXPR: %d; SECT: %s; TYPE: %s; LOCALITY: %s; REFERENCE: %d; DEFINED: %d\n", 
			EXPR, sectStr, ((TYPE == 0b0) ? "ADDR" : "SET"), ((LOCALITY == 0b0) ? "LOC" : "GLOB"), REFERENCE, DEFINED);
	}
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