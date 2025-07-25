#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "InstructionStream.h"
#include "SymbolTable.h"
#include "evaluator.h"


void encode(InstructionStream* instrStream, SymbolTable* symbTable);

#endif