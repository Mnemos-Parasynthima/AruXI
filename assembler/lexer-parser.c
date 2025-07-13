#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <strings.h>

#include "lexer-parser.h"
#include "constants.h"
#include "assemblerError.h"
#include "evaluator.h"


extern bool halt;

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
	return -1;
}

static void validateSection(enum DirectiveIndex directiveType, uint8_t activeSection) {
	// Valid types are found in the directives table, last column
	switch (directiveType)	{
		case STRING: case BYTE: case HWORD: case WORD: case FLOAT: case FILL:
			if (activeSection == 2) {
				handleError(ERR_DIRECTIVE_NOT_ALLOWED, FATAL, "Directive .%s is not allowed for bss section!\n", VALID_DIRECTIVES[directiveType]);
			} else if (activeSection == 3) {
				handleError(ERR_DIRECTIVE_NOT_ALLOWED, FATAL, "Directive .%s is not allowed for text section!\n", VALID_DIRECTIVES[directiveType]);
			}
			break;
		case ZERO:
			if (activeSection != 2) handleError(WARN, WARNING, "Consider using .zero in bss!\n");
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
	// Ensure no redefinition of reserved keywords
	// checkReserve(symbol);

	char* expr = strtok_r(NULL, ",", &args);

	if (!expr) handleError(ERR_INVALID_SYNTAX, FATAL, "Expression for set %s not found!\n", symbol);

	// Must be checked to ensure propery synax of .set [symb], [expr]
	
	// Allow:
	// .set as,2
	// .set as, 2
	// .set as	,	2
	// .set as	,2

	// .set as,
	// .set as	,
	// .set as

	// if (*comma != ',') handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid syntax for set directive on symbol %s\n", symbol);
	char* nnull = strtok_r(NULL, ",", &args);
	if (nnull) handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid syntax for set directive on symbol %s, found '%s'\n", symbol, nnull);

	// Make sure the symbol has not been set/defined before
	// It is okay than an entry has been created as it could have been referenced earlier
	symb_entry_t* entry = getSymbEntry(symbTable, symbol);
	if (entry && GET_DEFINED(entry->flags) == 0b1) handleError(ERR_REDEFINED, FATAL, "Symbol %s has already been defined!\n", symbol);

	bool canEval = true;
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
	debug("Setting global %s\n", args);

	// args only provides all other tokens/strings after .glob
	// ensure only one token left: the symbol
	char* symbol = strtok_r(NULL, " \t", &args);
	char* nullTok = strtok_r(NULL, " \t", &args);
	if (nullTok) handleError(ERR_INVALID_SYNTAX, FATAL, "More than one argument supplied to `.glob`: %s", nullTok);

	// Symbol may either already be present in table or not
	symb_entry_t* entry = getSymbEntry(symbTable, symbol);

	if (entry) { // entry exists, update locality
		SET_LOCALITY(entry->flags);
		SET_REFERENCE(entry->flags);
	} else { //entry does not exist, new entry
		uint32_t flags = CREATE_FLAGS(0, activeSection, 0, 1, 1, 0);

		entry = initSymbEntry(symbol, NULL, 0, flags);
		addSymbEntry(symbTable, entry);
	}
}

static void endDirective(char* args) {
	char* rest = strtok_r(NULL, " \t,", &args);
	if (rest) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", rest);

	halt = true;
}

static void stringDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(STRING, sectTable->activeSection);

	// args should be the string itself including the two "
	char* str = args+1; // skip the first "
	size_t strLen = strlen(str);
	*(str + strLen - 1) = '\0'; // replace last "
	strLen--;	

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
		i++;

		tok = strtok(NULL, " \t,");
	}
	free(temp);
	data_entry_t* entry = initDataEntry(4, sectTable->entries[sectTable->activeSection].lp, dataLen * 4, args, data);
	sectTable->entries[sectTable->activeSection].lp += (dataLen*4);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
}

