#include <signal.h>
#include <sys/types.h>
#include <pthread.h>

#include "core.h"
#include "mem.h"
#include "hardware.h"
#include "emSignal.h"
#include "diagnostics.h"


extern core_t core;
extern opcode_t imap[];
extern char* istrmap[];
extern SigMem* sigMem;

extern pthread_mutex_t idleLock;
extern pthread_cond_t idleCond;
extern volatile bool IDLE;


static void fault() {
	sigMem->metadata.signalType = UNIVERSAL_SIG;
	int set = setFaultSignal(GET_SIGNAL(sigMem->signals, UNIVERSAL_SIG));
	if (set != -1) {
		fflush(stderr);
		fflush(stdout);
		flushDebug();
		kill(sigMem->metadata.shellPID, SIGUSR1);
		raise(SIGUSR1);
	} else dLog(D_NONE, DSEV_WARN, "Was not able to set fault signal!");
}


static void fetch() {
	memerr_t err;
	dLog(D_NONE, DSEV_INFO, "\nfetch::Fetching from 0x%x...", core.IR);
	imem(core.IR, &core.uarch.fetchCtx.instrbits, &err);

	core.IR += 4;

	if (err != MEMERR_NONE) {
		if (GET_PRIV(core.CSTR) == 0b1) {
			// user code attempted to access elsewhere, run EVT
			// TODO: Need to work out EVT stuff
			dLog(D_NONE, DSEV_WARN, "User code attempted to access outside of permissible range!");
		} else {
			dLog(D_NONE, DSEV_WARN, "Kernel code attempted to access outside of permissible range!");
			fault();
		}
	}

	dLog(D_NONE, DSEV_INFO, "fetch::Got 0x%x", core.uarch.fetchCtx.instrbits);
}


void extractImm() {
	uint16_t imm14 = u32bitextract(core.uarch.fetchCtx.instrbits, 10, 14);
	int32_t simm24 = s32bitextract(core.uarch.fetchCtx.instrbits, 0, 24);
	int32_t simm19 = s32bitextract(core.uarch.fetchCtx.instrbits, 5, 19);
	uint16_t simm9 = s32bitextract(core.uarch.fetchCtx.instrbits, 15, 9);

	opcode_t opcode = core.uarch.fetchCtx.opcode;
	itype_t type = NO_TYPE;
	int32_t imm = 0x0;

	if (opcode >= OP_NOP && opcode <= OP_CMP) {
		if (((((core.uarch.fetchCtx.instrbits >> 24) & 0xff) & 0b1) == 0b0) && (opcode <= OP_MUL || opcode >= OP_SDIV)) {
			imm = imm14; // I-types
			type = I_TYPE;
		} else type = R_TYPE;
	} else if (opcode >= OP_LD && opcode <= OP_STRH) {
		imm = simm9; // M-types
		type = M_TYPE;
	} else if (opcode == OP_UB || opcode == OP_CALL) {
		imm = simm24; // Bi-types
		type = BI_TYPE;
	} else if (opcode == OP_B) {
		imm = simm19; // Bc-types
		type = BC_TYPE;
	}

	core.uarch.decodeCtx.imm = imm;
	core.uarch.decodeCtx.iType = type;
}

void extractRegs() {
	uint8_t _rd = u32bitextract(core.uarch.fetchCtx.instrbits, 0, 5);
	uint8_t _rsI = u32bitextract(core.uarch.fetchCtx.instrbits, 5, 5);
	uint8_t _rsR = u32bitextract(core.uarch.fetchCtx.instrbits, 10, 5);
	uint8_t _rr = u32bitextract(core.uarch.fetchCtx.instrbits, 5, 5);

	uint8_t rd, rs, rr;

	itype_t type = core.uarch.decodeCtx.iType;
	opcode_t opcode = core.uarch.fetchCtx.opcode;
	// if (type )

	if (opcode == OP_UBR || opcode == OP_RET) {
		rd = _rd; // Bu-Type
		core.uarch.decodeCtx.iType = BU_TYPE;
	} else if (type == I_TYPE) {
		rd = _rd;
		rs = _rsI;
	} else if (type == R_TYPE || type == M_TYPE) {
		rd = _rd;
		rs = _rsR;
		rr = _rr;
	}

	core.uarch.decodeCtx.rd = rd;
	core.uarch.decodeCtx.rs = rs;
	core.uarch.decodeCtx.rr = rr;
}

