#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include "SectionTable.h"
#include "InstructionStream.h"
#include "DataTable.h"
#include "SymbolTable.h"


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
 * Handles the label.
 * @param symbTable 
 * @param sectTable 
 * @param tok 
 * @param args 
 */
void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char** tok, char** args);

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