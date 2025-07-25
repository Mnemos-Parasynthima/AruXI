#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "InstructionStream.h"
#include "SymbolTable.h"
#include "DataTable.h"
#include "SectionTable.h"
#include "RelocationTable.h"
#include "aoef.h"


AOEFbin* generateObjectBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable);

void writeObjectBinary(AOEFbin* bin, char* outbin);

#endif