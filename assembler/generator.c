#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"
#include "assemblerError.h"


extern bool kern;

static void generateData(AOEFbin* bin, DataTable* dataTable) {
	AOEFFSectHeader* dataHeader = NULL;

	for (int i = 0; i < bin->header.hSectSize; i++) {
		AOEFFSectHeader* sectHeader = &(bin->sectHdrTable[i]);
		
		if (strcmp(".data", sectHeader->shSectName) == 0) {
			dataHeader = sectHeader;
			break;
		}
	}

	// data section might not even be present, skip
	if (!dataHeader) return;
	if (dataHeader->shSectSize == 0) return;

	uint8_t* dataSect = malloc(sizeof(uint8_t) * dataHeader->shSectSize);
	if (!dataSect) handleError(ERR_MEM, FATAL, "Could not allocate memory for data section!\n");

	uint32_t pos = 0x0;
	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		for (int b = 0; b < entry->size; b++) {
			dataSect[pos] = entry->data.bytes[b];
			pos++;
		}
	}

	bin->_data = dataSect;
}

static void generateConst(AOEFbin* bin, DataTable* dataTable) {
	AOEFFSectHeader* constHeader = NULL;

	for (int i = 0; i < bin->header.hSectSize; i++) {
		AOEFFSectHeader* sectHeader = &(bin->sectHdrTable[i]);

		if (strcmp(".const", sectHeader->shSectName) == 0) {
			constHeader = sectHeader;
			break;
		}
	}

	// const section might not even be present, skip
	if (!constHeader) return;
	if (constHeader->shSectSize == 0) return;

	uint8_t* constSect = malloc(sizeof(uint8_t) * constHeader->shSectSize);
	if (!constSect) handleError(ERR_MEM, FATAL, "Could not allocate memory for const section!\n");

	uint32_t pos = 0x0;
	for (int i = 0; i < dataTable->cSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		for (int b = 0; b < entry->size; b++) {
			constSect[pos] = entry->data.bytes[b];
			pos++;
		}
	}

	bin->_const = constSect;
}

static void generateText(AOEFbin* bin, InstructionStream* instrStream) {
	if (instrStream->size == 0) return;

	// AOEFFSectHeader* textHeader = NULL;

	// for (int i = 0; i < 4; i++) {
	// 	AOEFFSectHeader* sectHeader = &(bin->sectHdrTable[i]);
		
	// 	if (strcmp(".const", sectHeader->shSectName)) {
	// 		textHeader = sectHeader;
	// 		break;
	// 	}
	// }

	// // const section might not even be present, skip
	// if (!textHeader) return;

	// The number of bytes taken by the text section can either be the number of instructions * 4 bytes
	// or the total size in bytes using sectHeader
	// Note that the size may be inflated due to the possible presence of the EVT instructions but those will be ignored
	uint32_t* textSect = malloc(sizeof(uint32_t) * instrStream->size);
	if (!textSect) handleError(ERR_MEM, FATAL, "Could not allocate memory for text section!\n");

	uint32_t pos = 0x0;

	// Skip EVT instructions
	bool evtInstr = false;

	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instr = instrStream->instructions[i];

		// Ignore placeholders
		if (instr->addr == 0x00000000) { 
			if (!evtInstr) evtInstr = true; // EVT start
			else evtInstr = false; // EVT end
			
			continue;
		}

		if (evtInstr) continue;

		textSect[pos] = instr->encoding;
		pos++;
	}

	bin->_text = textSect;
}

