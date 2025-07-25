#ifndef _EVALUATOR_LEGACY_H_
#define _EVALUATOR_LEGACY_H_

#include <stdint.h>
#include <stdbool.h>

#include "SymbolTable-legacy.h"


/**
 * Evaluates the given expression, using the symbol table in case of any symbol usage.
 * If a symbol does not exist, it will place an incomplete entry, only placing the name and reference bit.
 * Since the expression can legitimately return -1, `canEval` is used to detect whether the expression is able to be evaluated.
 * Note that it assumes `canEval` is set to true initially.
 * @param expr The expression to evaluate
 * @param symbTable The symbol table
 * @param canEval True if it can be evaluated, false otherwise
 * @return The evaluated result
 */
int32_t eval(const char* expr, SymbolTable* symbTable, bool* canEval);

#endif