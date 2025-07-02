# AruXI

Welcome to the AruXI Project. This is a project for the Aru32 Architecture on IAru-0 (emulated) processor using Aru Assembly.

The project is setup in the following way:
- `assembler/`: Contains the assembler code
- `emulator/`: Contains the entire CPU emulation and emulator
- `headers/`: Contains the headers for each component (assembler, emulator, kernel, shell)
- `kernel/`: Contains the basic kernel code
- `shell/`: Contains a minimal shell code
- `testsuite/*`: Contains test cases for each component, written in Go
- `tools/`: Contains tools regarding the file format or execution, like a debugger (`adeb`), dissassembler (`bindump`), or to read the format (`readf`)


To compile, run `make [all|arxsm|ruemu|kernel|shell]` at the parent level directory.
All binary will be placed in `out/*`.

To run tests, go the desired component and run `go run tests`.
Note that on a new terminal session, the ld path must be set.
`export LD_LIBRARY_PATH=..path/AruXI/out:LD_LIBRARY_PATH`