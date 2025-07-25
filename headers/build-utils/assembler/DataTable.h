#ifndef _DATA_TABLE_H
#define _DATA_TABLE_H

#include <stdint.h>


typedef struct DataEntry {
	uint8_t type; // 0:string, 1:bytes, 2:halfwords, 3:words, 4:floats
	uint32_t addr; // Address in respect to its section
	uint32_t size; // Total size that it occupies
	char* source; // The original string that has been malloc'd
	int linenum; // Line number that the data is found in
	union {
		char* str;
		uint8_t* bytes;
		uint16_t* hwords;
		uint32_t* words;
		float* floats;
		void* _data;
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

	data_entry_t** evtEntries;
	uint32_t eSize;
	uint32_t eCapacity;
} DataTable;

typedef enum {
	DATA_SECT,
	CONST_SECT,
	BSS_SECT,
	NONE_SECT,
	EVT_SECT,
	IVT_SECT
} data_sect_t;

typedef enum {
	STRING_TYPE,
	BYTES_TYPE,
	HWORDS_TYPE,
	WORDS_TYPE,
	FLOATS_TYPE,
	NONE_TYPE,
} data_t;


/**
 * 
 * @return 
 */
DataTable* initDataTable();

/**
 * Creates a data entry for the data table with the given information.
 * @param type The type of data: 0 for str, 1 for bytes, 2 for halfwords, 3 for words, 4 for floats
 * @param addr The address in which the data is located at relative to its section, aka the LP
 * @param size The total size that the data occupies in bytes
 * @param source The original source string that must be malloc'd
 * @param data The actual data itself that must be malloc'd
 * @return The new data entry
 */
data_entry_t* initDataEntry(data_t type, uint32_t addr, uint32_t size, char* source, void* data);

/**
 * 
 * @param dataTable 
 * @param dataEntry 
 * @param sectType 
 */
void addDataEntry(DataTable* dataTable, data_entry_t* dataEntry, data_sect_t sectType);

/**
 * Gets the data entry from a given section based on the address.
 * @param dataTable The data table
 * @param sectType The section type
 * @param addr The address of the data
 * @return The data entry
 */
data_entry_t* getDataEntry(DataTable* dataTable, data_sect_t sectType, uint32_t addr);

void displayDataTable(DataTable* dataTable);

void deleteDataTable(DataTable* dataTable);


#endif