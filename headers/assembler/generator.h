#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "InstructionStream.h"
#include "SymbolTable.h"
#include "DataTable.h"
#include "SectionTable.h"
#include "aoef.h"


AOEFbin* generateBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable);

void writeBinary(AOEFbin* bin, char* outbin);

#endif