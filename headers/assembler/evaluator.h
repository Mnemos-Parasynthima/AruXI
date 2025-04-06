#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

#include <stdint.h>
#include <stdbool.h>

#include "SymbolTable.h"


int32_t eval(const char* expr, SymbolTable* symbTable, bool* canEval);

#endif