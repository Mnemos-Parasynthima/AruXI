#ifndef _LEXER_PARSER_LEGACY_H_
#define _LEXER_PARSER_LEGACY_H_

#include "SectionTable-legacy.h"
#include "InstructionStream-legacy.h"
#include "DataTable-legacy.h"
#include "SymbolTable-legacy.h"


enum ImmediateSize {
	IMM14, SIMM24, SIMM19, SIMM9
};


/**
 * 
 * @param reg 
 */
void validateRegister(char* reg);

/**
 * 
 * @param imm 
 * @param immSize
 */
void validateImmediate(char* imm, enum ImmediateSize immSize);

/**
 * 
 * @param reg 
 * @return 
 */
bool isValidRegister(char* reg);

/**
 * 
 * @param symbTable 
 * @param sectTable 
 * @param dataTable 
 * @param directive 
 * @param args 
 */
void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* directive, char* args);

/**
 * 
 * @param symbTable 
 * @param sectTable 
 * @param tok 
 * @param save 
 */
void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** save);

/**
 * 
 * @param instrStream 
 * @param symbTable 
 * @param sectTable 
 * @param instr 
 * @param args 
 */
void handleInstruction(InstructionStream* instrStream, SymbolTable* symbTable, SectionTable* sectTable, char* instr, char* args);

#endif