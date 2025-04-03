# AruXI

Welcome to the AruXI Project. This is a project for the Aru32 Architecture on IAru-0 (emulated) processor using Aru Assembly.

The project is setup in the following way:
- `assembler/`: Contains the assembler code
- `emulator/`: Contains the entire CPU emulation
- `headers/*`: Contains the headers for each component (assembler, emulator, kernel)
- `kernel/`: Contains the basic kernel code
- `testsuite/*`: Contains test cases for each component, written in Go


To compile, run `make [all|arxsm|emu|kernel]` at the parent level directory.
All binary will be placed in `out/*`.

To run tests, go the desired component and run `go run tests`.