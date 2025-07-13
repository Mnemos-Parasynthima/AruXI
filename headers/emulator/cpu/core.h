#ifndef _CORE_H_
#define _CORE_H_

#include <stdint.h>

#include "instr.h"


typedef enum status {
	STAT_HLT,
	STAT_RUNNING
} status_t;

typedef struct VectorRegister {
	union {
		uint64_t _v64[8];
		uint32_t _v32[4];
	};
} vec_reg_t;

typedef struct Core {
	uint32_t GPR[31];
	uint32_t IR;
	uint32_t SP;
	float FPR[16];
	vec_reg_t VR[6];
	uint16_t CSTR;

	InstrCtx uarch;
	status_t status;
} core_t;

/**
	| 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	| -  | A  | -  | -  | -  | -  | L | - | - | I | - | - | C | O | N | Z |
	A: Whether the AVExt is enabled
	L: The privilege level; 1 for user; 0 for kernel
	I: If interrupts are enabled
	C: Carry flag
	O: Overflow flag
	N: Negtive flag
	Z: Zero flag
*/

#define SET_CONDS(C,O,N,Z) ((C<<3)|(O<<2)|(N<<1)|(Z<<0))
#define SET_INT(I,bits) ((I<<6) | bits)
#define SET_PRIV(L,bits) ((L<<9) | bits)
#define SET_AVE(A,bits) ((A<<14) | bits)

#define GET_C(flags) ((flags>>3) & 0b1)
#define GET_O(flags) ((flags>>2) & 0b1)
#define GET_N(flags) ((flags>>1) & 0b1)
#define GET_Z(flags) ((flags>>0) & 0b1)
#define GET_INT(flags) ((flags>>6) & 0b1)
#define GET_PRIV(flags) ((flags>>9) & 0b1)
#define GET_AVE(flags) ((flags>>14) & 0b1)

void initCore();
void* runCore(void*);


#endif