static void zeroDirective(SectionTable* sectTable, DataTable* dataTable, SymbolTable* symbTable, char* args) {
	validateSection(ZERO, sectTable->activeSection);

	size_t argsLen = strlen(args);
	char* temp = (char*) malloc(sizeof(char) * argsLen+1);
	if (!temp) handleError(ERR_MEM, FATAL, "");
	strcpy(temp, args);

	char* _size = strtok_r(NULL, " \t", &args);
	if (!_size) handleError(ERR_INVALID_SYNTAX, FATAL, "Expected `size`!\n");

	bool canEval = true;
	int32_t size = eval(_size, symbTable, &canEval);
	if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Expression must contain defined symbols!\n");

	uint8_t* data = NULL;
	if (sectTable->activeSection != 2) data = calloc(size, sizeof(uint8_t));

	data_entry_t* entry = initDataEntry(1, sectTable->entries[sectTable->activeSection].lp, size, temp, (void*)data);
	sectTable->entries[sectTable->activeSection].lp += size;
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
	free(temp);
}

static void fillDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	validateSection(FILL, sectTable->activeSection);

	char* lenTok = strtok_r(NULL, ",", &args);
	if (!lenTok) handleError(ERR_INVALID_SYNTAX, FATAL, "Length not found for fill!\n");

	char* numTok = strtok_r(NULL, " \t", &args);
	if (!numTok) handleError(ERR_INVALID_SYNTAX, FATAL, "Number not found for fill!\n");

	// For now, labels cannot be used for len
	// Maybe a solution later on

	bool canEval = true;
	uint32_t len = eval(lenTok, NULL, &canEval);
	if (!canEval) handleError(ERR_INVALID_SYNTAX, FATAL, "Cannot use labels for fill len: `%s`\n", lenTok);

	// Since fill creates bytes, do the same thing as the byte directive
	// Create artificial exprs
	size_t numTokLen = strlen(numTok);
	// numTokLen + 1 for "[numTok],", as in +1 takes care of ','
	// last +1 for null terminator
	char* temp = (char*) malloc(sizeof(char) * (numTokLen+1) * len + 1);
	if (!temp) handleError(ERR_MEM, FATAL, "");

	// Need to create it such that `"[_num],[_num],[_num],..."`

	char buffer[numTokLen+2];
	sprintf(buffer, "%s,", numTok);
	strcpy(temp, buffer);
	for (int i = 0; i < len-1; i++) {
		strcat(temp, buffer);
	}

	data_entry_t* entry = initDataEntry(1, sectTable->entries[sectTable->activeSection].lp, len, temp, NULL);
	sectTable->entries[sectTable->activeSection].lp += (len*1);
	addDataEntry(dataTable, entry, (data_sect_type_t) sectTable->activeSection);
	free(temp);
}

// TODO
static void alignDirective(SectionTable* sectTable, DataTable* dataTable, char* args) {
	handleError(WARN, WARNING, ".align handling not implemented!\n");
}

void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* args) {
	debug("\tHandling directive (%s) with args (%s)\n", directive, args);

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

		case END: endDirective(args); break;

		case STRING: stringDirective(sectTable, dataTable, args); break;
		case BYTE: byteDirective(sectTable, dataTable, args); break;
		case HWORD: hwordDirective(sectTable, dataTable, args); break;
		case WORD: wordDirective(sectTable, dataTable, args); break;
		case FLOAT: floatDirective(sectTable, dataTable, args); break;
		case ZERO: zeroDirective(sectTable, dataTable, symbTable, args); break;
		case FILL: fillDirective(sectTable, dataTable, args); break;

		case ALIGN: alignDirective(sectTable, dataTable, args); break;

		default:
			break;
	}
}


