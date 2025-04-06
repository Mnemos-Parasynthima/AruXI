#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <strings.h>

#include "lexer-parser.h"
#include "assemblerError.h"
#include "evaluator.h"


enum DirectiveIndex {
	DATA,
	CONST,
	BSS,
	TEXT,
	SET,
	GLOB,
	END,
	STRING,
	BYTE,
	HWORD,
	WORD,
	FLOAT,
	ZERO,
	FILL,
	ALIGN
};

#define TOLOWER(str) for (; *str; ++str) *str = tolower(*str)
#define TOUPPER(str) for (; *str; ++str) *str = toupper(*str)


static int validateDirective(char* directive) {
	int size = sizeof(VALID_DIRECTIVES) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		// printf("Comparing directive %s to valid directive %s\n", directive, VALID_DIRECTIVES[i]);
		if (strcmp(VALID_DIRECTIVES[i], directive) == 0) {
			// printf("Equal directive at %d\n", i);
			return i;
		}
	}

	handleError(ERR_DIRECTIVE_NOT_FOUND, FATAL, "Directive %s is not valid!\n", directive);
}

static void validateSection(enum DirectiveIndex directiveType, uint8_t activeSection) {
	// Valid types are found in the directives table, last column
	switch (directiveType)	{
		case STRING: case BYTE: case HWORD: case WORD: case FLOAT: case FILL:
			if (activeSection == 2) {
				handleError(ERR_DIRECTIVE_NOT_ALLOWED, FATAL, "Directive .%s is not allowed for bss section!\n", VALID_DIRECTIVES[directiveType]);
			}
			break;
		default: break;
	}
}

/**
 * `.set` sets the value of an expression to a symbol. Then inserting in the symbol table.
 * If the symbol has been referenced before, it simply updates it. Additionally, if the expression cannot be evaluated,
 * it remains using `expr` until it can be evaluated (which might not after pass 2).
 * @param symbTable The symbol table
 * @param args The arguments for .set
 */
static void setDirective(SymbolTable* symbTable, char* args, uint8_t activeSection) {
	// args only provides all other tokens/strings after .set
	// ensure at least two left: the symbol and an expression
	char* symbol = strtok_r(NULL, " \t,", &args);
	char* expr = strtok_r(NULL, " \t", &args);

	if (!expr) handleError(ERR_INVALID_SYNTAX, FATAL, "Operands for set %s not found!\n", symbol);

	// Must be checked to ensure propery synax of .set [symb], [expr]
	
	// .set as,2
	// .set as, 2
	// .set as	,	2
	// .set as	,2

	// .set as,
	// .set as	,
	// .set as

	// if (*comma != ',') handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid syntax for set directive on symbol %s\n", symbol);

	// Make sure the symbol has not been set/defined before
	// It is okay than an entry has been created as it could have been referenced earlier
	symb_entry_t* entry = getSymbEntry(symbTable, symbol);
	if (entry && GET_DEFINED(entry->flags) == 0b1) handleError(ERR_REDEFINED, FATAL, "Symbol %s has already been defined!\n", symbol);

	bool canEval = false;
	int32_t res = eval(expr, symbTable, &canEval);
	if (!canEval) { 
		// The expression could not be evaluated because it references an undefined (as of now) symbol
		// Note that res cannot be used as an indicator as the expression can eval to a negative number
		
		// If this symbol was referenced before, an entry exists, update it
		if (entry) {
			entry->expr = expr;
			// SET_EXPRESSION(entry->flags);
			entry->flags |= (1 << 7); // ensure expression flag is set to 1

			uint32_t mask = ~(0b11 << 4); // update section defined in 
			entry->flags &= mask;
			entry->flags |= (activeSection << 4);

			entry->flags |= (1 << 3); // Type is naturally set to 1 since this is .set
			// Locality is only changed by .glob, everything defaults to 0
			// Reference is only changed by non-(label ids and .set)
			entry->flags |= (1 << 0); // Defined is set since it technically is defined, excepts it is in expr form
			// However, if expr contains an undefined symbol at the end, then the expr will remain as expr
			// And this symbol will remain being in expr form
		} else {
			entry = initSymbEntry(symbol, expr, 0, CREATE_FLAGS(1, activeSection, 1, 0, 0, 1));
			addSymbEntry(symbTable, entry);
		}
	} else {
		// It has been evaluated, update the entry or create a new one
		if (entry) {
			entry->expr = NULL;
			entry->value = res;
			SET_EXPRESSION(entry->flags); // ensure expression flag is set to 0 since it no longer holds one

			uint32_t mask = ~(0b11 << 4); // update section defined in 
			entry->flags &= mask;
			entry->flags |= (activeSection << 4);

			entry->flags |= (1 << 3); // Type is naturally set to 1 since this is .set
			// Locality is only changed by .glob, everything defaults to 0
			// Reference is only changed by non-(label ids and .set)
			entry->flags |= (1 << 0); // Defined is naturally set to 1 since this is .set (defines)
		} else { // New entry
			entry = initSymbEntry(symbol, NULL, res, CREATE_FLAGS(0, activeSection, 1, 0, 0, 1));
			addSymbEntry(symbTable, entry);
		}
	}	
}

