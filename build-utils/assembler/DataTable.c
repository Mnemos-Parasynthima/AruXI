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

	dataTable->evtEntries = (data_entry_t**) malloc(sizeof(data_entry_t*) * 5);
	dataTable->eSize = 0;
	dataTable->eCapacity = 5;

	return dataTable;
}

data_entry_t* initDataEntry(data_t type, uint32_t addr, uint32_t size, char* source, void* data) {
	data_entry_t* dataEntry = (data_entry_t*) malloc(sizeof(data_entry_t));
	if (!dataEntry) handleError(ERR_MEM, FATAL, "Could not allocate space for data entry!\n");

	dataEntry->type = type;
	dataEntry->addr = addr;
	dataEntry->size = size;
	dataEntry->source = (char*) malloc(sizeof(char) * strlen(source) + 1);
	if (dataEntry->source) strcpy(dataEntry->source, source); // source is optional, no fret if no space
	dataEntry->data._data = data;	// Assuming it has been allocated

	return dataEntry;
}

void addDataEntry(DataTable* dataTable, data_entry_t* dataEntry, data_sect_t sectType) {
	data_entry_t*** entries = NULL;
	uint32_t* size = 0;
	uint32_t* capacity = 0;

	debug("Detected section to add for: %d\n", sectType);
	if (sectType == DATA_SECT) {
		entries = &dataTable->dataEntries;
		size = &dataTable->dSize;
		capacity = &dataTable->dCapacity;
	} else if (sectType == CONST_SECT) {
		entries = &dataTable->constEntries;
		size = &dataTable->cSize;
		capacity = &dataTable->cCapacity;
	} else if (sectType == BSS_SECT) {
		entries = &dataTable->bssEntries;
		size = &dataTable->bSize;
		capacity = &dataTable->bCapacity;
	} else if (sectType == EVT_SECT) {
		entries = &dataTable->evtEntries;
		size = &dataTable->eSize;
		capacity = &dataTable->eCapacity;
	}	else return;

	if (*size == *capacity) {
		*capacity *= 2;

		data_entry_t** temp = (data_entry_t**) realloc(*entries, sizeof(data_entry_t*) * (*capacity));
		if (!temp) handleError(ERR_MEM, FATAL, "Could not reallocate memory for data entries!\n");

		*entries = temp;
	}

	int idx = *size;
	(*entries)[idx] = dataEntry;
	*size = *size + 1;
}

data_entry_t* getDataEntry(DataTable* dataTable, data_sect_t sectType, uint32_t addr) {
	data_entry_t** entries = NULL;
	uint32_t size = 0;

	if (sectType == DATA_SECT) { entries = dataTable->dataEntries; size = dataTable->dSize; }
	else if (sectType == CONST_SECT) { entries = dataTable->constEntries; size = dataTable->cSize; }
	else if (sectType == BSS_SECT) { entries = dataTable->bssEntries; size = dataTable->bSize; }
	else if (sectType == EVT_SECT) { entries = dataTable->evtEntries; size = dataTable->eSize; }
	else return NULL;


	for (int i = 0; i < size; i++) {
		data_entry_t* entry = entries[i];

		if (entry->addr == addr) return entry;
	}

	return NULL;
}

static void displayDataEntry(data_entry_t* dataEntry) {
	// TODO: Display actual data
	uint8_t type = dataEntry->type;

	char* typeStr = NULL;
	char* strdata = NULL;
	switch (type)	{
		case STRING_TYPE:
			typeStr = "string";
			strdata = dataEntry->data.str;
			break;
		case BYTES_TYPE:
			typeStr = "bytes";
			break;
		case HWORDS_TYPE:
			typeStr = "halfwords";
			break;
		case WORDS_TYPE:
			typeStr = "words";
			break;
		case FLOATS_TYPE:
			typeStr = "floats";
			break;
		default:
			break;
	}

	debug("\t\tType: %s\n", typeStr);
	debug("\t\tAddress: 0x%x\n", dataEntry->addr);
	debug("\t\tSize: 0x%x\n", dataEntry->size);
	debug("\t\tSource: %s\n\n", dataEntry->source);
	// debug("\t\tData: %s\n", dataEntry->data);
}

void displayDataTable(DataTable* dataTable) {
	debug("Data Table:\n");

	debug("\tData (%d entries)\n", dataTable->dSize);
	for (int i = 0; i < dataTable->dSize; i++) {
		displayDataEntry(dataTable->dataEntries[i]);
	}
	debug("\n");
	
	debug("\tConst (%d entries)\n", dataTable->cSize);
	for (int i = 0; i < dataTable->cSize; i++) {
		displayDataEntry(dataTable->constEntries[i]);
	}
	debug("\n");

	debug("\tBss (%d entries)\n", dataTable->bSize);
	for (int i = 0; i < dataTable->bSize; i++) {
		displayDataEntry(dataTable->bssEntries[i]);
	}
	debug("\n");

	debug("\tEVT (%d entires)\n", dataTable->eSize);
	for (int i = 0; i < dataTable->eSize; i++) {
		displayDataEntry(dataTable->evtEntries[i]);
	}
	debug("\n");
}

void deleteDataTable(DataTable* dataTable) {
	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		free(entry->source);
		free(entry->data.bytes);
		free(entry);
	}
	free(dataTable->dataEntries);

	for (int i = 0; i < dataTable->cSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		free(entry->source);
		free(entry->data.bytes);
		free(entry);
	}
	free(dataTable->constEntries);

	for (int i = 0; i < dataTable->bSize; i++) {
		data_entry_t* entry = dataTable->bssEntries[i];

		free(entry->source);
		// entry->data.* is not freed since bss cannot allow for actual data
		// it only needs the size needed
		free(entry);
	}
	free(dataTable->bssEntries);

	for (int i = 0; i < dataTable->eSize; i++) {
		data_entry_t* entry = dataTable->evtEntries[i];

		free(entry->source);
		free(entry->data.bytes);
		free(entry);
	}
	free(dataTable->evtEntries);

	free(dataTable);
}