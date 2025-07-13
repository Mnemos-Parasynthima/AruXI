#include <string.h>
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
	dLog(D_NONE, DSEV_INFO, "fetch::Fetching from 0x%x...", core.IR);
	imem(core.IR, &FetchCtx.instrbits, &err);

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

	dLog(D_NONE, DSEV_INFO, "fetch::Got 0x%x", FetchCtx.instrbits);
}


void extractImm() {
	uint16_t imm14 = (uint16_t) u32bitextract(FetchCtx.instrbits, 10, 14);
	int32_t simm24 = (int32_t) s32bitextract(FetchCtx.instrbits, 0, 24);
	int32_t simm19 = (int32_t) s32bitextract(FetchCtx.instrbits, 5, 19);
	int16_t simm9 = s32bitextract(FetchCtx.instrbits, 15, 9);

	opcode_t opcode = FetchCtx.opcode;
	itype_t type = NO_TYPE;
	int32_t imm = 0x0;

	if (opcode >= OP_NOP && opcode <= OP_CMP) {
		if (((((FetchCtx.instrbits >> 24) & 0xff) & 0b1) == 0b0) && (opcode <= OP_MUL || opcode >= OP_SDIV)) {
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

	DecodeCtx.imm = imm;
	DecodeCtx.iType = type;
}

void extractRegs() {
	uint8_t _rd = u32bitextract(FetchCtx.instrbits, 0, 5);
	uint8_t _rsIS = u32bitextract(FetchCtx.instrbits, 5, 5);
	uint8_t _rsR = u32bitextract(FetchCtx.instrbits, 10, 5);
	uint8_t _rr = u32bitextract(FetchCtx.instrbits, 5, 5);

	uint8_t rd, rs, rr;

	itype_t type = DecodeCtx.iType;
	opcode_t opcode = FetchCtx.opcode;

	if (opcode == OP_UBR || opcode == OP_RET) {
		rs = _rd; // Bu-Type
		DecodeCtx.iType = BU_TYPE;
	} else if (type == I_TYPE) {
		rd = _rd;
		rs = _rsIS;
	} else if (type == R_TYPE || type == M_TYPE) {
		rd = _rd;
		rs = _rsR;
		rr = _rr;

		if (type == M_TYPE) {
			// There is either `reg, [x]`, `reg, [x, imm]`, or `reg, [x], y`
			// Meaning addition will either be in between x and imm or x and y
			// rd in this context means the register containing the stuff to store
			// rs meaning the base register
			// rr meaning the index register (gets added to base)
		}
	} else if (type == S_TYPE) {
		if (opcode == OP_MVCSTR) {
			rs = _rsIS;
		} else if (opcode == OP_LDIR || opcode == OP_LDCSTR) {
			rd = _rd;
		}
	}

	DecodeCtx.rd = rd;
	DecodeCtx.rs = rs;
	DecodeCtx.rr = rr;
}

void decideALUOp() {
	aluop_t aluop = ALU_PASS;
	switch (FetchCtx.opcode)	{
		// MV, MVN, CMP, and NOP do not appear as they are aliased

		case OP_ADD: case OP_ADDS: case OP_LD: case OP_LDB:
		case OP_LDBS: case OP_LDBZ: case OP_LDH: case OP_LDHS:
		case OP_LDHZ: case OP_STR: case OP_STRB: case OP_STRH:
		case OP_MV: // MV includes both MVI and MVR, which MVR uses OR
		case OP_CALL:
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

	DecodeCtx.aluop = aluop;
}

static void nextIR() {
	// UB only needs imm and IR, all execution happens here
	// UBR/RET only needs to have value of xd from regfile, all execution happens here
	// BCOND needs imm and condval from prev execution, all execution happens here
	// CALL only needs imm and IR; writeback for lr is later
	// 	If before ALU(), place IR contents to vala and 4 to valb
	// 	Aka have CALL intercept since it doesn't have its data in encoding

	if (DecodeCtx.iType == BI_TYPE) {
		if (FetchCtx.opcode == OP_CALL) {
			// Intercept for ALU to do LR := IR + 4
			
			DecodeCtx.rd = 28; // LR
			ExecuteCtx.aluVala = core.IR-4; // undo the +4
			ExecuteCtx.aluValb = 0x4;

			dLog(D_NONE, DSEV_INFO, "execute::call val a: 0x%x; val b: 0x%x; rd: %d", ExecuteCtx.aluVala, ExecuteCtx.aluValb, DecodeCtx.rd);
		}
		// IR += 4 was done automatically after imem, reverse it
		core.IR = (core.IR-4) + ((DecodeCtx.imm & 0xffffff) << 2);
		return;
	}

	if (DecodeCtx.iType == BU_TYPE) {
		// vala comes from rs
		core.IR = DecodeCtx.vala;
		return;
	}

	// else BCOND
	if (ExecuteCtx.cond) core.IR = (core.IR-4) + ((DecodeCtx.imm & 0x7ffff) << 2);
}

static void decode() {
	uint8_t opcode = (FetchCtx.instrbits >> 24) & 0xff;
	opcode_t code = imap[opcode];
	FetchCtx.opcode = code;

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

	if (code == OP_ADDS || code == OP_SUBS || code == OP_CMP) DecodeCtx.setCC = true;
	else DecodeCtx.setCC = false;

	switch (code) {
		case OP_LDB: case OP_LDBS: case OP_LDBZ: case OP_STRB:
			DecodeCtx.memSize = 1;
			break;
		case OP_LDH: case OP_LDHS: case OP_LDHZ: case OP_STRH:
			DecodeCtx.memSize = 2;
			break;
		case OP_LD: case OP_STR:
			DecodeCtx.memSize = 4;
			break;
		default:
			DecodeCtx.memSize = 0;
			break;
	}

	// Subdivide for S-types
	if (code == OP_SYS) {
		DecodeCtx.iType = S_TYPE;
		// get subopcode
		uint8_t subopcode = u32bitextract(FetchCtx.instrbits, 20, 4);
		opcode_t subcode = OP_ERROR;
		switch (subopcode)	{
			case 0b0001: subcode = OP_SYSCALL; break;
			case 0b0011: subcode = OP_HLT; break;
			case 0b0101: subcode = OP_SI; break;
			case 0b0111: subcode = OP_DI; break;
			case 0b1001: opcode = OP_IRET; break;
			case 0b1011: opcode = OP_LDIR; break;
			case 0b1101: opcode = OP_MVCSTR; break;
			case 0b1111: opcode = OP_LDCSTR; break;
			default: break;
		}

		if (subcode == OP_ERROR) {
			if (GET_PRIV(core.CSTR) == 0b0) { // Kill for kernel code
				dLog(D_NONE, DSEV_WARN, "Invalid instruction: 0x%x!", opcode);
				fault();
			} else { // EVT for user code
				
			}
		}

		if (subcode != OP_SYSCALL) {
			// Syscall is the only S-type that is unprivileged (for now??), the rest are privileged
			if (GET_PRIV(core.CSTR) == 0b1) {
				// EVT
			}
		}

		FetchCtx.opcode = subcode;
	}

	extractImm();
	dLog(D_NONE, DSEV_INFO, "decode::Imm: 0x%x", DecodeCtx.imm);

	extractRegs();
	dLog(D_NONE, DSEV_INFO, "decode::Rd: %d; Rs: %d; Rr: %d", DecodeCtx.rd, DecodeCtx.rs, DecodeCtx.rr);

	DecodeCtx.regwrite = false;
	DecodeCtx.memwrite = false;

	if (DecodeCtx.iType == I_TYPE || DecodeCtx.iType == R_TYPE || (FetchCtx.opcode >= OP_LD && FetchCtx.opcode <= OP_LDHZ) || FetchCtx.opcode == OP_CALL) DecodeCtx.regwrite = true;
	if (FetchCtx.opcode >= OP_LD && FetchCtx.opcode <= OP_LDHZ) DecodeCtx.memwrite = true;

	if (FetchCtx.opcode >= OP_STR && FetchCtx.opcode <= OP_STRH) MemoryCtx.write = true;
	else MemoryCtx.write = false;

	decideALUOp();
	dLog(D_NONE, DSEV_INFO, "decode::ALU OP: %s", ALUOP_STR[DecodeCtx.aluop]);

	regfile(false);
	dLog(D_NONE, DSEV_INFO, "decode::Reg A val: 0x%x; Reg B val: 0x%x", DecodeCtx.vala, DecodeCtx.valb);

	// Val ex for M types contain
}

static void execute() {
	ExecuteCtx.aluVala = DecodeCtx.vala;
	
	if (DecodeCtx.iType == I_TYPE || DecodeCtx.iType == M_TYPE) ExecuteCtx.aluValb = DecodeCtx.imm;
	else ExecuteCtx.aluValb = DecodeCtx.valb;

	// M types can either use imm as aluvalb or valb (index reg) as aluvalb
	if (DecodeCtx.iType == M_TYPE) {
		if (DecodeCtx.imm == 0) {
			// It is difficult to detect the usage based off on the bits (for now, maybe use some bits as indication???)
			// On assembling, if no offset is used, it is stored as 0, if no index is used, it is stored as x30
			// meaning it could go like xd, [x, #0], [x30]
			// Zero will be added to anyway
			// Thus assume if no imm (#0), it uses index (rr/valb)
			// If by coincidence, imm is #0 (no much point but for clarity can be done), the presence of x30 takes care of it
			ExecuteCtx.aluValb = DecodeCtx.valb;
		}
		// Else is aluValb = imm, which can be done in earlier code
	}

	nextIR();
	dLog(D_NONE, DSEV_INFO, "execute::Next IR: 0x%x", core.IR);
	dLog(D_NONE, DSEV_INFO, "execute::ALU Val a: 0x%x; ALU Val b: 0x%x;", ExecuteCtx.aluVala, ExecuteCtx.aluValb);

	alu();
	dLog(D_NONE, DSEV_INFO, "execute::Val res: 0x%x", ExecuteCtx.alures);

	if (FetchCtx.opcode >= OP_STR && FetchCtx.opcode <= OP_STRH) MemoryCtx.valmem = DecodeCtx.valex;

	// fpu();
	// vcu();
}

static void memory() {
	// if (FetchCtx.opcode >= OP_STR && FetchCtx.opcode <= OP_STRH) {
	// 	Mem
	// }

	memerr_t err = MEMERR_NONE;

	if (MemoryCtx.write) {
		dLog(D_NONE, DSEV_INFO, "memory::Writing 0x%x to memory address 0x%x", MemoryCtx.valmem, ExecuteCtx.alures);
		dmem(ExecuteCtx.alures, NULL, &(MemoryCtx.valmem), &err);
	}	else {
		dmem(ExecuteCtx.alures, &(ExecuteCtx.aluValb), NULL, &err);
		dLog(D_NONE, DSEV_INFO, "memory::Read 0x%x from memory address 0x%x", ExecuteCtx.aluValb, ExecuteCtx.alures);
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


	if (DecodeCtx.memwrite) MemoryCtx.valout = ExecuteCtx.aluValb;
	else MemoryCtx.valout = ExecuteCtx.alures;

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

	memset(&core.uarch, 0x0, sizeof(InstrCtx));
}

void* runCore(void* _) {
	dLog(D_NONE, DSEV_INFO, "Executing core thread...");

	int runningCycles = 0;
	int idleCycles = 0;
	while (true) {
		pthread_mutex_lock(&idleLock);
		if (!IDLE) {
			dLog(D_NONE, DSEV_INFO, "\nCycle %d", runningCycles);
			fetch();
			decode();
			execute();
			memory();
			runningCycles++;

			// if (runningCycles == 68) {
			// 	IDLE = true;
			// 	pthread_cond_signal(&idleCond);
			// }
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