/**
 * Sets the symbol found in args to global status.
 * @param SymbolTable 
 * @param args 
 */
static void globDirective(SymbolTable* symbTable, char* args, uint8_t activeSection) {
	printf("Setting global %s\n", args);

	// args only provides all other tokens/strings after .glob
	// ensure only one token left: the symbol
	char* symbol = strtok_r(NULL, " \t", &args);
	char* nullTok = strtok_r(NULL, " \t", &args);
	if (nullTok) handleError(ERR_INVALID_SYNTAX, FATAL, "More than one argument supplied to `.glob`: %s", nullTok);

	// Symbol may either already be present in table or not
	symb_entry_t* entry = getSymbEntry(symbTable, symbol);

	if (entry) { // entry exists, update locality
		SET_LOCALITY(entry->flags);
	} else { //entry does not exist, new entry
		uint32_t flags = CREATE_FLAGS(0, activeSection, 0, 1, 1, 0);

		entry = initSymbEntry(symbol, NULL, 0, flags);
		addSymbEntry(symbTable, entry);
	}
}

static void stringDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(STRING, sectTable->activeSection);

	// args should be the string itself
	char* str = args;
	size_t strLen = strlen(str);

	char* data = (char*) malloc(sizeof(char) * strLen + 1);
	if (!data) handleError(ERR_MEM, FATAL, "Could not allocate memory for string data!\n");
	strcpy(data, str);

	data_entry_t* entry = initDataEntry(0, sectTable->entries[sectTable->activeSection].lp, strLen+1, str, data);
	sectTable->entries[sectTable->activeSection].lp += (strLen+1);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

static void byteDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(BYTE, sectTable->activeSection);

	// args is in the form expr{, *exprs}
	// However, an expression may contain a label which may not be defined yet
	// To ease things, evaluation is done in the second pass
	// For now, *data is set to null and size will be quickly calculated to determine how many exprs to eval

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	// Calculate how many expressions there are
	// Entry size will be count * 1 (assuming each expression results in a byte)
	int count = 0;
	char* tok = strtok(temp, ",");
	while (tok) {
		count++;
		tok = strtok(NULL, ",");
	}
	free(temp);

	data_entry_t* entry = initDataEntry(1, sectTable->entries[sectTable->activeSection].lp, count * 1, args, NULL);
	sectTable->entries[sectTable->activeSection].lp += (count*1);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

static void hwordDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(HWORD, sectTable->activeSection);

	// Same procedure as in byteDirective

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	// Calculate how many expressions there are
	// Entry size will be count * 2 (assuming each expression results in a halfword)
	int count = 0;
	char* tok = strtok(temp, ",");
	while (tok) {
		count++;
		tok = strtok(NULL, ",");
	}
	free(temp);

	data_entry_t* entry = initDataEntry(2, sectTable->entries[sectTable->activeSection].lp, count * 2, args, NULL);
	sectTable->entries[sectTable->activeSection].lp += (count*2);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