void decideALUOp() {
	aluop_t aluop = ALU_PASS;
	switch (core.uarch.fetchCtx.opcode)	{
		// MV, MVN, CMP, and NOP do not appear as they are aliased

		case OP_ADD: case OP_ADDS: case OP_LD: case OP_LDB:
		case OP_LDBS: case OP_LDBZ: case OP_LDH: case OP_LDHS:
		case OP_LDHZ: case OP_STR: case OP_STRB: case OP_STRH:
		case OP_MV: // MV includes both MVI and MVR, which MVR uses OR
			aluop = ALU_PLUS;
			break;
		case OP_SUB: case OP_SUBS: case OP_MVN: case OP_CMP:
			aluop = ALU_MINUS;
			break;
		case OP_OR:
			aluop = ALU_OR;
			break;
		case OP_AND:
			aluop = ALU_AND;
			break;
		case OP_XOR:
			aluop = ALU_XOR;
			break;
		case OP_NOT:
			aluop = ALU_INV;
			break;
		case OP_LSL:
			aluop = ALU_LSL;
			break;
		case OP_LSR:
			aluop = ALU_LSR;
			break;
		case OP_ASR:
			aluop = ALU_ASR;
			break;
		case OP_MUL: case OP_SMUL:
			aluop = ALU_MUL;
			break;
		case OP_DIV: case OP_SDIV:
			aluop = ALU_DIV;
			break;
		default:
			break;
	}

	core.uarch.decodeCtx.aluop = aluop;
}

static void decode() {
	uint8_t opcode = (core.uarch.fetchCtx.instrbits >> 24) & 0xff;
	opcode_t code = imap[opcode];
	core.uarch.fetchCtx.opcode = code;

	dLog(D_NONE, DSEV_INFO, "decode::Opcode: 0x%x; code %d -> %s", opcode, code, (code != OP_ERROR) ? istrmap[code] : "OP_ERROR");

	// TODO: EVT for user code, kill for kernel code
	// Invalid instruction
	if (code == OP_ERROR) {
		if (GET_PRIV(core.CSTR) == 0b0) { // Kill for kernel code
			dLog(D_NONE, DSEV_WARN, "Invalid instruction: 0x%x!", opcode);
			fault();
		} else { // EVT for user code

		}
	}

	if (code == OP_ADDS || code == OP_SUBS || code == OP_CMP) core.uarch.decodeCtx.setCC = true;
	else core.uarch.decodeCtx.setCC = false;

	switch (code) {
		case OP_LDB: case OP_LDBS: case OP_LDBZ: case OP_STRB:
			core.uarch.decodeCtx.memSize = 1;
			break;
		case OP_LDH: case OP_LDHS: case OP_LDHZ: case OP_STRH:
			core.uarch.decodeCtx.memSize = 2;
			break;
		case OP_LD: case OP_STR:
			core.uarch.decodeCtx.memSize = 4;
			break;
		default:
			core.uarch.decodeCtx.memSize = 0;
			break;
	}


	switch (code)	{
		case OP_HLT: case OP_SI: case OP_DI: case OP_IRET: case OP_ERET:
			if (GET_PRIV(core.CSTR) == 0b1) {
				// EVT
			} else core.uarch.decodeCtx.iType = S_TYPE;
			break;
		default:
			break;
	}

	extractImm();
	dLog(D_NONE, DSEV_INFO, "decode::Imm: 0x%x", core.uarch.decodeCtx.imm);

	extractRegs();
	dLog(D_NONE, DSEV_INFO, "decode::Rd: %d; Rs: %d; Rr: %d", core.uarch.decodeCtx.rd, core.uarch.decodeCtx.rs, core.uarch.decodeCtx.rr);

	DecodeCtx.regwrite = false;
	DecodeCtx.memwrite = false;

	if (DecodeCtx.iType == I_TYPE || DecodeCtx.iType == R_TYPE || (FetchCtx.opcode >= OP_LD && FetchCtx.opcode <= OP_LDHZ)) DecodeCtx.regwrite = true;
	if (FetchCtx.opcode >= OP_LD && FetchCtx.opcode <= OP_LDHZ) DecodeCtx.memwrite = true;

	decideALUOp();
	dLog(D_NONE, DSEV_INFO, "decode::ALU OP: %d", core.uarch.decodeCtx.aluop);

	regfile(false);
	dLog(D_NONE, DSEV_INFO, "decode::Reg A val: 0x%x; Reg B val: 0x%x", core.uarch.decodeCtx.vala, core.uarch.decodeCtx.valb);
}