static void generateEVT(AOEFbin* bin, InstructionStream* instrStream, DataTable* dataTable, uint32_t size) {
	// Reconstructing the EVT is somewhat difficult considering two streams: instructions and data
	// Have instrStream be the anchor as that is the clear one with the two buffer instructions before and after

	// Skip until buffer
	instr_obj_t* instr = NULL;
	int iInstr;
	for (iInstr = 0; iInstr < instrStream->size; iInstr++) {
		instr = instrStream->instructions[iInstr];
		if (instr->addr == 0x00000000) break; // initial buffer
	}
	iInstr++;

	// Cannot know size of EVT without being indicated, kind of
	// One possibility is having the size of evtEntries and having the assumption that the number of EVT header instructions
	// are met, but no can do assumptions
	uint8_t* evtSect = malloc(sizeof(uint8_t) * size);
	if (!evtSect) handleError(ERR_MEM, FATAL, "Could not allocate memory for EVT section!\n");

	uint32_t pos = 0x0;
	bool endInstr = false;

	for (int iData = 0; iData < dataTable->eSize; iData++) {
		data_entry_t* entry = dataTable->evtEntries[iData];

		// Compare addresses to know which one to write first
		uint32_t instrAddr = instr->addr;
		uint32_t dataAddr = entry->addr;

		if (instrAddr < dataAddr) {
			// Instruction came first, write it

			// Since evtSect is in bytes but instr is in halfwords, cast it first
			uint32_t* evtSect_Instr = (uint32_t*) evtSect;
			evtSect_Instr[pos] = instr->encoding;
			// Note that since array indexing is used, `pos` is not guaranteed to hit an aligment of 4
			// Not our issue, the CPU will detect it
			pos += 4;

			// Get next instruction if not end
			if (!endInstr) instr = instrStream->instructions[iInstr++];

			// Make sure still within EVT bounds
			if (instr->addr == 0x00000000) {
				// Next (rather this) instruction marks ending, meaning the rest to be written is data
				// Since the comparisons are made using addresses, set the address to be at a high address that it will go to data
				instr->addr = 0xFFFFFFFF;
				// Note that however this check only occurs once and for the instrucions after the ending marker, it will think they are part of the EVT
				// To prevent so, just mark it so
				endInstr = true;
			}

			// Since this is a for loop, advancing iData will be done naturally
			// In this case that instr came first, data must remain
			// Either this can be done in a while loop, I am too lazy to rewrite it
			// Soooo
			iData--;
		} else {
			// Data came first, write it
			// Data may not always be in bytes, same issue as generateData
			for (int b = 0; b < entry->size; b++) {
				evtSect[pos] = entry->data.bytes[b];
				pos++;
			}

			// Advancing to next data entry is done by the for loop
		}
	}

	bin->_evt = evtSect;
}

static uint32_t getSymbolStringsSize(SymbolTable* symbTable) {
	uint32_t size = 0;

	for (int i = 1; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];

		size_t len = strlen(entry->name);

		size += (len + 1); // include null-terminator
	}

	return (size + 16); // string table has ending string of size 16
}

/**
 * Like `strcat` but preserves the previous null byte instead.
 * @param dest 
 * @param src 
 */
static char* _strcat(char* dest, const char* src) {
	char* _src = (char*) src;

	// Copy all of src, including null
	while (*_src) {
		*dest = *_src;
		_src++;
		dest++;
	}

	// _src now at the null byte, copy that one
	*dest = *_src;
	dest++;
	// dest now after null

	return dest;
}

/**
 * Like `strncat` but preserves the previous null byte instead, stil in accordance to the limit.
 * @param dest 
 * @param src 
 */
static char* _strncat(char* dest, const char* src, size_t n) {
	size_t copied = 0;

	char* _src = (char*) src;

	// Copy all of src, including null
	while (*_src) {
		if (copied > n) break;

		*dest = *_src;
		_src++;
		dest++;
		copied++;
	}

	if (copied <= n) {
		*dest = *_src;
		dest++;
	}

	return dest;
}