static void validateLabel(char* label) {
	// Make sure initial character is valid
	// if (isalpha(*label)) printf("isalpha(%c) true\n", *label);
	if (!isalpha(*label) && *label != '_') handleError(ERR_INVALID_LABEL, FATAL, "Label %s cannot begin with `%c`!\n", label, *label);

	// Make sure it is not the name of an directive, instruction, or register
	// bool invalid = false;
	// char* temp = NULL;
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
	debug("\tHandling label (%s)\n", *tok);
	debug("\tRemaining: (%s)\n", *save);
	// Possible states could be:
	// [label]:\n
	// [label]: .[directive] [...operands]\n
	// [label]: [instruction]\n

	char* label = *tok;

	// Make sure label is valid
	validateLabel(label);

	symb_entry_t* entry = getSymbEntry(symbTable, label);
	if (entry) { // Update entry
		debug("Label exists. Updating record.\n");
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
		debug("Label does not exist. Creating record.\n");
		entry = initSymbEntry(label, NULL, sectTable->entries[sectTable->activeSection].lp, CREATE_FLAGS(0, sectTable->activeSection, 0, 0, 0, 1));
		addSymbEntry(symbTable, entry);
	}

	// Depending on whether there is more (in save)
	// There might be a directive or instruction
	if (*save) *tok = strtok_r(NULL, " \t", save);
	else *tok = NULL; // Indicate nothing left for the line
}


static int validateInstruction(char* instr) {
	// Quick hack, b{cond} is not detected normally due to {cond}
	// Easy way is to check if instr starts with 'b' as no other isntructions start with 'b' (as of now)
	// The rest can be checked if it's a valid condition by Bc
	if (*instr == 'b') return Bc_TYPE_IDX;


	int size = sizeof(VALID_INSTRUCTIONS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(VALID_INSTRUCTIONS[i], instr) == 0) return i;	
	}

	handleError(ERR_INVALID_INSTRUCTION, FATAL, "Instruction %s is not valid!\n", instr);
	return -1;
}

void validateRegister(char* reg) {
	int size = sizeof(VALID_REGISTERS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(VALID_REGISTERS[i], reg) == 0) return;
	}

	handleError(ERR_INVALID_REGISTER, FATAL, "Register %s is not a valid register!\n", reg);
}

bool isValidRegister(char* reg) {
	int size = sizeof(VALID_REGISTERS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(VALID_REGISTERS[i], reg) == 0) return true;
	}

	return false;
}

void validateImmediate(char* imm, enum ImmediateSize immSize) {
	if (*imm != '#') handleError(ERR_INVALID_SYNTAX, FATAL, "Immediate %s does not start with '#'!\n", imm);

	// There should be something next to #
	if (*(imm+1) == '\0') handleError(ERR_INVALID_SYNTAX, FATAL, "No immediate found!\n");

	// That something needs to be a valid imm

	// Make sure imm is either hex, bin, or decimal
	// Make sure the sizes are appropriate
}

#define HANDLE_INSTR(name) static void name (InstructionStream* instrStream, SymbolTable* symbTable, SectionTable* sectTable, char* instr, char* args)

HANDLE_INSTR(handleI) {
	// I-Types typically have it in the form `[instr] <xd>, <xs>, #<imm>`
	// But nop has none, although an alias of add xz, xz, #0

	char* xd = NULL;
	char* xs = NULL;
	char* imm = NULL;

	bool isNOP = (strcmp(instr, VALID_INSTRUCTIONS[NOP]) == 0);

	if (isNOP) {
		xd = VALID_REGISTERS[XZ];
		xs = VALID_REGISTERS[XZ];
		imm = "#0";

		// Ensure nothing after
		char* post = strtok_r(NULL, " \t,", &args);
		if (post) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", post);
	}

	char* operands[] = { xd, xs, imm, NULL };

	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	// Temporarily mark type with `encoding`
	instrObj->encoding = 0x0;
}

