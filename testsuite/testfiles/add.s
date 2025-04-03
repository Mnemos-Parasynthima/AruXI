.glob _init
.set _sy, 0xa0

.data
	obj:
		.byte _sy, _sy + 0x2, _sy + 0x4

.const
	helloWorld: .string "Hello World"
	.set strLen, @-helloWorld % strLen includes null char

.bss
	buffer: .zero strLen, 1

.text
	_init:
		ld x0, helloWorld
		ldb x1, [x0]
		add x1, x1, #1
		ld x2, buffer
		strb x1, [x2]
		ret