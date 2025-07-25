#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InstructionStream.h"
#include "assemblerDiagnostics.h"


InstructionStream* initInstrStream() {
	InstructionStream* instrStream = (InstructionStream*) malloc(sizeof(InstructionStream));
	if (!instrStream) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction stream!\n");

	instr_obj_t** instrObjs = (instr_obj_t**) malloc(sizeof(instr_obj_t*) * 10);
	if (!instrObjs) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction objects array!\n");

	instrStream->instructions = instrObjs;
	instrStream->capacity = 10;
	instrStream->size = 0;

	return instrStream;
}

instr_obj_t* initInstrObj(uint32_t addr, char* source, char* instr, char** operands) {
	debug("\tCreating instruction object for %s\n", instr);

	instr_obj_t* instrObj = (instr_obj_t*) malloc(sizeof(instr_obj_t));
	if (!instrObj) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction object!\n");

	instrObj->addr = addr;

	instrObj->source = source; // Assuming it has been allocated

	size_t instrLen = strlen(instr);
	instrObj->instr = (char*) malloc(sizeof(char) * instrLen + 1);
	if (!instrObj->instr) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction in object!\n");
	strcpy(instrObj->instr, instr);

	size_t operandsLen = 0;
	char** temp = operands;
	while (*temp) { temp++; operandsLen++; }
	instrObj->operands = (char**) malloc(sizeof(char*) * (operandsLen + 1));
	if (!instrObj->operands) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction operands!\n");

	for (int i = 0; i < operandsLen; i++) {
		char* operand = operands[i];

		if (operand == 0xFEEDFAED) {
			instrObj->operands[i] = operand;
			continue;
		}

		size_t operandLen = strlen(operand);
		char* _operand = (char*) malloc(sizeof(char) * operandLen + 1);
		if (!_operand) emitError(ERR_MEM, NULL, "Could not allocate memory for instruction operand!\n");

		strcpy(_operand, operand);

		instrObj->operands[i] = _operand;
	}
	instrObj->operands[operandsLen] = NULL;

	instrObj->linenum = 0;

	return instrObj;
}

void addInstrObj(InstructionStream* instrStream, instr_obj_t* instrObj) {
	if (instrStream->size == instrStream->capacity) {
		instrStream->capacity *= 2;

		instr_obj_t** temp = (instr_obj_t**) realloc(instrStream->instructions, instrStream->capacity * sizeof(instr_obj_t*));
		if (!temp) emitError(ERR_MEM, NULL, "Could not reallocate memory for new instructions!\n");

		instrStream->instructions = temp;
	}

	int idx = instrStream->size;
	instrStream->instructions[idx] = instrObj;
	instrStream->size++;	
}

instr_obj_t* getInstr(InstructionStream* instrStream, uint32_t addr) {
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instr = instrStream->instructions[i];

		if (instr->addr == addr) return instr;
	}

	return NULL;
}

void displayInstrStream(InstructionStream* instrStream, bool showEncoding) {
	debug(DEBUG_TRACE, "Instruction Stream (%d instructions):\n", instrStream->size);
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instrObj = instrStream->instructions[i];

		debug(DEBUG_TRACE, "\tInstruction: %s\n", instrObj->instr);
		debug(DEBUG_TRACE, "\t\tOperands: ");
		char** temp = instrObj->operands;
		while (*temp) {
			if (*temp != 0xFEEDFAED) debug(DEBUG_TRACE, "(%s), ", *temp);
			else debug(DEBUG_TRACE, "(), ");

			temp++;
		}
		debug(DEBUG_TRACE, "\n");

		debug(DEBUG_TRACE, "\t\tAddress: 0x%x\n", instrObj->addr);

		if (showEncoding) debug(DEBUG_TRACE, "\t\tEncoding: 0x%x\n", instrObj->encoding);
	}
}

void deleteInstrStream(InstructionStream* instrStream) {
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instrObj = instrStream->instructions[i];

		free(instrObj->instr);
		// Even though the source string is not allocated into the stream, it now takes ownership
		if (instrObj->source) free(instrObj->source);

		char** temp = instrObj->operands;
		while (*temp) {
			if (*temp != 0xFEEDFAED) free(*temp);
			temp++;
		}
		free(instrObj->operands);
		free(instrObj);
	}

	free(instrStream->instructions);
	free(instrStream);
}