HANDLE_INSTR(handleR) {
	// R-Types have it in the form `[instr] <xd>, <xs>, <xr>`
	// (mul, smul, div, sdiv)

	char* xd = strtok_r(NULL, " \t,", &args);
	if (!xd) handleError(ERR_INVALID_SYNTAX, FATAL, "No destination register for %s!\n", instr);
	validateRegister(xd);

	char* xs = strtok_r(NULL, " \t,", &args);
	if (!xs) handleError(ERR_INVALID_SYNTAX, FATAL, "No source register for %s!\n", instr);
	validateRegister(xs);

	char* xr = strtok_r(NULL, " \t,", &args);
	if (!xr) handleError(ERR_INVALID_SYNTAX, FATAL, "No second source register for %s!\n", instr);
	validateRegister(xr);

	char* operands[] = { xd, xs, xr, NULL };

	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	instrObj->encoding = 0x1;
}

HANDLE_INSTR(handleIR) {
	// All IR instructions have common dest reg except cmp
	// Easy enough filter
	char* xd = NULL;

	// So far, only `cmp` has 'c', might change with more instructions that are IR
	if (*instr != 'c') {
		xd = strtok_r(NULL, " \t,", &args);
		if (!xd) handleError(ERR_INVALID_SYNTAX, FATAL, "No destination register for %s!\n", instr);
		validateRegister(xd);
	} else xd = VALID_REGISTERS[XZ];

	char* operands[] = { xd, PTR(0xFEEDFAED), PTR(0xFEEDFAED), NULL };

	// The rest of the registers and/or immediates depend on the instruction itself and context
	// The checking will be done in pass 2
	// For now, only blind tokenizing will occur
	// The max amount of operands, excluding destination, is 2
	int count = 1;

	while (args && count < 3) {
		char* operand = strtok_r(NULL, " \t,", &args);
		if (!operand) break;

		operands[count++] = operand;
	}
	char* rem = strtok_r(NULL, " \t,", &args);

	// However, all IR instructions have minimum of two operands and max of three
	if (operands[1] == PTR(0xFEEDFAED)) handleError(ERR_INVALID_SYNTAX, FATAL, "Expected operands, got nothing\n");
	if (rem) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", rem);


	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	instrObj->encoding = 0x10;
}