static void generateSectionHeaders(AOEFbin* bin, SectionTable* sectTable) {
	AOEFFSectHeader* sectHeaders = malloc(sizeof(AOEFFSectHeader) * bin->header.hSectSize);
	if (!sectHeaders) handleError(ERR_MEM, FATAL, "Could not allocate memory for section header table!\n");

	// offset where all sections start at, basically at the end of the string table
	uint32_t baseOffset = bin->header.hStrTabOff + bin->header.hStrTabSize;

	uint32_t offset = baseOffset;

	int sectIdx = 0;
	for (int i = 0; i < 6; i++) {
		section_entry_t entry = sectTable->entries[i];
		if (!entry.present) continue;

		sectHeaders[sectIdx].shSectSize = 0x00000000;
		sectHeaders[sectIdx].shSectOff = 0x00000000;

		char* sectName;
		if (i == 0) sectName = ".data";
		else if (i == 1) sectName = ".const";
		else if (i == 2) sectName = ".bss";
		else if (i == 3) sectName = ".text";
		else if (i == 4) sectName = ".evt";
		else sectName = ".ivt";

		strncpy(sectHeaders[sectIdx].shSectName, sectName, 8);
		sectHeaders[sectIdx].shSectSize = entry.size;
		sectHeaders[sectIdx].shSectOff = offset;

		// Change offset in accordance to the size
		offset += entry.size;

		sectIdx++;
	}

	strncpy(sectHeaders[sectIdx].shSectName, "._none", 8);
	sectHeaders[sectIdx].shSectOff = 0x0000;
	sectHeaders[sectIdx].shSectSize = 0x0000;

	bin->sectHdrTable = sectHeaders;
}

static void generateSymbolTable(AOEFbin* bin, SymbolTable* symbTable) {
	// Symbol Table includes the LP ('@'), ignore it
	// However, the need for end blank symbol undos the -1
	AOEFFSymbEntry* aoeffSymTable = malloc(sizeof(AOEFFSymbEntry) * (symbTable->size));
	if (!aoeffSymTable) handleError(ERR_MEM, FATAL, "Could not allocate memory for AOEFF symbol table!\n");

	// Fill out the string table as well
	char* stStrs = malloc(sizeof(char) * bin->header.hStrTabSize);
	if (!stStrs) handleError(ERR_MEM, FATAL, "Could not allocate memory for strings!\n");

	char* _stStrs = stStrs;

	uint32_t stridx = 0;
	// LP is always first entry, skip it
	for (int i = 1; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];

		uint32_t flags = entry->flags;
		aoeffSymTable[i-1].seSymbInfo = SE_INFO(GET_TYPE(flags), GET_LOCALITY(flags));
		aoeffSymTable[i-1].seSymbSect = GET_SECTION(flags);
		aoeffSymTable[i-1].seSymbVal = entry->value;

		_stStrs = _strcat(_stStrs, entry->name);
		aoeffSymTable[i-1].seSymbName = stridx;
		stridx += (strlen(entry->name) + 1);
	}

	_strncat(_stStrs, "END_AOEFF_STRS\0", 16);

	// Add ending blank entry
	int lastIdx = symbTable->size - 1;
	aoeffSymTable[lastIdx].seSymbInfo = SE_INFO(0, 0);
	aoeffSymTable[lastIdx].seSymbSect = 0x0000;
	aoeffSymTable[lastIdx].seSymbVal = 0x0000;

	bin->symbEntTable = aoeffSymTable;
	bin->stringTable.stStrs = stStrs;
}

AOEFbin* generateBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable) {
	int sectEntries = 0;

	for (int i = 0; i < 4; i++) {
		if (sectTable->entries[i].present) sectEntries++;
	}

	sectEntries++; // ending blank entry
	debug("%d sect entries\n", sectEntries);
	uint32_t symbTableSize = symbTable->size;
	// Since the symbol table will have an empty ending symbol, the size would be +1
	// However, symbTable.size includes the LP, so have that +1 be the blank

	uint32_t symbOff = sizeof(AOEFFheader) + (sizeof(AOEFFSectHeader) * sectEntries);
	uint32_t strTabOff = symbOff + (sizeof(AOEFFSymbEntry) * symbTableSize);

	uint32_t symbStrs = getSymbolStringsSize(symbTable);

	AOEFFheader header = {
		.hID = {AH_ID0, AH_ID1, AH_ID2, AH_ID3},
		.hType = ((kern) ?  AHT_KERN : AHT_EXEC),
		.hEntry = getSymbEntry(symbTable, "_init")->value,
		.hSectOff = sizeof(AOEFFheader),
		.hSectSize = sectEntries,
		.hSymbOff = symbOff,
		.hSymbSize = symbTableSize,
		.hStrTabOff = strTabOff,
		.hStrTabSize = symbStrs
	};

	AOEFbin* bin = malloc(sizeof(AOEFbin));
	if (!bin) handleError(ERR_MEM, FATAL, "Could not allocate memory for the binary image!\n");

	bin->header = header;

	bin->symbEntTable = NULL;
	bin->stringTable.stStrs = NULL;
	generateSymbolTable(bin, symbTable);
	bin->sectHdrTable = NULL;
	generateSectionHeaders(bin, sectTable);

	bin->_data = NULL;
	generateData(bin, dataTable);
	bin->_const = NULL;
	generateConst(bin, dataTable);
	bin->_text = NULL;
	generateText(bin, instrStream);
	bin->_evt = NULL;
	generateEVT(bin, instrStream, dataTable, sectTable->entries[4].size);

	return bin;
}

