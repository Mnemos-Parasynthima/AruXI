#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include "SectionTable.h"
#include "InstructionStream.h"
#include "DataTable.h"
#include "SymbolTable.h"


void handleDirective(SymbolTable* symbTable, SectionTable* sectTable, DataTable* dataTable, char* tok, char* save);

void handleLabel(SymbolTable* symbTable, SectionTable* sectTable, char* tok, char* save);

#endif