HANDLE_INSTR(handleM) {
	// Memory instructions are slightly more tricky
	// All memory ops have the following addressing modes:
	// mem_op reg, [x]
	// mem_op reg, [x/ir, off]
	// mem_op reg, [x], y

	// But ld has an exception:
	// ld reg, label
	// Which decomposes into six instructions: mv -> lsl -> mv -> lsl -> or -> add

	// After getting the destination register,
	// the way to store the possible configurations is in an array of three
	// First will store either x/ir
	// Second will either store off or 0xFEEDFAED
	// Third will either store y or NULL
	// { x_ir, off_0xFEEDFAED, y_NULL }

	char* xd = strtok_r(NULL, " \t,", &args);
	if (!xd) handleError(ERR_INVALID_SYNTAX, FATAL, "No destination register found for %x!\n", instr);
	validateRegister(xd);

	char* base = NULL;
	char* offset = PTR(0xFEEDFAED);
	char* index = PTR(0xFEEDFAED);

	// Skip all whitespace until '['
	// strtok is not used as there is no need to null-terminate early
	while (isblank(*args)) args++;
	char* addressing = args;
	
	// ld doesn't need [x.., it can also start with a label/immediate
	if (strcmp(instr, VALID_INSTRUCTIONS[M_TYPE_IDX]) == 0) {
		if (*addressing != '[') {
			// In the case that it does start with a label/immediate, do special handling
			// It can either be just a label or an expression involving a label

			char* expr = strtok_r(NULL, ",", &addressing);
			// expr should be everything after the first ',', addressing should point to the end
			char* rest = strtok_r(NULL, "", &addressing);
			if (rest) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", rest);

			bool moveLiteral = false;
			if (*expr == '=') {
				// Wanting to move a big number, just decompose
				expr++; // skip '='
				moveLiteral = true;
			}

			bool evald = true;
			uint32_t imm = eval(expr, symbTable, &evald);
			// Note that if eval returns false, it either means a symbol is not found yet or that the expression
			// 	is invalid, that won't be known until all symbol all gathered

			char* operands[5] = { xd, VALID_REGISTERS[XZ], NULL, NULL, NULL };
			char immmstr[16];
			char immlstr[16];
			uint32_t immm, imml;
			// If it was able to be evaled, check if it can be done IR-relative, if not, split
			if (evald) {
				if (!moveLiteral) {
					debug("ld is to attempt loading from immediate address, attempt to do IR-relative\n");
					uint32_t currLP = sectTable->entries[3].lp;
					int32_t irOffset = imm - currLP;
					debug("Offset is %d\n", irOffset);
					if (-256 <= irOffset && irOffset <= 255) {
						debug("Doing IR-relative ld!\n");
						// Range is in perfect range for IR-relative
						char irOffsetStr[16];
						sprintf(irOffsetStr, "#%d", irOffset);

						char* ldOperands[5] = { xd, "ir", irOffsetStr, index, NULL };

						instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) ldOperands);
						addInstrObj(instrStream, instrObj);
						instrObj->encoding = 0x2;

						return;
					}
				}
				debug("Unable to do IR-relative (too big or moving literal), doing decomposition\n");

				char immhstr[16];
				uint32_t immh;

				immh = (imm >> 18) & 0x3fff;
				immm = (imm >> 4) & 0x3fff;
				imml = (imm >> 0) & 0xf;

				sprintf(immhstr, "#%u", immh);
				sprintf(immmstr, "#%u", immm);
				sprintf(immlstr, "#%u", imml);

				operands[2] = immhstr;
				instr_obj_t* mvHInstr = initInstrObj(sectTable->entries[3].lp, NULL, "mv", (char**) operands);
				addInstrObj(instrStream, mvHInstr);
				mvHInstr->encoding = 0x0;
			} else {
				// If not (meaning it used an undefined label/symbol), leave it for future evaluation
				// But keep the need to split imm
				// This will be done in second pass (or maybe even before??) after the symbol table is complete
				// However, since many things depend on LP, pre-allocate space
				// Still keep the first/original instruction as the first
				// Save it as an I-type
				operands[2] = expr;
				instr_obj_t* ldInstr = initInstrObj(sectTable->entries[3].lp, NULL, "ld", (char**) operands);
				addInstrObj(instrStream, ldInstr);
				ldInstr->encoding = 0x11; // special encoding type

				// Add skeleton decompositions, omitting the immediates
				// Or even better, placing fake immediates
				sprintf(immmstr, "#o0");
				sprintf(immlstr, "#o0");
			}

			operands[1] = xd;
			operands[2] = "#18";
			instr_obj_t* lsl18Instr = initInstrObj(sectTable->entries[3].lp+4, NULL, "lsl", (char**) operands);
			addInstrObj(instrStream, lsl18Instr);
			lsl18Instr->encoding = 0x0;

			operands[0] = VALID_REGISTERS[C0];
			operands[1] = VALID_REGISTERS[XZ];
			operands[2] = immmstr;
			instr_obj_t* mvMInstr = initInstrObj(sectTable->entries[3].lp+8, NULL, "mv", (char**) operands);
			addInstrObj(instrStream, mvMInstr);
			mvMInstr->encoding = 0x0;

			operands[1] = VALID_REGISTERS[C0];
			operands[2] = "#4";
			instr_obj_t* lsl4Instr = initInstrObj(sectTable->entries[3].lp+12, NULL, "lsl", (char**) operands);
			addInstrObj(instrStream, lsl4Instr);
			lsl4Instr->encoding = 0x0;

			operands[0] = xd;
			operands[1] = xd;
			operands[2] = VALID_REGISTERS[C0];
			instr_obj_t* orInstr = initInstrObj(sectTable->entries[3].lp+16, NULL, "or", (char**) operands);
			addInstrObj(instrStream, orInstr);
			orInstr->encoding = 0x1;

			operands[2] = immlstr;
			instr_obj_t* addInstr = initInstrObj(sectTable->entries[3].lp+20, NULL, "add", (char**) operands);
			addInstrObj(instrStream, addInstr);
			addInstr->encoding = 0x0;

			sectTable->entries[3].lp += 20; // The normal LP is incremented by 4 in main, handle the extra five instructions

			// In the case that it is not a literal (need to load a value from the immediate address), add a normal load-reg
			// ld reg, [reg]
			if (!moveLiteral) {
				debug("ld is for loading from immediate address, adding normal ld\n");
				operands[2] = PTR(0xFEEDFAED); // Since this will be `ld xd, [xd]`, 0 is already xd, 1 is already xd
				operands[3] = VALID_REGISTERS[XZ];
				instr_obj_t* ldInstr = initInstrObj(sectTable->entries[3].lp+24, NULL, "ld", (char**) operands);
				addInstrObj(instrStream, ldInstr);
				ldInstr->encoding = 0x2;

				sectTable->entries[3].lp += 4;
			}

			return;
		}
		// Else, apply the general case
	}

	// Making sure the addressing starts with '['
	if (*addressing != '[') handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid memory addressing. Could not find '['.\n");

	char* end = strchr(addressing, ']');
	if (!end) handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid memory addressing. Could not find ']'.\n");

	// Get everything within "[]"
	char* dereferenced = strtok_r(NULL, "[]", &addressing);

	// There might be stuff after [], that being `y`, the index
	char* sep = strchr(addressing, ',');
	if (sep) {
		addressing = sep+1;
		// form is indexed, expected register after
		index = strtok_r(NULL, " \t", &addressing);
		if (!index) handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid memory addressing. Expected index register.\n");
		validateRegister(index);
		// Quick hack to check for ',' on index
		// Better way to check or something
		sep = strchr(index, ',');
		if (sep) handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid memory addressing. Unexpected ','.\n");
	}

	char* rest = strtok_r(NULL, " \t,", &addressing);
	if (rest) handleError(ERR_INVALID_SYNTAX, FATAL, "Invalid memory addressing. Unexpected operands: `%s`\n", rest);

	// index is set, if any
	// Now to extract base and offset

	sep = strchr(dereferenced, ',');
	if (sep) {
		// offset
		// [ base, #offset ]
		sep++; // skip ','
		base = strtok_r(NULL, " \t,", &dereferenced);
		offset = strtok_r(NULL, " \t", &sep);

	} else {
		// pure base
		// Note that there still might be some whitespace when `[   xs  ]`
		base = strtok_r(NULL, " \t", &dereferenced);
	}

	validateRegister(base);
	// Maybe defer offset validation later
	// validateImmediate(offset, SIMM9);

	// When using index addressing, no offset allowed and vice versa
	if (offset != PTR(0xFEEDFAED) && index != PTR(0xFEEDFAED)) {
		// Both got assigned to something, disallow it
		handleError(ERR_INVALID_SYNTAX, FATAL, "Indexed and Offset addressing not allowed in the same operation for `%s`\n", instr);
	} else {
		if (index == PTR(0xFEEDFAED)) {
			// Only offset got assigned, need to have index be XZ
			index = "xz";
		}
	}

	char* operands[5] = { xd, base, offset, index, NULL };

	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	instrObj->encoding = 0x2;
}

