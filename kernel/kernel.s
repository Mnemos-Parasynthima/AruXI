.glob _init
.glob _usrSetup

.set HEAP_START, 0x20990000
.set HEAP_LIMIT, 0xF007FFFF
.set STACK_START, 0xF0080000
.set STACK_LIMIT, 0xFFFFFFFF

.set USR_STACK_START, 0x709907FF

.text
_init:
	ld sp, =STACK_LIMIT

	nop
	hlt


_usrSetup:
	% sp contains at the limit where argv stops (last argument) placed by the loader/emulator

	ld x0, =STACK_LIMIT-0x4
	ld x1, [x0] % get the user entry point where emulator loader has placed it

	ld x2, [x0, #-0x4] % get argc

	% argv


	% save x1, x2
	sub sp, sp, #8
	str x1, [sp, #0x4] % save entry point
	str x2, [sp] % save argc

	call _setPS

	% restore x1, x2
	ld c0, [sp, #0x4]
	ld x0, [sp]
	add sp, sp, #8

	ubr c0

_setPS:
	% set up the process state

	% get memory for the process state
	% PS takes up 298 bytes (maybe???)

	mv a0, #298
	call _kmalloc
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







_kmalloc:
	% simple heap bump allocator
	% void* _kmalloc(uint32_t size)
	% a0: size
	% xr: void*

	ld x10, KERN_HEAPTOP
	mv xr, x10 % have the previous heaptop be the return
	ld x11, [x10]
	add x11, x11, a0
	str x11, [x10]

	ret





.data
	KERN_HEAPTOP: .word HEAP_START
	KERN_PS: .word 0x0
