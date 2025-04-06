#ifndef _INSTRUCTION_STREAM_H_
#define _INSTRUCTION_STREAM_H_

#include <stdint.h>


typedef struct InstructionObject {
	// uint8_t status; 
	uint32_t addr; // Address relative to section
	uint32_t encoding;
	char* source; // The original (preprocessed) string
	char* instr;
	char** operands; // null-terminated
} instr_obj_t;

typedef struct InstructionStream {
	instr_obj_t** instructions;
	uint32_t size;
	uint32_t capacity;
} InstructionStream;

InstructionStream* initInstrStream();

instr_obj_t* initInstrObj(uint32_t addr, char* source, char* instr, char** operands);

void addInstrObj(InstructionStream* instrStream, instr_obj_t* instrObj);

// instr_obj_t* getInstrObj();

void displayInstStream(InstructionStream* instrStream);

void deleteInstrStream(InstructionStream* instrStream);


#endif