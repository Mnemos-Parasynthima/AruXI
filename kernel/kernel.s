.glob _init
.glob _usrSetup

.set HEAP_START, 0xD0080000
.set HEAP_LIMIT, 0xF007FFFF
.set STACK_START, 0xF0080000
.set STACK_LIMIT, 0xFFFFFFFF

.set USR_STACK_START, 0x709907FF

.text
_init:
	% the init section is to take a total of 8 4-bytes (for now)
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	hlt


_usrSetup:
	% loader/emulator placed user entry point at the very bottom of the stack (aka at the limit)
	% however it did not update the sp (cannot even do it), but it is to be guaranteed that
	% it is guaranteed to be at STACK_LIMIT-4
	ld sp, =STACK_LIMIT-#0x4


	call _setPS

	% save kernel sp
	ld c1, =KERN_STATE_SP
	str sp, [c1]

	% a call is to be done but the address is known in stack, move it to a reg
	ld c1, [sp]
	% c1 contains the entry point, before a simulated call, set the link register so
	% on user return, it comes back here
	% save the instruction after ubr
	ld lr, =USER_RET_AT

	% set the user stack pointer
	ld sp, =USR_STACK_START-#0x4

	% switch to user mode, this needs to be done right before the call
	% CSTR = CSTR & ~(1<<9)
	ldcstr x10
	mv x11, #0x1
	lsl x11, x11, #9 % PRIV flag is in bit 9
	not x11, x11
	and x10, x10, x11
	mvcstr x10
	ubr c1 % run user program
	.set USER_RET_AT @
	% restore sp
	ld sp, KERN_STATE_SP
	% return value of user program is in xr
	sub sp, sp, #4
	str xr, [sp]

	% remove PS
	call _destroyPS

	ld xr, [sp]
	add sp, sp, #4

	hlt


