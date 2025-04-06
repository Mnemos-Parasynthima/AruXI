#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "evaluator.h"


typedef enum TokenType {
	END, INT, SYMB, OP, LPAREN, RPAREN
} token_type_t;

typedef struct Token {
	enum TokenType type;
	char* text;
	int32_t val;
} token_t;

typedef struct Lexer {
	const char* input;
	uint32_t pos;
	token_t curr;
} Lexer;


static void nextToken(struct Lexer* lexer);
static int32_t _eval(struct Lexer* lexer, SymbolTable* symbTable, int prec, bool* canEval);

static int precedence(const char* op) {
	if (strcmp(op, "|") == 0) return 8;
	if (strcmp(op, "^") == 0) return 7;
	if (strcmp(op, "&") == 0) return 6;
	if (strcmp(op, "<<") == 0 || strcmp(op, ">>") == 0) return 5;
	if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 4;
	if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0) return 3;
	return -1;
}

static int32_t parse(struct Lexer* lexer, SymbolTable* symbTable, bool* canEval) {
	token_t tok = lexer->curr;
	if (tok.type == INT) {
		*canEval = true;
		nextToken(lexer);
		return tok.val;
	} else if (tok.type == SYMB) {
		symb_entry_t* entry = getSymbEntry(symbTable, tok.text);
		if (!entry || GET_DEFINED(entry->flags) == 0) {
			if (!entry) {
				uint32_t flags = CREATE_FLAGS(0, 0, 0, 0, 1, 0);
				entry = initSymbEntry(tok.text, NULL, 0, flags);
				addSymbEntry(symbTable, entry);
			}

			*canEval = false;
			return -1;
		}

		SET_REFERENCE(entry->flags);

		int32_t res;
		if (GET_EXPRESSION(entry->flags)) {
			struct Lexer subLexer = { .input = entry->expr, .pos = 0 };
			nextToken(&subLexer);
			res = _eval(&subLexer, symbTable, 0, canEval);
		} else {
			*canEval = true;
			res = entry->value;
		}
		nextToken(lexer);
		return res;
	} else if (tok.type == LPAREN) {
		nextToken(lexer);

		int32_t res = _eval(lexer, symbTable, 0, canEval);
		if (lexer->curr.type != RPAREN) {
			*canEval = false;
			return -1;
		}
		nextToken(lexer);
		return res;
	} else if (tok.type == OP && (strcmp(tok.text, "-") == 0 || strcmp(tok.text, "~") == 0)) {
		char* op = tok.text;
		nextToken(lexer);

		int32_t right = parse(lexer, symbTable, canEval);
		if (!*canEval) return right;
		if (strcmp(op, "-") == 0) return -right;
		else if (strcmp(op, "~") == 0) return ~right;
	}

	*canEval = false;
	return -1;
}

static int32_t _eval(struct Lexer* lexer, SymbolTable* symbTable, int prec, bool* canEval) {
	int32_t left = parse(lexer, symbTable, canEval);
	if (!*canEval) return left;

	while (lexer->curr.type == OP && precedence(lexer->curr.text) >= prec) {
		char* op = lexer->curr.text;
		int opPrec = precedence(op);
		nextToken(lexer);
		int32_t right = _eval(lexer, symbTable, opPrec+1, canEval);
		if (!*canEval) return right;

		if (strcmp(op, "+") == 0) left += right;
		else if (strcmp(op, "-") == 0) left -= right;
		else if (strcmp(op, "*") == 0) left *= right;
		else if (strcmp(op, "/") == 0) left /= right;
		else if (strcmp(op, "<<") == 0) left <<= right;
		else if (strcmp(op, ">>") == 0) left >>= right;
		else if (strcmp(op, "&") == 0) left &= right;
		else if (strcmp(op, "^") == 0) left ^= right;
		else if (strcmp(op, "|") == 0) left |= right;
	}
	
	return left;
}

static void nextToken(struct Lexer* lexer) {
	while (isspace(lexer->input[lexer->pos])) lexer->pos++;

	const char* str = &lexer->input[lexer->pos];
	if (!*str) {
		lexer->curr = (token_t){ .type = END };
		return;
	}

	if (isdigit(*str) || (*str == '0' && (str[1] == 'x' || str[1] == 'b'))) {
		int base = 10;
		if (str[1] == 'x') base = 16;
		else if (str[1] == 'b') base = 2;

		char* end;
		int32_t val = strtol(str, &end, base);

		lexer->curr = (token_t){ .type = INT, .val = val };
		lexer->pos = end - lexer->input;
	} else if (isalpha(*str) || *str == '_') {
		size_t len = 0;
		while (isalpha(str[len]) || str[len] == '_') len++;

		char* name = strndup(str, len);
		lexer->curr = (token_t){ .type = SYMB, .text = name };
		lexer->pos += len;
	} else if (*str == '(') {
		lexer->curr = (token_t){ .type = LPAREN };
		lexer->pos++;
	} else if (*str == ')') {
		lexer->curr = (token_t){ .type = RPAREN };
		lexer->pos++;
	} else {
		size_t len = 1;
		if ((str[0] == '<' || str[0] == '>') && str[1] == str[0]) len = 2;

		char* op = strndup(str, len);
		lexer->curr = (token_t){ .type = OP, .text = op };
		lexer->pos += len;
	}
}

int32_t eval(const char* expr, SymbolTable* symbTable, bool* canEval) {
	printf("\t\tEVALUATING (%s)\n", expr);

	struct Lexer lexer = { .input = expr, .pos = 0 };
	nextToken(&lexer);
	int32_t res = _eval(&lexer, symbTable, 0, canEval);

	if (*canEval) printf("\t\t \x1b[32m ABLE TO EVALUATE TO %d \x1b[0m\n", res);
	else printf("\t\t \x1b[31m UNABLE TO EVALUATE \x1b[0m\n");

	return res;
}