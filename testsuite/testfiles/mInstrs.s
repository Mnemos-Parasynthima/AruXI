% this file uses all M instructions in a correct manner
.glob _init


.text
_init:
	bare:
	ld x0, [ sp ]
	ld x1,[ir, #0x4 -#0x2	]
	ld x2, [	sp], x15



	str x5, [sp]
	str x6, [sp, #0x8]
	str x7, [sp]	 , 	x15

	strb x7, [x20]
	strh x8, [x25],x15
	str x9, [x25] 	,x20
	% strb x20,[x30]	,x20,,	#0x2

	labels:
	ld ir, bare +0x4