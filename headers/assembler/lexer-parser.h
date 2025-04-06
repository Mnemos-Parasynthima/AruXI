#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include "SectionTable.h"
#include "InstructionStream.h"
#include "DataTable.h"
#include "SymbolTable.h"


static char* VALID_DIRECTIVES[] = {
	"data", "const", "bss", "text", "set", "glob", "end",
	"string", "byte", "hword", "word", "float", "zero", "fill", "align",
	// "entry", "size"
};

// The array is to be ordered by instruction types!!!
// This is vital to instruction parsing
static char* VALID_INSTRUCTIONS[] = {
	// I/R-Type (for instruction that have two types)
	"add", "adds", "sub", "subs",
	"or", "and", "xor", "not",
	"lsl", "lsr", "asr",
	"cmp", "mv", "mvn", // aliased

	// I-Type
	"nop", // aliased to add (imm)


	// R-Type
	"mul", "smul", "div", "sdiv",

	// M-Type
	"ld", "ldb", "ldbs", "ldbz", "ldh", "ldhs", "ldhz",
	"str", "strb", "strh",

	// Bi-Type
	"ub", "call",

	// Bu-Type
	"ubr", "ret",

	// Bc-Type
	"b",

	// S-Type
	"syscall", "hlt",

	// F-Type

};
#define IR_TYPE_IDX 0
#define I_TYPE_IDX 14
#define R_TYPE_IDX 15
#define M_TYPE_IDX 19
#define Bi_TYPE_IDX 29
#define Bu_TYPE_IDX 31
#define Bc_TYPE_IDX 33
#define S_TYPE_IDX 34
#define F_TYPE_IDX 36

static char* VALID_REGISTERS[] = {
	"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10",
	"x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19", "x20",
	"x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31", "sp",
	"xr", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9",
	"c0", "c1", "c2", "c3", "c4", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10",
	"lr", "xb", "xz"
};

static char* VALID_CONDS[] = {
	"eq",
	"ne",
	"ov",
	"nv",
	"mi",
	"pz",
	"cc",
	"cs",
	"gt",
	"ge",
	"lt",
	"le"
};

// The handle* functions are kind of lexer-parser type of functionality but not really but kind of
// Might need to find a new home for them

void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* args);

void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** save);

void handleInstruction(InstructionStream* instrStream, SymbolTable* symbTable, SectionTable* sectTable, char* instr, char* args);

#endif