#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "InstructionStream.h"
#include "SymbolTable.h"
#include "DataTable.h"
#include "SectionTable.h"
#include "aef.h"


AEFbin* generateBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable);

void writeBinary(AEFbin* bin, char* outbin);

#endif