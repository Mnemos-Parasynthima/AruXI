#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "encoder.h"
#include "constants.h"
#include "assemblerError.h"
#include "evaluator.h"


enum ImmediateSize {
	IMM14, SIMM24, SIMM19, SIMM9
};

#define SHOW_OPERANDS(ops) while (*ops) { printf("%s, ", *ops); ops++; }


static uint8_t getConditionEncoding(char* cond) {

}

static uint8_t getRegisterEncoding(char* reg) {
	// Special name
	if (strcasecmp(reg, VALID_REGISTERS[XR]) == 0) return 0;
	else if (strcasecmp(reg, VALID_REGISTERS[LR]) == 0) return 28;
	else if (strcasecmp(reg, VALID_REGISTERS[XB]) == 0) return 29;
	else if (strcasecmp(reg, VALID_REGISTERS[XZ]) == 0) return 30;
	else if (strcasecmp(reg, VALID_REGISTERS[SP]) == 0) return 31;

	if ((*reg == 'x' || *reg == 'X') || (*reg == 'a' || *reg == 'A')) return atoi(reg+1);

	if (*reg == 'c' || *reg == 'C') return (atoi(reg+1) + 12);

	if (*reg == 's' || *reg == 'S') return (atoi(reg+1) + 17);

	return 0;
}

static uint32_t getImmediateEncoding(char* imm, SymbolTable* symbTable, enum ImmediateSize size) {
	bool canEval = true;
	int32_t res = eval(imm, symbTable, &canEval);

	if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate immediate %s!\n", imm);

	// Ensure sizes are appropriate
	switch (size)	{
		case IMM14:
			if ((res & 0xc000L) != 0x0) handleError(ERR_INVALID_SIZE, FATAL, "Immediate %d exceeds allowed size!\n", res);
		case SIMM24:
			if ((res & 0xf000000L) != 0x0) handleError(ERR_INVALID_SIZE, FATAL, "Immediate %d exceeds allowed size!\n", res);
		case SIMM19:
			if ((res & 0x80000L) != 0x0) handleError(ERR_INVALID_SIZE, FATAL, "Immediate %d exceeds allowed size!\n", res);
		default:
			break;
	}

	return (uint32_t) res;
}

static void encodeI(instr_obj_t* instr, SymbolTable* symbTable) {
	uint32_t encoding = 0x00000000;
	uint8_t opcode = 0b00000000;

	char* instrStr = instr->instr;
	char** ops = instr->operands;

	char** temp = ops;
	SHOW_OPERANDS(temp)
	printf("\n");

	char* xd = ops[0];
	char* xs = NULL;
	char* imm = NULL;
	
	if (ops[2]) {
		xs = ops[1];
		imm = ops[2];
	} else imm = ops[1];

	if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[ADD]) == 0) || 
			(strcasecmp(instrStr, VALID_INSTRUCTIONS[MV]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[NOP]) == 0)) {
		opcode = 0b10000000;
	} else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[ADDS]) == 0) opcode = 0b10001000;
	else if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[SUB]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[MVN]) == 0)) {
		opcode = 0b10010000;
	} else if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[SUBS]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[CMP]) == 0)) {
		opcode = 0b10011000;	
	} else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[OR]) == 0) opcode = 0b01000000;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[AND]) == 0) opcode = 0b01000010;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[XOR]) == 0) opcode = 0b01000100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[NOT]) == 0) opcode = 0b01000110;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LSL]) == 0) opcode = 0b01001000;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LSR]) == 0) opcode = 0b01001010;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[ASR]) == 0) opcode = 0b01001100;
	else handleError(ERR_INVALID_INSTRUCTION, FATAL, "Could not detect instruction %s!\n", instrStr);

	uint8_t rd = getRegisterEncoding(xd);
	uint8_t rs = getRegisterEncoding(xs);
	uint16_t imm14 = (uint16_t) getImmediateEncoding(imm+1, symbTable, IMM14);

	encoding = (opcode << 24) | (imm14 << 10) | (rs << 5) | (rd << 0);
	instr->encoding = encoding;
}

static void encodeR(instr_obj_t* instr) {
	uint32_t encoding = 0x00000000;
	uint8_t opcode = 0b00000000;

	char* instrStr = instr->instr;
	char** ops = instr->operands;

	char** temp = ops;
	SHOW_OPERANDS(temp)
	printf("\n");

	char* xd = ops[0];
	char* xs = ops[1];
	char* xr = NULL;
	
	if (ops[2]) xr = ops[2];

	if (strcasecmp(instrStr, VALID_INSTRUCTIONS[ADD]) == 0) opcode = 0b10000001;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[ADDS]) == 0) opcode = 0b10001001;
	else if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[SUB]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[MVN]) == 0)) {
		opcode = 0b10010001;
	} else if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[SUBS]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[CMP]) == 0)) {
		opcode = 0b10011001;	
	} else if ((strcasecmp(instrStr, VALID_INSTRUCTIONS[OR]) == 0) || (strcasecmp(instrStr, VALID_INSTRUCTIONS[MV]) == 0)) {
		opcode = 0b01000001;
	} else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[AND]) == 0) opcode = 0b01000011;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[XOR]) == 0) opcode = 0b01000101;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[NOT]) == 0) opcode = 0b01000111;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LSL]) == 0) opcode = 0b01001001;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LSR]) == 0) opcode = 0b01001011;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[ASR]) == 0) opcode = 0b01001101;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[MUL]) == 0) opcode = 0b10100000;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[SMUL]) == 0) opcode = 0b10100010;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[DIV]) == 0) opcode = 0b10101000;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[SDIV]) == 0) opcode = 0b10101010;
	else handleError(ERR_INVALID_INSTRUCTION, FATAL, "Could not detect instruction %s!\n", instrStr);

	uint8_t rd = getRegisterEncoding(xd);
	uint8_t rs = getRegisterEncoding(xs);
	uint8_t rr = getRegisterEncoding(xr);

	encoding = (opcode << 24) | (rs << 10) | (rr << 5) | (rd << 0);
	instr->encoding = encoding;
}