static void wordDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(WORD, sectTable->activeSection);

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	// Calculate how many expressions there are
	// Entry size will be count * 4 (assuming each expression results in a word)
	int count = 0;
	char* tok = strtok(temp, ",");
	while (tok) {
		count++;
		tok = strtok(NULL, ",");
	}
	free(temp);

	data_entry_t* entry = initDataEntry(3, sectTable->entries[sectTable->activeSection].lp, count * 4, args, NULL);
	sectTable->entries[sectTable->activeSection].lp += (count*4);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

static void floatDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(FLOAT, sectTable->activeSection);

	// args is in the form float{, *floats}
	// Go through each and convert to float

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	float* data = (float*) malloc(sizeof(float) * 5);
	if (!data) handleError(ERR_MEM, FATAL, "Could not allocate memory for float data!\n");
	int dataLen = 0;
	int dataCap = 5;

	char* tok = strtok(temp, " \t,");
	char* endptr = NULL;
	int i = 0;
	while (tok) {
		// Attempt to convert
		errno = 0;
		float _float = strtof(tok, &endptr);

		if (errno != 0) handleError(ERR_INVALID_EXPRESSION, FATAL, "%s\n", strerror(errno));
		if (endptr == tok) handleError(ERR_INVALID_EXPRESSION, FATAL, "No digits found!\n");
		if (*endptr != '\0') handleError(ERR_INVALID_EXPRESSION, WARNING, "String %s is not a complete valid float!\n", tok);

		// Complete valid float, add to array

		if (dataLen == dataCap) {
			dataCap += 5;
			float* temp = realloc(data, dataCap);
			if (!temp) handleError(ERR_MEM, FATAL, "Could not reallocate for new float array!\n");
		}

		data[i] = _float;
		dataLen++;

		tok = strtok(NULL, " \t,");
	}
	free(temp);

	data_entry_t* entry = initDataEntry(4, sectTable->entries[sectTable->activeSection].lp, dataLen * 32, args, data);
	sectTable->entries[sectTable->activeSection].lp += (dataLen*32);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

// TODO
static void zeroDirective(SectionTable* sectTable, DataTable* dataTable, SymbolTable* symbTable, char* args) {
	validateSection(ZERO, sectTable->activeSection);

	// args is in the form expr, size
	// 

	// expr   			,  		 	size ->> expr\0,  		 	size
	// expr,		size ->> expr,\0size
	// expr   	, size ->> expr\0, size
	// expr	 	  , ->> expr\0,
	// expr, ->> expr,\0
	// expr ->> expr\0

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	// Get the first arg, that is, the expression resulting in how many zeros
	char* expr = strtok(temp, " \t");
	if (!expr) handleError(ERR_INVALID_SYNTAX, FATAL, "expr not found in %s! Syntax is `expr, size`.\n", args);

	// Ge the second arg, that is, the size
	char* size = strtok(NULL, " \t");

	void* data = NULL;
	bool canEval = false;
	int32_t len = eval(expr, symbTable, &canEval);
	// If it cannot be evaluated, leave it NULL and for pass 2 to complete it
	if (canEval) {

	}

	// 

	// data_entry_t* entry = initDataEntry(type, sectTable->entries[sectTable->activeSection].lp, size, args, data);
	// sectTable->entries[sectTable->activeSection].lp += size;
	// addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

// TODO
static void fillDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	
}

// TODO
static void alignDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {}