.set PS_SIZE, 570
_setPS:
	% set up the process state

	% get memory for the process state
	% PS takes up 570 bytes

	mv a0, PS_SIZE
	% save LR
	sub sp, sp, #4
	str lr, [sp]
	call _kmalloc
	% restore lr
	ld lr, [sp]
	add sp, sp, #4
	% xr contains pointer to memory block for PS
	% save it
	ld c1, =PS_PTR
	str xr, [c1]

	strb xz, [xr] % ignore PIDs for now; PS.pid
	strb xz, [xr, #1] % no threads for now; PS.threadc
	str xz, [xr, #2] % no threads; PS.threadStates
	ld c1, =USR_STACK_START
	str c1, [xr, #6] % user sp; PS.sp
	str x1, [xr, #10] % user ir; PS.ir

	mv c2, #566 % offset is too large for mem ops (9 bits), use index mode
	str xz, [xr], c2 % excpType

	ret


_destroyPS:
	% destroy the process state
	% basically just free the memory from pointer
	ld a0, =PS_PTR % get the stored PS pointer
	mv a1, PS_SIZE

	sub sp, sp, #4
	str lr, [sp]
	call _kfree
	ld lr, [sp]
	add sp, sp, #4

	% "null" PS_PTR
	str xz, [a0]

	ret


%% HEAP ALLOCATING %%

_kmalloc:
	% simple heap bump allocator
	% void* _kmalloc(uint32_t size)
	% a0: size
	% xr: void*

	ld x10, KERN_HEAPTOP
	mv xr, x10 % have the previous heaptop be the return

	% get the actual address to update it by size
	ld x10, =KERN_HEAPTOP
	ld x11, [x10]
	add x11, x11, a0
	str x11, [x10]

	ret

_kfree:
	% simple free-er
	% void _kfree(uint32_t* ptr, uint32_t size)
	% a0: ptr
	% a1: size (temporary until more structured memory allocation)

	ld c1, =KERN_HEAPTOP
	ld c2, [c1]
	sub c2, c2, a1
	str c2, [c1]

	ret




	%% EVT HANDLERS %%
	.set STDOUT, 0
_writeHndlr:
	% _write(uint32_t count, const char* buffer)

	% get memory for struct
	% takes up 12 bytes

	sub sp, sp, #8
	str lr, [sp]
	str a0, [sp, #4]

	mv a0, #12
	call _kmalloc
	mv x10, xr

	ld a0, [sp, #4]
	ld lr, [sp]
	add sp, sp, #4

	% refer to documentation for structure layout
	mv c0, STDOUT
	str c0, [x10, #0] % fd
	str a0, [x10, #4] % count
	str a1, [x10, #8] % buffer

	% in order to tell the cpu IO request, use CSTR bit 13
	% CSTR = CSTR | (1<<13)
	ldcstr c0
	mv c1, #1
	lsl c1, c1, #13
	or c0, c0, c1
	mvcstr c0
	hlt

	% clear bit
	% it is guaranteed registers are not changed
	not c1, c1
	and c0, c0, c1
	mvcstr c0
	% bit 13 should be cleared

	% free memory
	mv a0, a1
	mv a1, #12
	sub sp, sp, #4
	str lr, [sp]
	call _kfree
	ld lr, [sp]
	add sp, sp, #4

	ub HNDLR_END


	.set STDIN, 1
_readHndlr:
	nop

	ub HNDLR_END



HNDLR_END:
	ld c1, PS_PTR

	% restore
	ld sp, [c1, #6]
	ld x0, [c1, #18]
	ld x1, [c1, #22]
	ld x2, [c1, #26]
	ld x3, [c1, #30]
	ld x4, [c1, #34]
	ld x5, [c1, #38]
	ld x6, [c1, #42]
	ld x7, [c1, #46]
	ld x8, [c1, #50]
	ld x9, [c1, #54]
	ld x10, [c1, #58]
	ld x11, [c1, #62]
	ld x17, [c1, #66]
	ld x18, [c1, #70]
	ld x19, [c1, #74]
	ld x20, [c1, #78]
	ld x21, [c1, #82]
	ld x22, [c1, #86]
	ld x23, [c1, #90]
	ld x24, [c1, #94]
	ld x25, [c1, #98]
	ld x26, [c1, #102]
	ld x27, [c1, #106]
	ld x28, [c1, #110]
	ld x29, [c1, #114]

	eret


_exitHndlr:
	% basic exit
	% restore sp
	ld sp, KERN_STATE_SP
	% return value of user program is in xr
	sub sp, sp, #4
	str xr, [sp]

	% remove PS
	call _destroyPS

	ld xr, [sp]
	add sp, sp, #4

	hlt


_excpHndlr1:
	% for now, place non-0 in PS.excpType
	ld c1, PS_PTR
	mv c2, #0b10 % FETCH ABORT
	mv c0, #566
	strb c2, [c1], c0
	hlt

_excpHndlr0:
_excpHndlr2:
	% for now, place non-0 in PS.excpType
	ld c1, PS_PTR
	mv c2, #0b01 % DATA ABORT
	mv c0, #566
	strb c2, [c1], c0
	hlt


.data
	KERN_HEAPTOP: .word HEAP_START
	PS_PTR: .word 0x0

	KERN_STATE:
		% Saved sp of the kernel stack to return after user program
		KERN_STATE_SP: .word STACK_LIMIT





.evt
	EVT_START:
	% save cpu context
	ld c1, PS_PTR % note that Cx are truly volatile, meaning no piece of code can assume Cx are safe from overwriting	
	% ir was saved by cpu
	% PS->sp
	str sp, [c1, #6]

	% restore kernel sp
	ld sp, KERN_STATE_SP

	ldcstr c0
	str c0, [c1, #14]

	% PS->grp[i]
	str x0, [c1, #18]
	str x1, [c1, #22]
	str x2, [c1, #26]
	str x3, [c1, #30]
	str x4, [c1, #34]
	str x5, [c1, #38]
	str x6, [c1, #42]
	str x7, [c1, #46]
	str x8, [c1, #50]
	str x9, [c1, #54]
	str x10, [c1, #58]
	str x11, [c1, #62]
	str x17, [c1, #66]
	str x18, [c1, #70]
	str x19, [c1, #74]
	str x20, [c1, #78]
	str x21, [c1, #82]
	str x22, [c1, #86]
	str x23, [c1, #90]
	str x24, [c1, #94]
	str x25, [c1, #98]
	str x26, [c1, #102]
	str x27, [c1, #106]
	str x28, [c1, #110]
	str x29, [c1, #114]


	% get the offset based off on exception number
	resr c0
	% save it as well
	str c0, [c1, #16]

	% if RESR is 0x0, it is a syscall, refer to a0 for offset
	% else if it an exception, use the RESR contents
	cmp c0, #0x0
	bne offsetFromExecp

	% offset into table is done as follows:
	% IR := EVT_BASE + SIZE_OF_HEADER_CODE + (INDEX * SIZE_OF_EVT_ENTRY)


	offsetFromSyscall:
	mv c0, a0

	offsetFromExecp:
	% as is

	calculateOffset:
	mv c4, #8
	mul c2, c0, c4 % INDEX * SIZE_OF_EVT_ENTRY (8 bytes)
	add c2, c2, HEADER_CODE_SIZE % + SIZE_OF_HEADER_CODE
	ld c1, =#0x00040000
	add c2, c2, c1 % + EVT_BASE

	ld c0, [c2]
	ubr c0
	.set HEADER_CODE_SIZE, @-EVT_START + 4

	%% BEGIN EVT %%
	.byte 0b00000000 % syscall for read
	.hword 0x0000 % unused
	.byte 0x00 % unused
	.word _readHndlr

	.byte 0b00000001 % syscall for write
	.hword 0x0000
	.byte 0x00
	.word _writeHndlr

	.byte 0b00000010
	.hword 0x0000
	.byte 0x00
	.word _exitHndlr

	% an entry is 8 bytes, 9 entries between write and first exception, 9 * 8 = 72
	.zero 72
	% ....

	.byte 0b01001100 % exception for invalid access
	.hword 0x0000 % unused
	.byte 0x00 % unused
	.word _excpHndlr0

	.byte 0b10001101 % exception for invalid instruction
	.hword 0x0000 % unused
	.byte 0x00 % unused
	.word _excpHndlr1

	.byte 0b01001110 % exception for privilege use
	.hword 0x0000 % unused
	.byte 0x00 % unused
	.word _excpHndlr2