#ifndef _GENERATOR_LEGACY_H_
#define _GENERATOR_LEGACY_H_

#include "InstructionStream-legacy.h"
#include "SymbolTable-legacy.h"
#include "DataTable-legacy.h"
#include "SectionTable-legacy.h"
#include "aoef-legacy.h"


AOEFbin* generateBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable);

void writeBinary(AOEFbin* bin, char* outbin);

#endif