void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* args) {
	printf("\tHandling directive (%s) with args (%s)\n", directive, args);
	
	char* temp = directive;
	TOLOWER(temp);
	int index = validateDirective(directive);

	switch (index)	{
		case DATA: sectTable->activeSection = 0; sectTable->entries[0].present = true; break;
		case CONST: sectTable->activeSection = 1; sectTable->entries[1].present = true; break;
		case BSS: sectTable->activeSection = 2; sectTable->entries[2].present = true; break;
		case TEXT: sectTable->activeSection = 3; sectTable->entries[3].present = true; break;

		case SET: setDirective(symbTable, args, sectTable->activeSection); break;
		case GLOB: globDirective(symbTable, args, sectTable->activeSection); break;

		// case END:

		case STRING: stringDirective(sectTable, dataTable, args); break;
		case BYTE: byteDirective(sectTable, dataTable, args); break;
		case HWORD: hwordDirective(sectTable, dataTable, args); break;
		case WORD: wordDirective(sectTable, dataTable, args); break;
		case FLOAT: floatDirective(sectTable, dataTable, args); break;
		// case ZERO: zeroDirective(sectTable, dataTable, symbTable, args); break;
		// case FILL: fillDirective(sectTable, dataTable, args); break;

		// case ALIGN: alignDirective()

		default:
			break;
	}
}

static void validateLabel(char* label) {
	// Make sure initial character is valid
	// if (isalpha(*label)) printf("isalpha(%c) true\n", *label);
	if (!isalpha(*label) && *label != '_') handleError(ERR_INVALID_LABEL, FATAL, "Label %s cannot begin with `%c`!\n", label, *label);

	// Make sure it is not the name of an directive, instruction, or register
	bool invalid = false;
	char* temp = NULL;
	int size = sizeof(VALID_DIRECTIVES) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(label, VALID_DIRECTIVES[i]) == 0) handleError(ERR_INVALID_LABEL, FATAL, "Label %s cannot be a directive!\n", label);
	}

	size = sizeof(VALID_INSTRUCTIONS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(label, VALID_INSTRUCTIONS[i]) == 0) handleError(ERR_INVALID_LABEL, FATAL, "Label %s cannot be a instruction!\n", label);
	}

	size = sizeof(VALID_REGISTERS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(label, VALID_REGISTERS[i]) == 0) handleError(ERR_INVALID_LABEL, FATAL, "Label %s cannot be a register!\n", label);
	}
}

void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** save) {
	printf("\tHandling label (%s)\n", *tok);
	printf("\tRemaining: (%s)\n", *save);
	// Possible states could be:
	// [label]:\n
	// [label]: .[directive] [...operands]\n
	// [label]: [instruction]\n

	char* label = *tok;

	// Make sure label is valid
	validateLabel(label);

	symb_entry_t* entry = getSymbEntry(symbTable, label);
	if (entry) { // Update entry
		printf("Label exists. Updating record.\n");
		entry->value = sectTable->entries[sectTable->activeSection].lp;
		
		entry->flags |= (0 << 7); // ensure expression flag is set to 0

		uint32_t mask = ~(0b11 << 4); // update section defined in 
		entry->flags &= mask;
		entry->flags |= (sectTable->activeSection << 4);

		entry->flags |= (0 << 3); // ensure type is set to 0
		// Locality is only changed by .glob, everything defaults to 0
		// Reference is only changed by non-(label ids and .set)
		entry->flags |= (1 << 0); // Defined is set since it is defined
	} else {
		printf("Label does not exist. Creating record.\n");
		entry = initSymbEntry(label, NULL, sectTable->entries[sectTable->activeSection].lp, CREATE_FLAGS(0, sectTable->activeSection, 0, 0, 0, 1));
		addSymbEntry(symbTable, entry);
	}

	// Depending on whether there is more (in save)
	// There might be a directive or instruction
	if (*save) *tok = strtok_r(NULL, " \t", save);
	else *tok = NULL; // Indicate nothing left for the line
}

static void validateInstruction(char* instr) {
	int size = sizeof(VALID_INSTRUCTIONS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcmp(VALID_INSTRUCTIONS[i], instr) == 0) handleError(ERR_INVALID_INSTRUCTION, FATAL, "Instruction %s is not valid!\n", instr);
	}
}

void handleInstruction(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* instr, char* args) {
	printf("\tHandling instruction (%s) with args (%s)\n", instr, args);

	TOLOWER(instr);
	// Make sure instr is valid
	validateInstruction(instr);


}