HANDLE_INSTR(handleBi) {
	char* expr = strtok_r(NULL, " \t", &args);
	if (!expr) handleError(ERR_INVALID_SYNTAX, FATAL, "Label not found for %s!\n", instr);
	if (*expr == '#') handleError(ERR_INVALID_SYNTAX, FATAL, "Cannot use immediate %s!\n", expr);

	char* rest = strtok_r(NULL, " \t,", &args);
	if (rest) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", rest);

	// args can either be a label in itself or an expression
	// Either way, it may contain undefined (as of now) labels
	// Hold

	char* operands[] = { expr, NULL };
	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	instrObj->encoding = 0x3;
}

HANDLE_INSTR(handleBu) {
	// ubr <xd> or ret
	char* xd = strtok_r(NULL, " \t,", &args);
	char* operands[2] = {NULL, NULL};

	if (xd) {
		validateRegister(xd);
		operands[0] = xd;
	} else operands[0] = "lr";

	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);

	instrObj->encoding = 0x4;
}

HANDLE_INSTR(handleBc) {
	// Almost the same as Bi but check on the condition

	bool valid = false;
	int size = sizeof(VALID_CONDS) / sizeof(char*);
	for (int i = 0; i < size; i++) {
		if (strcasecmp(VALID_CONDS[i], instr+1) == 0) {
			valid = true;
			break;
		}
	}

	if (!valid) handleError(ERR_INVALID_INSTRUCTION, FATAL, "Condition %s is not valid!\n", instr+1);

	handleBi(instrStream, symbTable, sectTable, instr, args);
}

