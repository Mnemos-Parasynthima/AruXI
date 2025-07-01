% this file uses all B* instructions in a correct manner
.glob _init

.text
FUNC:
	mv xr, #0
	ret

JUMP1:
	mv x5, #0
	mv x6, #1
	cmp x5, x6
	beq EQUAL
	ub NON

_init:
	ub JUMP

JUMP:
	ld x0, JUMP1
	ubr x0

EQUAL:
	call FUNC
	nop

NON:
	mvn x0, #1
	nop