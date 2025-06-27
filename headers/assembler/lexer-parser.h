#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include "SectionTable.h"
#include "InstructionStream.h"
#include "DataTable.h"
#include "SymbolTable.h"


enum ImmediateSize {
	IMM14, SIMM24, SIMM19, SIMM9
};


void validateRegister(char* reg);
void validateImmediate(char* imm, enum ImmediateSize immSize);

void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* args);


void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** save);


void handleInstruction(InstructionStream* instrStream, SymbolTable* symbTable, SectionTable* sectTable, char* instr, char* args);

#endif