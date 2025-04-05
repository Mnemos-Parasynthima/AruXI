#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdint.h>


typedef struct SymbolEntry {
	char* name;
	union {
		char* expr;
		int32_t value;
	};
	uint32_t flags;
} symb_entry_t;

typedef struct SymbolTable {
	symb_entry_t** entries;
	uint32_t size;
	uint32_t capacity;
} SymbolTable;

/**
	| 31-8 | 7   | 6   | 5   | 4   | 3   | 2   | 1   | 0   |
	| x    | E   | -   | S   | S   | T   | L   | R   | D   |
	E: Whether the symbol hold an expression string or a value. `0` for value, `1` for expression
	S: The section the symbol is defined in. `00` for data, `01` for const, `10` for bss, and `11` for text.
	T: The type of symbol. `0` for address, and `1` for `.set`.
	L: The locality of the symbol. `0` for local, and `1` for global.
	R: The reference state of the symbol. `0` if not referenced, and `1` if referenced.
	D: The defined state of the symbol. `0` for undefined, and `1` for defined.
*/

#define CREATE_FLAGS(E,S,T,L,R,D) ((E<<7)|(S<<4)|(T<<3)|(L<<2)|(R<<1)|(D<<0))

#define GET_EXPRESSION(flags) ((flags>>7) & 0b1)
#define GET_SECTION(flags) ((flags>>4) & 0b11)
#define GET_TYPE(flags) ((flags>>3) & 0b1)
#define GET_LOCALITY(flags) ((flags>>2) & 0b1)
#define GET_REFERENCE(flags) ((flags>>1) & 0b1)
#define GET_DEFINED(flags) ((flags>>0) & 0b1)

#define SET_EXPRESSION(flags) (flags &= (0 << 7)) // Sets the expression flag to 0
#define SET_LOCALITY(flags) (flags |= (1 << 2)) // Sets the locality flag to 1 for global
#define SET_REFERENCE(flags) (flags |= (1 << 1)) // Sets referenced
#define SET_DEFINED(flags) (flags |= (1 << 0))


/**
 * Creates an empty symbol table.
 * @return The symbol table
 */
SymbolTable* initSymbTable();

/**
 * Creates a symbol entry with the provided information. `expr` and `value` are exclusive.
 * That is, if `expr` is NULL, then `value` must hold a number (unless it is 0, naturally).
 * If not, then `expr` will hold the expression string. The passed flags are to be already created.
 * @param name The symbol name
 * @param expr The expression string
 * @param value The symbol value
 * @param flags The symbol entry flags to set
 * @return A symbol entry
 */
symb_entry_t* initSymbEntry(char* name, char* expr, int32_t value, uint32_t flags);

// void updateSymbEntry(symb_entry_t* symbEntry, int32_t value, uint8_t activeSection, )

/**
 * Adds a symbol entry to the symbol table. This creates a new entry to the table, that is 
 * it will assume the entry does not exist. If an entry needs to be updated, it is to use `updateSymbEntry`.
 * @param symbTable The symbol table
 * @param symbEntry The symbol entry
 */
void addSymbEntry(SymbolTable* symbTable, symb_entry_t* symbEntry);

/**
 * @brief 
 * @param symbEntry 
 * @param value 
 * @param flags 
 */
void updateSymbEntry(symb_entry_t* symbEntry, uint32_t value, uint32_t flags);

symb_entry_t* getSymbEntry(SymbolTable* symbTable, char* name);

void deleteSymbTable(SymbolTable* symbTable);

#endif