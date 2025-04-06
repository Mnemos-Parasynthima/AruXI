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

static char* VALID_INSTRUCTIONS[] = {
 "add", "adds", "sub", "subs", "mul", "smul", "div", "sdiv,",
 "or", "and", "xor", "not", "lsl", "lsr", "asr", "cmp",
 "mv", "mvn", "sxb", "sxh", "uxb", "uxh",
 "ld", "ldb", "ldbs", "ldbz", "ldh", "ldhs", "ldhz",
 "str", "strb", "strh",
 "ub", "ubr", "b", "call", "ret",
 "nop", "syscall", "hlt"
};

static char* VALID_REGISTERS[] = {
	"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10",
	"x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19", "x20",
	"x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31", "sp",
	"xr", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9",
	"c0", "c1", "c2", "c3", "c4", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10",
	"lr", "xb", "xz"
};

// The handle* functions are kind of lexer-parser type of functionality but not really but kind of
// Might need to find a new home for them

void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* save);

void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** save);

#endif