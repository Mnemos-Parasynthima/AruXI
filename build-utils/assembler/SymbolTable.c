#include <stdlib.h>
#include <string.h>

#include "SymbolTable.h"
#include "assemblerDiagnostics.h"


SymbolTable* initSymbTable() {
	SymbolTable* symbTable = (SymbolTable*) malloc(sizeof(SymbolTable));
	if (!symbTable) emitError(ERR_MEM, NULL, "Could not allocate memory for SymbolTable!\n");

	symbTable->entries = (symb_entry_t**) malloc(sizeof(symb_entry_t*) * 10);
	if (!symbTable->entries) emitError(ERR_MEM, NULL, "Could not allocate memory for symbol entries!\n");
	symbTable->capacity = 10;
	symbTable->size = 0;

	return symbTable;
}

symb_entry_t* initSymbEntry(char* name, char* expr, int32_t value, uint32_t flags, char* source) {
	symb_entry_t* symbEntry = (symb_entry_t*) malloc(sizeof(symb_entry_t));
	if (!symbEntry) emitError(ERR_MEM, NULL, "Could not allocate memory for symbol entry!\n");

	size_t nameLen = strlen(name);
	symbEntry->name = (char*) malloc(sizeof(char) * nameLen + 1);
	if (!symbEntry->name) emitError(ERR_MEM, NULL, "Could not allocate memory for symbol name!\n");
	strcpy(symbEntry->name, name);

	if (expr && GET_EXPRESSION(flags) == 1) symbEntry->expr = expr;
	else symbEntry->value = value;
	
	symbEntry->flags = flags;
	symbEntry->source = source; // Assuming it has been malloc'd
	symbEntry->linenum = 0;

	return symbEntry;
}

void addSymbEntry(SymbolTable* symbTable, symb_entry_t* symbEntry) {
	if (symbTable->size == symbTable->capacity) {
		symbTable->capacity *= 2;

		symb_entry_t** temp = (symb_entry_t**) realloc(symbTable->entries, symbTable->capacity * sizeof(symb_entry_t*));
		if (!temp) emitError(ERR_MEM, NULL, "Could not reallocate memory for new entries!\n");

		symbTable->entries = temp;
	}

	int idx = symbTable->size;
	symbTable->entries[idx] = symbEntry;
	symbTable->size++;	
}

void updateSymbEntry(symb_entry_t* symbEntry, uint32_t value, uint32_t flags) {
	free(symbEntry->expr);
	symbEntry->value = value;
	symbEntry->flags = flags; // assuming `flags` has been updated
}

symb_entry_t* getSymbEntry(SymbolTable* symbTable, char* name) {
	if (!symbTable) return NULL;

	for (int i = 0; i < symbTable->size; i++) {
		if (strcmp(symbTable->entries[i]->name, name) == 0) return symbTable->entries[i];
	}

	return NULL;
}

void displaySymbTable(SymbolTable* symbTable) {
	debug(DEBUG_TRACE, "Symbol Table with %d entries:\n", symbTable->size);
	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		uint32_t flags = entry->flags;
		uint8_t EXPR = GET_EXPRESSION(flags);
		uint8_t SECT = GET_SECTION(flags);
		uint8_t TYPE = GET_TYPE(flags);
		uint8_t LOCALITY = GET_LOCALITY(flags);
		uint8_t REFERENCE = GET_REFERENCE(flags);
		uint8_t DEFINED = GET_DEFINED(flags);
		
		debug(DEBUG_TRACE, "%s: ", entry->name);

		if (EXPR == 0b1) debug(DEBUG_TRACE, "(%s)\t", entry->expr);
		else debug(DEBUG_TRACE, "(0x%x)\t", entry->value);

		char* sectStr;
		if (SECT == 0b00) sectStr = "DATA";
		else if (SECT == 0b01) sectStr = "CONST";
		else if (SECT == 0b10) sectStr = "BSS";
		else if (SECT == 0b11) sectStr = "TEXT";
		else if (SECT == 0b100) sectStr = "EVT";
		else if (SECT == 0b101) sectStr = "IVT";
		else sectStr = "UNDEF";

		char* typeStr;
		if (TYPE == 0b00) typeStr = "NONE";
		else if (TYPE == 0b01) typeStr = "ABS";
		else if (TYPE == 0b10) typeStr = "FUNC";
		else if (TYPE == 0b11) typeStr = "OBJ";
		else typeStr = "UNDEF";

		// EXPR: (0|1); SECT: (DATA|CONST|BSS|TEXT|EVT|IVT); TYPE: (NONE|ABS|FUNC|OBJ; LOCALITY: (LOC|GLOB); REFERENCE: (0|1); DEFINED: (0|1)
		debug(DEBUG_TRACE, "EXPR: %d; SECT: %s; TYPE: %s; LOCALITY: %s; REFERENCE: %d; DEFINED: %d\n", 
			EXPR, sectStr, typeStr, ((LOCALITY == 0b0) ? "LOC" : "GLOB"), REFERENCE, DEFINED);
	}
}

void deleteSymbTable(SymbolTable* symbTable) {
	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		free(entry->name);
		if (entry->source) free(entry->source);
		free(entry);
		// Assuming expr has been freed before
		// Should be guaranteed by updateSymbEntry()
	}
	free(symbTable->entries);
	free(symbTable);
}