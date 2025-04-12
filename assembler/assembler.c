#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include "SectionTable.h"
#include "InstructionStream.h"
#include "DataTable.h"
#include "SymbolTable.h"
#include "assemblerError.h"
#include "preprocessor.h"
#include "lexer-parser.h"
#include "evaluator.h"



static void resolveSymbols(SymbolTable* symbTable) {
	printf("Resolving symbols!\n");

	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		// Resolve when an expression
		if (GET_EXPRESSION(entry->flags) == 0b1) {
			char* expr = entry->expr;
			printf("Resolving (%s)\n", expr);

			bool canEval = true;
			int32_t value = eval(expr, symbTable, &canEval);
			printf("Resolved to %d\n", value);

			// canEval needs to be true always
			if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate symbol %s for the last pass!\n", entry->name);

			SET_EXPRESSION(entry->flags);
			entry->value = value;
		}
	}
}


static void completeData(DataTable* dataTable, SymbolTable* symbTable) {
	printf("Completing data!\n");
	// .byte, .hword, and .word are the only ones that left it blank
	// place in `source`
	// To determine if the data entry is to be completed, either `data` can be checked if NULL
	// or `type`

	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		if (!entry->data._data) {
			char* srcData = entry->source;
			printf("Completing (%s)\n", srcData);
			// data remains as the source, that is like "0xff, 0x0, 0x10"

			// `strtok` can either be on `source` itself, but it will "ruin" it, thus not able to see the source later on
			// or make a temp copy, work on that copy, keeping the source intact
			char* temp = (char*) malloc(sizeof(char) * strlen(srcData) + 1);
			strcpy(temp, srcData);
			
			// This is to work on all types so make it generic
			void* data = malloc(entry->size);
			uint32_t i = 0;
			
			char* saveptr = NULL;
			char* dataI = strtok_r(temp, ",", &saveptr);
			while (dataI) {
				printf("Evaluating (%s)\n", dataI);
				bool canEval = true;
				uint32_t value = eval(dataI, symbTable, &canEval);

				// Make sure it is evaluated
				if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate %s!\n", dataI);

				// Depending on the type, make sure it conforms to size
				switch (entry->type)	{
					case 1: // byte
						if ((value & 0xff00UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for byte!\n", dataI);
						uint8_t* bytedata = (uint8_t*) data;
						bytedata[i] = (uint8_t) value;
						break;
					case 2: // halfwords
						if ((value & 0xff0000UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for halfword!\n", dataI);
						uint16_t* hworddata = (uint16_t*) data;
						hworddata[i] = (uint16_t) value;
						break;
					case 3: //words
						if ((value & 0xff00000000UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for word!\n", dataI);
						uint32_t* worddata = (uint32_t*) data;
						worddata[i] = value;
						break;
					default:
						break;
				}

				i++;
				dataI = strtok_r(NULL, ",", &saveptr);
			}

			entry->data._data = data;
			free(temp);
		}
	}

	printf("Completing const!\n");
	for (int i = 0; i < dataTable->cSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		if (!entry->data._data) {
			char* srcData = entry->source;
			printf("Completing (%s)\n", srcData);
			// data remains as the source, that is like "0xff, 0x0, 0x10"

			// `strtok` can either be on `source` itself, but it will "ruin" it, thus not able to see the source later on
			// or make a temp copy, work on that copy, keeping the source intact
			char* temp = (char*) malloc(sizeof(char) * strlen(srcData) + 1);
			strcpy(temp, srcData);

			// This is to work on all types so make it generic
			void* data = malloc(entry->size);
			uint32_t i = 0;

			char* saveptr = NULL;
			char* dataI = strtok_r(temp, " \t,", &saveptr);
			while (dataI) {
				bool canEval = true;
				printf("Evaluating (%s)\n", dataI);
				uint32_t value = eval(dataI, symbTable, &canEval);

				// Make sure it is evaluated
				if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate %s!\n", dataI);

				// Depending on the type, make sure it conforms to size
				switch (entry->type)	{
					case 1: // byte
						if ((value & 0xff00UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for byte!\n", dataI);
						uint8_t* bytedata = (uint8_t*) data;
						bytedata[i] = (uint8_t) value;
						break;
					case 2: // halfwords
						if ((value & 0xff0000UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for halfword!\n", dataI);
						uint16_t* hworddata = (uint16_t*) data;
						hworddata[i] = (uint16_t) value;
						break;
					case 3: //words
						if ((value & 0xff00000000UL) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for word!\n", dataI);
						uint32_t* worddata = (uint32_t*) data;
						worddata[i] = value;
						break;
					default:
						break;
				}

				i++;
				dataI = strtok_r(NULL, " \t,", &saveptr);
			}

			entry->data._data = data;
			free(temp);
		}
	}

	// for (int i = 0; i < dataTable->bSize; i++) {
		
	// }
}


int main(int argc, char const* argv[]) {
	char* infile = NULL;
	char* outbin = "out.ar";
	int opt;

	while ((opt = getopt(argc, (char* const*) argv, "o:")) != -1) {
		switch (opt) {
			case 'o':
				outbin = optarg;
				break;
			case '?':
				if (optopt == 'o') fprintf(stderr, "Optionc -%c requires an argument.\n", optopt);
				else if (isprint(optopt)) fprintf(stderr, "Unknown option '-%c'.\n", optopt);
		}
	}

	const char* allowedExts[] = { ".s" };

	for (int i = optind; i < argc; i++) {
		// Checking input file ends in .s

		const char* dot = strrchr(argv[i], '.');
		if (!dot || dot == argv[i] || *(dot-1) == '/') continue;

		for (int j = 0; j < 1; j++) {
			if (strcmp(dot, allowedExts[j]) != 0) continue;

			infile = (char*) argv[i];
			break;
		}
	}

	if (!infile) {
		fprintf(stderr, "Input file is not a valid assembly file.\n");
		exit(-1);
	}

	FILE* source = fopen(infile, "r");
	if (!source) handleError(ERR_IO, FATAL, "Could not open %s!\n", infile);

	// PASS 1

	SymbolTable* symbTable = initSymbTable();
	InstructionStream* instrStream = initInstrStream();
	SectionTable* sectTable = initSectionTable();
	DataTable* dataTable = initDataTable();

	// Expressions can use the LP (@)
	// Instead of passing it to eval, treat it as a (variable) label/symbol
	// so eval can use it should it come up
	symb_entry_t* lpEntry = initSymbEntry("@", NULL, 0, CREATE_FLAGS(0,0,0,0,1,1));
	addSymbEntry(symbTable, lpEntry);

	char* line = NULL;
	size_t n;

	ssize_t read = getline(&line, &n, source);
	while (read != -1) {
		char* cleanLine = preprocess(line, read);

		// cleanLine does not have any leading or trailing whitespace, or comments
		// if null, it means it did not contain any useful stuff

		if (cleanLine) {
			printf("CURRENT WORKING SECTION: ");
			if (sectTable->activeSection == 0) printf("DATA\n");
			else if (sectTable->activeSection == 1) printf("CONST\n");
			else if (sectTable->activeSection == 2) printf("BSS\n");
			else if (sectTable->activeSection == 3) printf("TEXT\n");
			else printf("UNRECOGNIZED\n");

			lpEntry->value = sectTable->entries[sectTable->activeSection].lp;
			lpEntry->flags = CREATE_FLAGS(0,sectTable->activeSection,0,0,1,1);

			// First token is very important
			char* saveptr = NULL;
			char* tok = strtok_r(cleanLine, " \t", &saveptr);
			size_t tokLen = strlen(tok);

			printf("Working on (%s) (%s)\n", tok, saveptr);

			// bool seqDirective
			if (*(tok+tokLen-1) == ':') {
				*(tok+tokLen-1) = '\0'; // Take off ':'
				handleLabel(symbTable, sectTable, &tok, &saveptr);
			}

			if (tok) {
				if (*tok == '.') handleDirective(symbTable, sectTable, dataTable, tok+1, saveptr);
				else { // Assume it is an instruction
					// Since it is assuming the line is an instruction, ensure the current section is text
					if (sectTable->activeSection != 3) handleError(ERR_INSTR_NOT_IN_TEXT, FATAL, "Instruction `%s %s` is not in the text section!\n", tok, saveptr);
					// Also ensure LP is aligned
					if (sectTable->entries[3].lp % 4 != 0) handleError(ERR_MISALIGNMENT, FATAL, "Instruction not aligned!\n");

					// tokenize, parse, and eval (if needed) instruction
					handleInstruction(instrStream, symbTable, sectTable, tok, saveptr);

					// Increment LP by instruction size
					sectTable->entries[3].lp += 4;
				}
			}
		}

		read = getline(&line, &n, source);
	}
	fclose(source);

	sectTable->entries[0].size = sectTable->entries[0].lp;
	sectTable->entries[1].size = sectTable->entries[1].lp;
	sectTable->entries[2].size = sectTable->entries[2].lp;
	sectTable->entries[3].size = sectTable->entries[3].lp;

	printf("\n");
	displaySymbTable(symbTable);
	displayDataTable(dataTable);
	displaySectionTable(sectTable);
	displayInstrStream(instrStream);
	printf("\n");
	
	// Confirms existence of text section and _init label (and marked global) at text, undefined (but referenced) symbols

	if (!sectTable->entries[3].present) handleError(ERR_NO_TEXT, FATAL, "Text section has not been defined!\n");

	symb_entry_t* initLabel = getSymbEntry(symbTable, "_init");
	if (!initLabel) handleError(ERR_NO_ENTRY, FATAL, "Entry point '_init' not found!\n");
	if (GET_LOCALITY(initLabel->flags) == 0b0) handleError(ERR_ENTRY_NOT_GLOBAL, FATAL, "Entry point '_init' is not global!\n");
	if (GET_SECTION(initLabel->flags) != 0b11) handleError(ERR_NO_ENTRY, FATAL, "Entry point is not in text section!\n");

	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];

		if (GET_DEFINED(entry->flags) == 0b0) handleError(ERR_UNDEFINED_SYMBOL, FATAL, "Symbol %s is referenced but undefined!\n", entry->name);
		// Add warning on defined but unreferenced
	}

	// PASS 2

	resolveSymbols(symbTable);
	completeData(dataTable, symbTable);

	



	deleteSymbTable(symbTable);
	deleteInstrStream(instrStream);
	deleteSectionTable(sectTable);
	deleteDataTable(dataTable);

	return 0;
}