void writeBinary(AOEFbin* bin, char* outbin) {
	FILE* outfile = fopen(outbin, "wb");
	if (!outfile) handleError(ERR_IO, FATAL, "Could not open %s!\n", outbin);

	// Write header info
	fwrite(&bin->header.hID, sizeof(uint8_t), 4, outfile);
	fwrite(&bin->header.hType, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hEntry, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hSectOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hSectSize, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hSymbOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hSymbSize, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hStrTabOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.hStrTabSize, sizeof(uint32_t), 1, outfile);

	// Write tables
	// debug("secthdr[0]: %s %x %x\n", bin->sectHdrTable[0].shSectName, bin->sectHdrTable[0].shSectOff, bin->sectHdrTable[0].shSectSize);
	// debug("secthdr[1]: %s %x %x\n", bin->sectHdrTable[1].shSectName, bin->sectHdrTable[1].shSectOff, bin->sectHdrTable[1].shSectSize);
	// debug("secthdr[2]: %s %x %x\n", bin->sectHdrTable[2].shSectName, bin->sectHdrTable[2].shSectOff, bin->sectHdrTable[2].shSectSize);
	fwrite(bin->sectHdrTable, sizeof(AOEFFSectHeader), bin->header.hSectSize, outfile);
	fwrite(bin->symbEntTable, sizeof(AOEFFSymbEntry), bin->header.hSymbSize, outfile);
	fwrite(bin->stringTable.stStrs, sizeof(char), bin->header.hStrTabSize, outfile);

	// Write payload
	AOEFFSectHeader* dataHeader = NULL;
	AOEFFSectHeader* constHeader = NULL;
	AOEFFSectHeader* textHeader = NULL;
	AOEFFSectHeader* evtHeader = NULL;
	AOEFFSectHeader* ivtHeader = NULL;
	AOEFFSectHeader* header = NULL;

	for (int i = 0; i < bin->header.hSectSize; i++) {
		header = &(bin->sectHdrTable[i]);

		if (strcmp(".data", header->shSectName) == 0) dataHeader = header;
		else if (strcmp(".const", header->shSectName) == 0) constHeader = header;
		else if (strcmp(".text", header->shSectName) == 0) textHeader = header;
		else if (strcmp(".evt", header->shSectName) == 0) evtHeader = header;
	}

	if (dataHeader && dataHeader->shSectSize != 0) fwrite(bin->_data, sizeof(uint8_t), dataHeader->shSectSize, outfile);
	if (constHeader && constHeader->shSectSize != 0) fwrite(bin->_const, sizeof(uint8_t), constHeader->shSectSize, outfile);
	if (textHeader && textHeader->shSectSize != 0) fwrite(bin->_text, sizeof(uint8_t), textHeader->shSectSize, outfile);
	if (evtHeader && evtHeader->shSectSize != 0) fwrite(bin->_evt, sizeof(uint8_t), evtHeader->shSectSize, outfile);

	debug("Wrote to %s!\n", outbin);

	fclose(outfile);
	free(bin->stringTable.stStrs);
	free(bin->symbEntTable);
	free(bin->sectHdrTable);
	free(bin->_data);
	free(bin->_const);
	free(bin->_text);
	free(bin->_evt);
	free(bin);
}