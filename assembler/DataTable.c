#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataTable.h"
#include "assemblerError.h"


DataTable* initDataTable() {
	DataTable* dataTable = (DataTable*) malloc(sizeof(DataTable));
	if (!dataTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for data table!\n");

	dataTable->dataEntries = (data_entry_t**) malloc(sizeof(data_entry_t*) * 5);
	dataTable->dSize = 0;
	dataTable->dCapacity = 5;

	dataTable->constEntries = (data_entry_t**) malloc(sizeof(data_entry_t*) * 5);
	dataTable->cSize = 0;
	dataTable->cCapacity = 5;

	dataTable->bssEntries = (data_entry_t**) malloc(sizeof(data_entry_t*) * 5);
	dataTable->bSize = 0;
	dataTable->bCapacity = 5;

	return dataTable;
}

data_entry_t* initDataEntry(uint8_t type, uint32_t addr, uint32_t size, char* source, void* data) {
	data_entry_t* dataEntry = (data_entry_t*) malloc(sizeof(data_entry_t));
	if (!dataEntry) handleError(ERR_MEM, FATAL, "Could not allocate space for data entry!\n");

	dataEntry->type = type;
	dataEntry->addr = addr;
	dataEntry->size = size;
	dataEntry->source = (char*) malloc(sizeof(char) * strlen(source) + 1);
	strcpy(dataEntry->source, source);
	dataEntry->data.bytes = data;	// Assuming it has been allocated

	return dataEntry;
}

void addDataEntry(DataTable* dataTable, data_entry_t* dataEntry, data_sect_type_t sectType) {
	data_entry_t** entries = NULL;

	if (sectType == DATA_SECT) entries = dataTable->dataEntries;
	else if (sectType == CONST_SECT) entries = dataTable->constEntries;
	else if (sectType == BSS_SECT) entries = dataTable->bssEntries;
	else return;

	// TODO
}

data_entry_t* getDataEntry(DataTable* dataTable, data_sect_type_t sectType, uint32_t addr) {
	data_entry_t** entries = NULL;
	uint32_t size = 0;

	if (sectType == DATA_SECT) { entries = dataTable->dataEntries; size = dataTable->dSize; }
	else if (sectType == CONST_SECT) { entries = dataTable->constEntries; size = dataTable->cSize; }
	else if (sectType == BSS_SECT) { entries = dataTable->bssEntries; size = dataTable->bSize; }
	else return NULL;


	for (int i = 0; i < size; i++) {
		data_entry_t* entry = entries[i];

		if (entry->addr == addr) return entry;
	}

	return NULL;
}

void deleteDataTable(DataTable* dataTable) {
	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		free(entry->source);
		free(entry->data.bytes);
		free(entry);
	}

	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		free(entry->source);
		free(entry->data.bytes);
		free(entry);
	}

	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->bssEntries[i];

		free(entry->source);
		// entry->data. is not freed since bss cannot allow for actual data
		// it only needs the size needed
		free(entry);
	}

	free(dataTable);
}