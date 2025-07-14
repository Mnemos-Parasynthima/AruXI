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
	ldir c2 % get the IR contents
	% IR is for the following instruction as on fetch after ldir, the advancing by 4 is done already
	add c2, c2, #8
	% save the instruction after ubr
	mv lr, c2

	% set the user stack pointer
	ld sp, =USR_STACK_START-#0x4

	% switch to user mode, this needs to be done right before the call
	ldcstr x10
	mv x11, #0x1
	lsl x11, x11, #9 % PRIV flag is in bit 9
	or x10, x10, x11
	mvcstr x10
	ubr c1 % run user program

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


_setPS:
	% set up the process state

	% get memory for the process state
	% PS takes up 298 bytes (maybe???)

	mv a0, #298
	% save LR
	sub sp, sp, #4
	str lr, [sp]
	call _kmalloc
	% restore lr
	ld lr, [sp]
	add sp, sp, #4
	% xr contains pointer to memory block for PS
	% save it
	ld c1, =KERN_PS
	str xr, [c1]

	mv c0, #0x0 % ignore PIDs for now
	str xz, [xr]
	str xz, [xr, #0x1] % no threads for now
	str xz, [xr, #0x2] % no threads
	ld c1, =USR_STACK_START
	str c1, [xr, #0x3] % user sp
	str x1, [xr, #0x7] % user ir
	% save registers

	ret


_destroyPS:
	% destroy the process state
	% basically just free the memory from pointer
	ld a0, =KERN_PS % get the stored PS pointer
	mv a1, #298
	call _kfree

	% "null" KERN_PS
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

	ld c0, =KERN_HEAPTOP
	ld c1, [c0]
	sub c1, c1, a1
	str c1, [c0]

	ret



.data
	KERN_HEAPTOP: .word HEAP_START
	KERN_PS: .word 0x0

	KERN_STATE:
		% Saved sp of the kernel stack to return after user program
		KERN_STATE_SP: .word STACK_LIMIT 