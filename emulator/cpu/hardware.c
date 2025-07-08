#include "hardware.h"
#include "core.h"
#include "diagnostics.h"

extern core_t core;

void alu() {
	uint32_t res = 0xFAEDDEAF;

	uint32_t vala = core.uarch.executeCtx.vala;
	uint32_t valb = core.uarch.executeCtx.valb;

	switch (core.uarch.decodeCtx.aluop)	{
		case ALU_PLUS:
			res = vala + valb;
			break;
		case ALU_MINUS:
			res = vala - valb;
			break;
		case ALU_MUL:
			res = vala * valb;
			break;
		case ALU_DIV:
			res = vala / valb;
			break;
		case ALU_OR:
			res = vala | valb;
			break;
		case ALU_XOR:
			res = vala ^ valb;
			break;
		case ALU_AND:
			res = vala & valb;
			break;
		case ALU_INV:
			res = ~vala;
		case ALU_LSL:
			res = vala << valb;
			break;
		case ALU_LSR:
			res = vala >> valb;
			break;
		case ALU_ASR:
			res = (int32_t)(vala >> valb);
			break;
		case ALU_PASS:
			res = 0;
			break;
		default:
			break;
	}

	core.uarch.executeCtx.valres = res;

	if (core.uarch.decodeCtx.setCC) {

	}
}

void fpu() {}

void vcu() {}

void regfile(bool write) {
	if (write) {
		// Do not allow write to X30
		if (core.uarch.decodeCtx.rd == 30) return;

		if (core.uarch.decodeCtx.rd == 31) {
			core.SP = MemoryCtx.valout;
		} else if (core.uarch.decodeCtx.rd != 30) {
			core.GPR[core.uarch.decodeCtx.rd] =MemoryCtx.valout;
		}
		dLog(D_NONE, DSEV_INFO, "regfile::Writing 0x%x to register %d", core.uarch.memoryCtx.valout, core.uarch.decodeCtx.rd);
		return;
	}

	if (core.uarch.decodeCtx.rs == 31) core.uarch.decodeCtx.vala = core.SP;
	else core.uarch.decodeCtx.vala = core.GPR[core.uarch.decodeCtx.rs];

	if (core.uarch.decodeCtx.rr == 31) core.uarch.decodeCtx.valb = core.SP;
	else core.uarch.decodeCtx.valb = core.GPR[core.uarch.decodeCtx.rr];

	dLog(D_NONE, DSEV_INFO, "regfile::Reading 0x%x from rs %d and 0x%x from rd %d", DecodeCtx.vala, DecodeCtx.rs, DecodeCtx.valb, DecodeCtx.rr);
}

void imem(uint32_t addr, uint32_t* ival, memerr_t* imemErr) {
	if (GET_PRIV(core.CSTR) == 0b0) *imemErr = validKIMemAddr(addr); // Kernel mode
	else *imemErr = validUIMemAddr(addr);

	if (*imemErr != MEMERR_NONE) return;

	*ival = (uint32_t) memReadInt(addr, imemErr);
}

void dmem(uint32_t addr, uint32_t* rval, uint32_t* wval, memerr_t* imemErr) {
	if (core.uarch.decodeCtx.memSize == 0) return;

	if (!rval && !wval) { *imemErr = MEMERR_INTERNAL; return; }

	int (*memRead)(uint32_t, memerr_t*);
	memerr_t (*memWrite)(uint32_t, int);

	switch (core.uarch.decodeCtx.memSize)	{
		case 1:
			memRead = &memReadByte;
			memWrite = &memWriteByte;
			break;
		case 2:
			memRead = &memReadShort;
			memWrite = &memWriteShort;
			break;
		case 4:
			memRead = &memReadInt;
			memWrite = &memWriteInt;
			break;
	}

	if (wval) *imemErr = memWrite(addr, *wval);
	else *rval = (uint32_t) memRead(addr, imemErr);
}