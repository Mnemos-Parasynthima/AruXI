#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN64
#include <unistd.h>
#else
#include "unistd-legacy.h"
#endif
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include "SectionTable-legacy.h"
#include "InstructionStream-legacy.h"
#include "DataTable-legacy.h"
#include "SymbolTable-legacy.h"
#include "assemblerError-legacy.h"
#include "preprocessor-legacy.h"
#include "lexer-parser-legacy.h"
#include "evaluator-legacy.h"
#include "encoder-legacy.h"
#include "generator-legacy.h"


bool halt; // To stop assembling in case of `.end`
bool kern = false; // To indicate whether an executable or kernel

static void resolveSymbols(SymbolTable* symbTable) {
	// printf("Resolving symbols!\n");

	for (int i = 0; i < symbTable->size; i++) {
		symb_entry_t* entry = symbTable->entries[i];
		// Resolve when an expression
		if (GET_EXPRESSION(entry->flags) == 0b1) {
			char* expr = entry->expr;
			// printf("Resolving (%s)\n", expr);

			bool canEval = true;
			int32_t value = eval(expr, symbTable, &canEval);
			// printf("Resolved to %d\n", value);

			// canEval needs to be true always
			if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate symbol %s for the last pass!\n", entry->name);

			SET_EXPRESSION(entry->flags);
			entry->value = value;
		}
	}
}


