#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InstructionStream.h"
#include "assemblerError.h"


InstructionStream* initInstrStream() {
	InstructionStream* instrStream = (InstructionStream*) malloc(sizeof(InstructionStream));
	if (!instrStream) handleError(ERR_MEM, FATAL, "Could not allocate memory for instruction stream!\n");

	instr_obj_t** instrObjs = (instr_obj_t**) malloc(sizeof(instr_obj_t*) * 10);
	if (!instrObjs) handleError(ERR_MEM, FATAL, "Could not allocate memory for instruction objects array!\n");

	instrStream->instructions = instrObjs;
	instrStream->capacity = 10;
	instrStream->size = 0;

	return instrStream;
}

instr_obj_t* initInstrObj(uint32_t addr, char* source, char* instr, char** operands) {
	instr_obj_t* instrObj = (instr_obj_t*) malloc(sizeof(instr_obj_t));
	if (!instrObj) handleError(ERR_MEM, FATAL, "Could not allocate memory for instruction object!\n");

	instrObj->addr = addr;

	size_t sourceLen = strlen(source);
	instrObj->source = (char*) malloc(sizeof(char) * sourceLen + 1);
	strcpy(instrObj->source, source);

	size_t instrLen = strlen(instr);
	instrObj->instr = (char*) malloc(sizeof(char) * instrLen + 1);
	strcpy(instrObj->instr, instr);

	size_t operandsLen = 0;
	char** temp = operands;
	while (*temp) { temp++; operandsLen++; }
	instrObj->operands = (char**) malloc(sizeof(char*) * operandsLen + 1);


	return instrObj;
}

void addInstrObj(InstructionStream* instrStream, instr_obj_t* instrObj) {
}

void deleteInstrStream(InstructionStream* instrStream) {
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instrObj = instrStream->instructions[i];

		free(instrObj->instr);
		free(instrObj->source);

		char** temp = instrObj->operands;
		while (*temp) { free(*temp); temp++; }
		free(instrObj->operands);
		free(instrObj);
	}

	free(instrStream);
}