static void encodeM(instr_obj_t* instr, SymbolTable* symbTable) {
	uint32_t encoding = 0x00000000;
	uint8_t opcode = 0b00000000;

	char* instrStr = instr->instr;
	char** ops = instr->operands;

	char** temp = ops;
	while (*temp && *temp != 0xFEEDFAED) { printf("%s, ", *temp); temp++; }
	printf("\n");

	char* xd = ops[0];
	char* base = ops[1];
	char* offset = ops[2];
	char* index = ops[3];

	uint8_t rd = getRegisterEncoding(xd);
	uint8_t rs = getRegisterEncoding(base);
	uint8_t rr = 0x0;
	int16_t simm = 0x0;

	if (index != 0xFEEDFAED) rr = getRegisterEncoding(index);

	if (offset != 0xFEEDFAED) simm = getImmediateEncoding(offset, symbTable, SIMM9);

	if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LD]) == 0) opcode = 0b00010100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDB]) == 0) opcode = 0b00110100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDBS]) == 0) opcode = 0b01010100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDBZ]) == 0) opcode = 0b01110100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDH]) == 0) opcode = 0b10010100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDHS]) == 0) opcode = 0b10110100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[LDHZ]) == 0) opcode = 0b11010100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[STR]) == 0) opcode = 0b00011100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[STRB]) == 0) opcode = 0b00111100;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[STRH]) == 0) opcode = 0b01011100;
	else handleError(ERR_INVALID_INSTRUCTION, FATAL, "Could not detect instruction %s!\n", instrStr);

	encoding = (opcode << 24) | (simm << 15) | (rs << 10) | (rr < 5) | (rd << 0);
	instr->encoding = encoding;
}

static void encodeBiBc(instr_obj_t* instr, SymbolTable* symbTable) {
	uint32_t encoding = 0x00000000;
	uint8_t opcode = 0b00000000;

	char* instrStr = instr->instr;
	char** ops = instr->operands;

	char** temp = ops;
	SHOW_OPERANDS(temp)
	printf("\n");

	char* expr = ops[0];

	bool canEval = true;
	int32_t simm = eval(expr, symbTable, &canEval);
	if (!canEval) handleError(ERR_INVALID_EXPRESSION, FATAL, "Could not evaluate label %s!\n", expr);
	if (simm % 4 != 0) handleError(ERR_INVALID_EXPRESSION, FATAL, "Immediate %d is not aligned to 4!\n", simm);

	// The encoding of the jump is (@ - IMM) / 4
	uint32_t addr = instr->addr;
	int32_t offset = (addr - simm) / 4;

	uint8_t shift = 0x0;
	uint8_t cond = 0x0;
	if (strcasecmp(instrStr, VALID_INSTRUCTIONS[UB]) == 0) opcode = 0b11000000;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[CALL]) == 0) opcode = 0b11000110;
	else if (*instrStr == 'b') { //b{cond}
		if ((offset & 0x80000L) != 0x0) handleError(ERR_INVALID_SIZE, FATAL, "Offset %d exceeds allowed size!\n", offset);
		opcode = 0b11000100;
		shift = 5;
		cond = getConditionEncoding(instrStr+1);
	}

	if ((offset & 0xf000000L) != 0x0) handleError(ERR_INVALID_SIZE, FATAL, "Offset %d exceeds allowed size!\n", offset);

	encoding = (opcode << 24) | (offset << shift) | (cond);
	instr->encoding = encoding;
}

static void encodeBu(instr_obj_t* instr) {
	uint32_t encoding = 0x00000000;
	uint8_t opcode = 0b00000000;

	char* instrStr = instr->instr;
	char** ops = instr->operands;

	char** temp = ops;
	SHOW_OPERANDS(temp)
	printf("\n");

	char* xd = ops[0];

	if (strcasecmp(instrStr, VALID_INSTRUCTIONS[UBR]) == 0) opcode = 0b11000010;
	else if (strcasecmp(instrStr, VALID_INSTRUCTIONS[RET]) == 0) opcode = 0b11001000;
	else handleError(ERR_INVALID_INSTRUCTION, FATAL, "Could not detect instruction %s!\n", instrStr);

	uint8_t rd = getRegisterEncoding(xd);

	encoding = (opcode << 24) | (rd << 0);
	instr->encoding = encoding;
}

void encode(InstructionStream* instrStream, SymbolTable* symbTable) {
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instr = instrStream->instructions[i];

		printf("Encoding %s\n", instr->instr);

		switch (instr->encoding) {
			case 0x0:
				encodeI(instr, symbTable);
				break;
			case 0x1:
				encodeR(instr);
				break;
			case 0x2:
				encodeM(instr, symbTable);
				break;
			case 0x3:
				encodeBiBc(instr, symbTable);
				break;
			case 0x4:
				encodeBu(instr);
				break;
			default:
				break;
		}
	}
}