HANDLE_INSTR(handleS) { 
	// As of now, S-types either have no operands or only one (ldir, mvcstr, ldcstr)
	// Can currently be differentiated by either 'l' or 'm'
	
	char* operands[2] = { NULL, NULL };
	if (*instr == 'l' || *instr == 'm') {
		// Check only one operand
		char* xd_xs = strtok_r(NULL, " \t", &args);
		if (!xd_xs) handleError(ERR_INVALID_SYNTAX, FATAL, "Expected registers, got nothing!\n");
		validateRegister(xd_xs);

		operands[0] = xd_xs;
	}
	// Check no operands
	char* rest = strtok_r(NULL, " \t,", &args);
	if (rest) handleError(ERR_INVALID_SYNTAX, FATAL, "Unexpected operands: `%s`\n", rest);

	instr_obj_t* instrObj = initInstrObj(sectTable->entries[3].lp, NULL, instr, (char**) operands);
	addInstrObj(instrStream, instrObj);
	instrObj->encoding = 0x5;
}

HANDLE_INSTR(handleF) { handleError(WARN, WARNING, "F-type instructions not implemented!\n"); }

void handleInstruction(InstructionStream* instrStream, SymbolTable* symbTable, SectionTable* sectTable, char* instr, char* args) {
	debug("\tHandling instruction (%s) with args (%s)\n", instr, args);

	// Maybe no need to lower, comparing can be down with strcasecmp
	// Even encoding uses strcasecmp
	// Or maybe, lower everything here so all others can use strcmp
	// Is the overhead that strcasecmp has worth it???
	// char* temp = instr;
	// TOLOWER(temp);
	// Make sure instr is valid
	int index = validateInstruction(instr);

	/**
	 * Due to the varying syntax of I-R instructions, most of its checking will be deferred to pass 2, when more context appears
	 * However, for true I/R types, they will be processed immediately
	 */

	if (index >= IR_TYPE_IDX && index < I_TYPE_IDX) handleIR(instrStream, symbTable, sectTable, instr, args);
	else if (index >= I_TYPE_IDX && index < R_TYPE_IDX ) handleI(instrStream, symbTable, sectTable, instr, args);
	else if (index >= R_TYPE_IDX && index < M_TYPE_IDX) handleR(instrStream, symbTable, sectTable, instr, args);
	else if (index >= M_TYPE_IDX && index < Bi_TYPE_IDX) handleM(instrStream, symbTable, sectTable, instr, args);
	else if (index >= Bi_TYPE_IDX && index < Bu_TYPE_IDX) handleBi(instrStream, symbTable, sectTable, instr, args);
	else if (index >= Bu_TYPE_IDX && index < Bc_TYPE_IDX) handleBu(instrStream, symbTable, sectTable, instr, args);
	else if (index >= Bc_TYPE_IDX && index < S_TYPE_IDX) handleBc(instrStream, symbTable, sectTable, instr, args);
	else if (index >= S_TYPE_IDX && index < F_TYPE_IDX) handleS(instrStream, symbTable, sectTable, instr, args);
	else if (index >= F_TYPE_IDX) handleF(instrStream, symbTable, sectTable, instr, args);
}