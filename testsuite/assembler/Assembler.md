# Assembler Tests

The go module `assemblerTests` tests the various parts of the `arxsm` assembler.

The organization is as follows:
- `wrapper.go`:
	- Because the `testing` package cannot directly work with `cgo`, this file serves as a bridge.
	- All functions, structs, and other objects in `headers/assembler/*.h` are imported and have a Go function
	wrapper for each c function.
- `eval_test.go`:
	- Tests the `eval()` function. It also ensures proper entries are created and updated.
- `preprocessor_test.go`:
	- Tests the `preprocess()` function.

## Running Test files

If the current working directory is the one this file is found in, that is, `testsuite/assembler/`:
- To run all tests across files: `go test`
- To run all tests in a file: `go test -run [Function]`
	- `[Function]` is the function name following `Test`, like `Eval` in `TestEval`
- To run a single test in a file: `go test -run Test[Name]`