#ifndef _DATA_TABLE_H
#define _DATA_TABLE_H

#include <stdint.h>


typedef struct DataEntry {
	uint8_t type; // 0:string, 1:bytes, 2:halfwords, 3:words, 4:floats
	uint32_t addr; // Address in respect to its section
	uint32_t size; // Total size that it occupies
	char* source; // The original string
	union {
		char* str;
		uint8_t* bytes;
		uint16_t* hwords;
		uint32_t* words;
		float* floats;
	} data;
} data_entry_t;

typedef struct DataTable {
	data_entry_t** dataEntries;
	uint32_t dSize;
	uint32_t dCapacity;

	data_entry_t** constEntries;
	uint32_t cSize;
	uint32_t cCapacity;

	data_entry_t** bssEntries;
	uint32_t bSize;
	uint32_t bCapacity;
} DataTable;

typedef enum {
	DATA_SECT,
	CONST_SECT,
	BSS_SECT
} data_sect_type_t;


/**
 * 
 * @return 
 */
DataTable* initDataTable();

/**
 * 
 * @param type 
 * @param addr 
 * @param size 
 * @param source 
 * @param data 
 * @return 
 */
data_entry_t* initDataEntry(uint8_t type, uint32_t addr, uint32_t size, char* source, void* data);

/**
 * 
 * @param dataTable 
 * @param dataEntry 
 * @param sectType 
 */
void addDataEntry(DataTable* dataTable, data_entry_t* dataEntry, data_sect_type_t sectType);

/**
 * Gets the data entry from a given section based on the address.
 * @param dataTable The data table
 * @param sectType The section type
 * @param addr The address of the data
 * @return The data entry
 */
data_entry_t* getDataEntry(DataTable* dataTable, data_sect_type_t sectType, uint32_t addr);

void deleteDataTable(DataTable* dataTable);


#endif