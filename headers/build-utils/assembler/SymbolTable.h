#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include <stdint.h>


typedef struct SymbolEntry {
	char* name;
	union {
		char* expr;
		int32_t value;
	};
	uint16_t flags;
	char* source;
} symb_entry_t;

typedef struct SymbolTable {
	symb_entry_t** entries;
	uint32_t size;
	uint32_t capacity;
} SymbolTable;

/**
	| 15-9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	| x    | E | S | S | S | T | T | L | R | D |
	E: Whether the symbol hold an expression string or a value. `0` for value, `1` for expression
	S: The section the symbol is defined in. `00` for data, `01` for const, `10` for  bss, `11` for text, `100` for evt, `101` for ivt, `111` for undefined.
	T: The type of symbol. `00` for none (extern or unknown), `01` for absolute (set), `10` for function, `11` for object
	L: The locality of the symbol. `0` for local, and `1` for global.
	R: The reference state of the symbol. `0` if not referenced, and `1` if referenced.
	D: The defined state of the symbol. `0` for undefined, and `1` for defined.
*/

#define CREATE_FLAGS(E,S,T,L,R,D) ((E<<8)|(S<<5)|(T<<3)|(L<<2)|(R<<1)|(D<<0))

#define GET_EXPRESSION(flags) ((flags>>8) & 0b1)
#define GET_SECTION(flags) ((flags>>5) & 0b111)
#define GET_TYPE(flags) ((flags>>3) & 0b11)
#define GET_LOCALITY(flags) ((flags>>2) & 0b1)
#define GET_REFERENCE(flags) ((flags>>1) & 0b1)
#define GET_DEFINED(flags) ((flags>>0) & 0b1)

#define SET_EXPRESSION(flags) (flags &= ~(1 << 8)) // Sets the expression flag to 0
#define SET_TYPE(flags, type) ((flags & ~0b11000) | ((type & 0b11000) << 3)) // Sets the type to the given type
#define SET_LOCALITY(flags) (flags |= (1 << 2)) // Sets the locality flag to 1 for global
#define SET_REFERENCE(flags) (flags |= (1 << 1)) // Sets referenced
#define SET_DEFINED(flags) (flags |= (1 << 0)) // Sets defined


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
 * @param source The source line
 * @return A symbol entry
 */
symb_entry_t* initSymbEntry(char* name, char* expr, int32_t value, uint32_t flags, char* source);

/**
 * Adds a symbol entry to the symbol table. This creates a new entry to the table, that is 
 * it will assume the entry does not exist. If an entry needs to be updated, it is to use `updateSymbEntry`.
 * @param symbTable The symbol table
 * @param symbEntry The symbol entry
 */
void addSymbEntry(SymbolTable* symbTable, symb_entry_t* symbEntry);

/**
 * Updates an existing entry with the value and flags. This is most common when either the expression has been evaluated
 * or the state has been updates (from local to global or unreferenced to referenced).
 * @param symbEntry The symbol entry
 * @param value The new (or current value)
 * @param flags The updated (or same) flags
 */
void updateSymbEntry(symb_entry_t* symbEntry, uint32_t value, uint32_t flags);

/**
 * Retrieves the symbol with the given name.
 * @param symbTable The symbol table
 * @param name The name of the symbol
 * @return The symbol entry
 */
symb_entry_t* getSymbEntry(SymbolTable* symbTable, char* name);

/**
 * Displays the entire symbol table.
 * @param symbTable The symbol table
 */
void displaySymbTable(SymbolTable* symbTable);

/**
 * Frees all contents of the symbol table.
 * @param symbTable The symbol table
 */
void deleteSymbTable(SymbolTable* symbTable);

#endif