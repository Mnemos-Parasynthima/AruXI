#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "preprocessor.h"
#include "assemblerError.h"

#define COMMENT '%'

char* preprocess(char* line, ssize_t len) {
	// Quick and immediate
	if (*line == '\n' || *line == COMMENT) return NULL;

	// Line could either be a lot of whitespace before actual text
	// or text upfront but whitespace following it
	// or comments after text
	// or a combination

	// Start from the left and go through any whitespace
	char* start = line;
	while (isblank(*start)) start++;

	// Comment or end of line has been reached, ignore the rest
	if (*start == COMMENT || *start == '\n') return NULL;

	// Start from the right and go through any whitespace left-wise
	char* end = (line + len) - 1;
	if (*end == '\n') end -= 1;
	while (end > start && isblank(*end)) end--;

	// There may be comments at the end, start search at (new) beginning
	char* commentPos = start;
	while (*commentPos != COMMENT && commentPos <= end) commentPos++;

	// There may be some space between the last proper "token" and the start of the comment
	// Like "add x0, x1, x2  % comment"
	// Backtrack to remove it
	end = commentPos;

	if (*end == COMMENT && isblank(*(end-1))) {
		end--;
		while (end > start && isblank(*end) && isblank(*(end-1))) end--;
	}

	// New string for removed whitespace and comments
	size_t newSize = end - start;
	char* trimmed = (char*) malloc(newSize + 1);
	if (!trimmed) handleError(ERR_MEM, FATAL, "Could not allocate memory for trimmed string!\n");
	memcpy(trimmed, start, newSize);
	*(trimmed + newSize) = '\0';

	return trimmed;
}