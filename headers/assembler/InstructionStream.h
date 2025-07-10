#ifndef _INSTRUCTION_STREAM_H_
#define _INSTRUCTION_STREAM_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct InstructionObject {
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

instr_obj_t* getInstr(InstructionStream* instrStream, uint32_t addr);

void displayInstrStream(InstructionStream* instrStream, bool showEncoding);

void deleteInstrStream(InstructionStream* instrStream);


#endif