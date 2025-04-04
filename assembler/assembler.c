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

	// PASS 1
	SymbolTable* symbTable = initSymbTable();
	InstructionStream* instrStream = initInstrStream();
	SectionTable* sectTable = initSectionTable();
	DataTable* dataTable = initDataTable();
	

	char* line = NULL;
	size_t n;

	ssize_t read = getline(&line, &n, source);
	while (read != -1) {
		char* cleanLine = preprocess(line);

		// cleanLine does not have any leading or trailing whitespace, or comments
		// if null, it means it did not contain any useful stuff

		if (cleanLine) {
			// First token is very important
			char* saveptr;
			char* tok = strtok_r(line, " ", &saveptr);

			if (*tok == '.') handleDirective(symbTable, sectTable, dataTable, tok, saveptr);
			else if (*(saveptr-2) == ':') handleLabel(symbTable, sectTable, tok, saveptr);
			else { // Assume it is an instruction
				// Since it is assuming the line is an instruction, ensure the current section is text
				if (sectTable->activeSection != 0) handleError(ERR_INSTR_NOT_IN_TEXT, FATAL, "Instruction is not in the text section!\n");
				
				// tokenize, parse, and eval (if needed) instruction


				// Increment LP by instruction size
				sectTable->entries[0].lp += 4;
			}
		}

		read = getline(&line, &n, source);
	}

	// Confirms existance of text section and _init label (and marked global) at text, undefined (but referenced) symbols

	if (!sectTable->entries[0].present) handleError(ERR_NO_TEXT, FATAL, "Text section has not been defined!\n");

	symb_entry_t* initLabel = getSymbEntry(symbTable, "_init");
	if (!initLabel) handleError(ERR_NO_ENTRY, FATAL, "Entry point '_init' not found!\n");
	if (GET_LOCALITY(initLabel->flags) == 0b0) handleError(ERR_ENTRY_NOT_GLOBAL, FATAL, "Entry point '_init' is not global!\n");
	if (GET_SECTION(initLabel->flags) != 0b11) handleError(ERR_NO_ENTRY, FATAL, "Entry point is not in text section!\n");

	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = &symbTable->entries[i];

		if (GET_DEFINED(entry->flags) == 0b0) handleError(ERR_UNDEFINED_SYMBOL, FATAL, "Symbol %s is referenced but undefined!\n", entry->name);
	}

	// PASS 2







	deleteSymbTable(symbTable);
	deleteInstrStream(instrStream);
	deleteSectionTable(sectTable);
	deleteDataTable(dataTable);
	fclose(source);

	return 0;
}