static void _completeData(data_entry_t* entry, DataTable* dataTable, SymbolTable* symbTable) {
	char* srcData = entry->source;
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
		// printf("Evaluating (%s)\n", dataI);
		bool canEval = true;
		uint32_t value = eval(dataI, symbTable, &canEval);

		// Make sure it is evaluated
		if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate %s!\n", dataI);

		// Depending on the type, make sure it conforms to size
		switch (entry->type)	{
			case 1: // byte
				if ((value & 0xff00L) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for byte!\n", dataI);
				uint8_t* bytedata = (uint8_t*) data;
				bytedata[i] = (uint8_t) value;
				break;
			case 2: // halfwords
				if ((value & 0xff0000L) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for halfword!\n", dataI);
				uint16_t* hworddata = (uint16_t*) data;
				hworddata[i] = (uint16_t) value;
				break;
			case 3: //words
				if ((value & 0xff00000000L) != 0x00) handleError(ERR_INVALID_SIZE, FATAL, "Expression %s exceeds allowed size for word!\n", dataI);
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

static void completeData(DataTable* dataTable, SymbolTable* symbTable) {
	// .byte, .hword, and .word are the only ones that left `data` blank
	// placed in `source`
	// To determine if the data entry is to be completed, either `data` can be checked if NULL
	// or `type`

	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		if (!entry->data._data) _completeData(entry, dataTable, symbTable);
	}

	for (int i = 0; i < dataTable->cSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		if (!entry->data._data) _completeData(entry, dataTable, symbTable);
	}

	// Currently, only .zero is available for bss, and no need to complete data
	// for (int i = 0; i < dataTable->bSize; i++) {}

	for (int i = 0; i < dataTable->eSize; i++) {
		data_entry_t* entry = dataTable->evtEntries[i];

		if (!entry->data._data) _completeData(entry, dataTable, symbTable);
	}
}

static void clearHash(char* expr) {
	// Using hash indicator for purposes of printing an error so it doesn't get printed multiple times for the same expr
	// However, don't print it if expr is an atomic number
	bool hash = false;

	if (strspn(expr, "+-*/|&^<>") == 0) hash = true;

	char* tmp = expr;
	while (*tmp != '\0') {
		if (*tmp == '#') {
			if (!hash) handleError(WARN, WARNING, "'#' prefix in expression %s ignored.\n", expr);

			*tmp = ' ';
			hash = true;
		}

		tmp++;
	}
}

static void fixInstrs(InstructionStream* instrStream, SymbolTable* symbTable) {
	/** 
	 * This function just does the rest regarding IR instructions as described in `handleIR`
	 * The end goal of this function is:
	 * `.encoding` is to either be 0x0 or 0x1 indicating I- or R-type
	 * syntax checked
	 * `.operands` is to have the proper layout with XZ as appropriate:
	 * 		operands = { xd, xs, xr_or_imm, NULL }
	 */


	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instrObj = instrStream->instructions[i];
		// Skip non-IR instrs
		if (instrObj->encoding != 0x10) continue;

		char* instr = instrObj->instr;
		char** operands = instrObj->operands;

		// Make sure total length is 4
		if (operands[3] != NULL) handleError(ERR_INNER, FATAL, "Expected operands[3] to be null, found %s\n", operands[3]);

		// Immediates can either exist in index 1 or 2 depending on the instruction (index 1 for not, mv, mvn)
		// cmp has its operands adjusted already, so imm in index 2
		if (*instr == 'n' || *instr == 'm') {
			char* immQ = operands[1];
			// immQ for 'immediate?'

			if (isValidRegister(immQ)) {
				instrObj->encoding = 0x1;

				// Adjust locations
				// mvn doesn't have first source, not and mv as is
				if (*instr == 'm' && *(instr+2) == 'n') {
					operands[2] = operands[1];
					operands[1] = strdup("xz");
				} else operands[2] = strdup("xz");
			} else {
				instrObj->encoding = 0x0;
				// Note that expressions (or atomics) might or might not contain '#'
				// Allow prescence of '#' but clear it out for eval purposes
				clearHash(immQ);

				// Adjust locations
				// not, mv, and mvn don't have a source, only destination and immediate
				if (*instr != 'c') {
					operands[2] = operands[1];
					operands[1] = strdup("xz");
				}
			}
		} else { // possible immediate is in 2, aka well-formed (xd, xs, xr_imm)
			char* immQ = operands[2];

			validateRegister(operands[0]);
			validateRegister(operands[1]);

			if (isValidRegister(immQ)) {
				instrObj->encoding = 0x1;
			} else {
				instrObj->encoding = 0x0;
				clearHash(immQ);
			}
		}
	}
}


int main(int argc, char const* argv[]) {
	char* infile = NULL;
	char* outbin = "out.ar";
	int opt;

	while ((opt = getopt(argc, (char* const*) argv, "o:k")) != -1) {
		switch (opt) {
			case 'o':
				outbin = optarg;
				break;
			case 'k':
				kern = true;
				break;
			case '?':
				if (optopt == 'o') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
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

	// For now, program is loaded at a fixed address, refer to documentation on Process Address Space
	// Space for kernel code is different
	uint32_t dataStart, constStart, bssStart, textStart;
	uint32_t evtStart, ivtStart;

	if (kern) {
		evtStart = 0x00040000;
		ivtStart = 0x0;
		dataStart = 0xA0080000;
		constStart = 0xA0080000;
		bssStart = 0xA0080000;
		textStart = 0xB8080000;
	} else {
		dataStart = 0x20090000;
		constStart = 0x20090000;
		bssStart = 0x20040000;
		textStart = 0x20190000;
	}

	// PASS 1

	SymbolTable* symbTable = initSymbTable();
	InstructionStream* instrStream = initInstrStream();
	SectionTable* sectTable = initSectionTable();
	DataTable* dataTable = initDataTable();

	sectTable->entries[0].lp = dataStart;
	sectTable->entries[1].lp = constStart;
	sectTable->entries[2].lp = bssStart;
	sectTable->entries[3].lp = textStart;
	sectTable->entries[4].lp = evtStart;
	sectTable->entries[5].lp = ivtStart;

	// Expressions can use the LP (@)
	// Instead of passing it to eval, treat it as a (variable) label/symbol
	// so eval can use it should it come up
	symb_entry_t* lpEntry = initSymbEntry("@", NULL, 0, CREATE_FLAGS(0,0,0,0,1,1));
	addSymbEntry(symbTable, lpEntry);

	char* line = NULL;
	size_t n;

	halt = false;

	// EVT/IVT can only be declared once, that means no `.evt` ... `.data` ... `.evt`
	// bool evtDefined = false;
	char evtDefined = 0x00; // upper nibble marks start, lower nibble marks end
	// bool ivtDefined = false;
	// This is also used to mark off instructions that are in the evt/ivt


	ssize_t read = getline(&line, &n, source);
	while (read != -1) {
		char* cleanLine = preprocess(line, read);

		// cleanLine does not have any leading or trailing whitespace, or comments
		// if null, it means it did not contain any useful stuff

		if (cleanLine) {
			debug("CURRENT WORKING SECTION: ");
			if (sectTable->activeSection == 0) debug("DATA\n");
			else if (sectTable->activeSection == 1) debug("CONST\n");
			else if (sectTable->activeSection == 2) debug("BSS\n");
			else if (sectTable->activeSection == 3) debug("TEXT\n");
			else if (sectTable->activeSection == 4) debug("EVT\n");
			else if (sectTable->activeSection == 5) debug("IVT\n");
			else debug("UNRECOGNIZED\n");

			lpEntry->value = sectTable->entries[sectTable->activeSection].lp;
			lpEntry->flags = CREATE_FLAGS(0,sectTable->activeSection,0,0,1,1);

			// First token is very important
			char* saveptr = NULL;
			char* tok = strtok_r(cleanLine, " \t", &saveptr);
			size_t tokLen = strlen(tok);

			debug("Working on (%s) (%s)\n", tok, saveptr);

			// bool seqDirective
			if (*(tok+tokLen-1) == ':') {
				*(tok+tokLen-1) = '\0'; // Take off ':'
				handleLabel(symbTable, sectTable, &tok, &saveptr);
			}

			if (tok) {
				if (*tok == '.') handleDirective(symbTable, sectTable, dataTable, tok+1, saveptr);
				else { // Assume it is an instruction
					// Since it is assuming the line is an instruction, ensure the current section is text or ivt/evt
					if (sectTable->activeSection < 3) handleError(ERR_INSTR_NOT_IN_TEXT, FATAL, "Instruction `%s %s` is not in the text/evt/ivt section!\n", tok, saveptr);
					// Also ensure LP is aligned
					if (sectTable->entries[3].lp % 4 != 0) handleError(ERR_MISALIGNMENT, FATAL, "Instruction not aligned!\n");

					// tokenize, parse, and eval (if needed) instruction
					handleInstruction(instrStream, symbTable, sectTable, tok, saveptr);

					// Increment LP by instruction size
					int sectIdx = 3;
					if ((evtDefined & 0xff) == 0xf0) sectIdx = 4;
					sectTable->entries[sectIdx].lp += 4;
				}

				if (halt) break;
			}

			// Disallow ivt/evt for non-kernel code
			if (!kern && (sectTable->activeSection == 4 || sectTable->activeSection == 5)) {
				handleError(ERR_DIRECTIVE_NOT_ALLOWED, FATAL, "Cannot use EVT or IVT in non-kernel code!\n");
			}

			// Disallow reusage of EVT
			if (sectTable->activeSection == 4 && ((evtDefined & 0xff) == 0xff)) {
				handleError(ERR_DIRECTIVE_NOT_ALLOWED, FATAL, "EVT cannot be declared more than once!\n");
			}

			if (sectTable->activeSection == 4 && ((evtDefined & 0xff) == 0x00)) {
				// The current section is EVT and it hasn't been defined before
				evtDefined = 0xf0;
				// Also place a nonsense instruction indicating following instructions are for evt

				char* _null[] = {NULL};
				instr_obj_t* marker = initInstrObj(0x00000000, NULL, "__EvtStart__", (char**) &_null);
				marker->encoding = 0xff;
				addInstrObj(instrStream, marker);
			}

			if (sectTable->activeSection != 4 && ((evtDefined & 0xff) == 0xf0)) {
				// Just changed sections, also mark it off
				evtDefined = 0xff;
				char* __null_[] = {NULL};
				instr_obj_t* marker = initInstrObj(0x00000000, NULL, "__EvtEnd__", (char**) &__null_);
				marker->encoding = 0xff;
				addInstrObj(instrStream, marker);
			}
		}

		free(cleanLine);
		read = getline(&line, &n, source);
	}
	fclose(source);

	// If the code ends with the EVT, no __EvtEnd__ is placed, add it manually
	if ((evtDefined & 0xff) == 0xf0) {
		evtDefined = 0xff;
		char* __null_[] = {NULL};
		instr_obj_t* marker = initInstrObj(0x00000000, NULL, "__EvtEnd__", (char**) &__null_);
		marker->encoding = 0xff;
		addInstrObj(instrStream, marker);
	}

	sectTable->entries[0].size = sectTable->entries[0].lp - dataStart;
	sectTable->entries[1].size = sectTable->entries[1].lp - constStart;
	sectTable->entries[2].size = sectTable->entries[2].lp - bssStart;
	sectTable->entries[3].size = sectTable->entries[3].lp - textStart;
	sectTable->entries[4].size = sectTable->entries[4].lp - evtStart;
	sectTable->entries[5].size = sectTable->entries[5].lp - ivtStart;

	// Even though sections may be declared, if nothing is contained, might as well pretend they never existed
	for (int i = 0; i < 6; i++) {
		if (sectTable->entries[i].size == 0) sectTable->entries[i].present = false;
	}

	debug("\n");
	displaySymbTable(symbTable);
	displayDataTable(dataTable);
	displaySectionTable(sectTable);
	// displayInstrStream(instrStream, false);
	debug("\n");
	
	// Confirms existence of text section and _init label (and marked global) at text, undefined (but referenced) symbols
	// Also make sure EVT/IVT exists in kernel code

	if (!sectTable->entries[3].present) handleError(ERR_NO_TEXT, FATAL, "Text section has not been defined!\n");

	if (kern && ((evtDefined & 0xff) == 0x00)) { // || !ivtDefined)) {
		handleError(ERR_NO_VT, FATAL, "No IVT or EVT defined for kernel code!\n");
	}

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
	fixInstrs(instrStream, symbTable);

	encode(instrStream, symbTable);

	debug("\n");
	displayInstrStream(instrStream, true);

	AOEFbin* bin = generateBinary(instrStream, symbTable, dataTable, sectTable);

	writeBinary(bin, outbin);

	deleteSymbTable(symbTable);
	deleteInstrStream(instrStream);
	deleteSectionTable(sectTable);
	deleteDataTable(dataTable);
	free(line);

	return 0;
}