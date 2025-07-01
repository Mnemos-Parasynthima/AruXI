% this file uses all I-R instructions in a correct manner
.glob _init

.set IMM0, 	0x0 + 0x1
.set X0R, IMM0


.text
_init:
	bare:
	add x0, x0, #0x1 % add (imm)
	add x1, x1, x0 % add (reg)
	not x2, #0xff % not (imm)
	not x3, x2 % not (reg)
	cmp x3, #0xff % cmp (imm)
	cmp x2, x3 % cmp (reg)
	mv x10, #0xde % mv (imm)
	mv x11, x10 % mov (reg)
	mvn x20, #2 %mvn (imm)
	mvn x21, x20 % mvn (reg)
	nop

	labels:
	add x0, x0, IMM0
	add x1, x1, x0
	not x2, X0R