static void execute() {
	dLog(D_NONE, DSEV_INFO, "execute::");

	core.uarch.executeCtx.vala = core.uarch.decodeCtx.vala;

	if (core.uarch.decodeCtx.iType == I_TYPE) core.uarch.executeCtx.valb = core.uarch.decodeCtx.imm;
	else core.uarch.executeCtx.valb = core.uarch.decodeCtx.valb;
	dLog(D_NONE, DSEV_INFO, "execute::Val a: 0x%x; Val b: 0x%x", core.uarch.executeCtx.vala, core.uarch.executeCtx.valb);
	alu();
	dLog(D_NONE, DSEV_INFO, "execute::Val res: 0x%x", core.uarch.executeCtx.valres);
}

static void memory() {
	memerr_t err = MEMERR_NONE;

	if (core.uarch.memoryCtx.write) {
		dmem(core.uarch.executeCtx.valres, NULL, &(core.uarch.memoryCtx.valmem), &err);
		dLog(D_NONE, DSEV_INFO, "memory::Writing 0x%x to memory address 0x%x", core.uarch.memoryCtx.valmem, core.uarch.executeCtx.valres);
	}	else {
		dmem(core.uarch.executeCtx.valres, &(core.uarch.executeCtx.valb), NULL, &err);
		dLog(D_NONE, DSEV_INFO, "memory::Read 0x%x from memory address 0x%x", core.uarch.executeCtx.valb, core.uarch.executeCtx.valres);
	}

	if (err == MEMERR_INTERNAL) {
		dLog(D_NONE, DSEV_WARN, "Something went wrong with execute context!");
		fault();
	}

	if (err != MEMERR_NONE && GET_PRIV(core.CSTR) == 0b0) {
		switch (err) {
			case MEMERR_KERN_OVERFLOW:
				dLog(D_NONE, DSEV_WARN, "Detected kernel overflow!");
				break;
			case MEMERR_KERN_OVERREAD:
				dLog(D_NONE, DSEV_WARN, "Detected kernel overread!");
				break;
			case MEMERR_KERN_STACK_OVERFLOW:
				dLog(D_NONE, DSEV_WARN, "Detected kernel stack overflow!");
				break;
			case MEMERR_KERN_HEAP_OVERFLOW:
				dLog(D_NONE, DSEV_WARN, "Detected kernel heap overflow!");
				break;
			case MEMERR_KERN_TEXT_WRITE:
				dLog(D_NONE, DSEV_WARN, "Detected kernel writing to text!");
				break;
			case MEMERR_KERN_SECT_WRITE:
				dLog(D_NONE, DSEV_WARN, "Detected kernel writing to invalid memory!");
				break;
			case MEMERR_KERN_SECT_READ:
				dLog(D_NONE, DSEV_WARN, "Detected kernel reading from invalid memory!");
				break;
			default:
				break;
		}
		fault();
	}


	if (DecodeCtx.memwrite) MemoryCtx.valout = MemoryCtx.valmem;
	else MemoryCtx.valout = ExecuteCtx.valres;

	regfile(DecodeCtx.regwrite);
}

void initCore() {
	for (int i = 0; i < 31; i++) {
		core.GPR[i] = 0x00000000;
	}

	core.IR = 0x00000000;
	core.SP = 0x00000000;

	for (int i = 0; i < 16; i++) {
		core.FPR[i] = 0.0;
	}

	for (int i = 0; i < 6; i++) {
		core.VR[i]._v32[0] = 0x00000000;
		core.VR[i]._v32[1] = 0x00000000;
		core.VR[i]._v32[2] = 0x00000000;
		core.VR[i]._v32[3] = 0x00000000;
	}

	core.CSTR = 0x0000;
}

void* runCore(void* _) {
	dLog(D_NONE, DSEV_INFO, "Executing core thread...");

	int runningCycles = 0;
	int idleCycles = 0;
	while (true) {
		pthread_mutex_lock(&idleLock);
		if (!IDLE) {
			fetch();
			decode();
			execute();
			memory();
			runningCycles++;

			if (runningCycles == 8) {
				IDLE = true;
				pthread_cond_signal(&idleCond);
			}
		} else {
			pthread_mutex_unlock(&idleLock);
			idleCycles++;
			continue;
		}
		pthread_mutex_unlock(&idleLock);
	}

	return NULL;
}

void viewCoreState() {

}