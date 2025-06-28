package assemblerTests

/*
#cgo LDFLAGS: -L../../out -larxsm
#include <stdlib.h>
#include "../../headers/assembler/DataTable.h"
#include "../../headers/assembler/evaluator.h"
#include "../../headers/assembler/InstructionStream.h"
#include "../../headers/assembler/lexer-parser.h"
#include "../../headers/assembler/preprocessor.h"
#include "../../headers/assembler/SectionTable.h"
#include "../../headers/assembler/SymbolTable.h"
*/
import "C"
import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"testing"
	"unsafe"
)

// DataTable.c/h

func arxsmInitDataTable() *C.DataTable {
	return C.initDataTable()
}

func arxsmGetDataEntry(dataTable *C.DataTable, sectType C.data_sect_type_t, addr C.uint32_t) *C.data_entry_t {
	return C.getDataEntry(dataTable, sectType, addr)
}

func arxsmDisplayDataTable(dataTable *C.DataTable) {
	C.displayDataTable(dataTable)
}

func arxsmDeleteDataTable(dataTable *C.DataTable) {
	C.deleteDataTable(dataTable)
}

// evaluator.c/h

func arxsmEval(expr string, symbTable *C.SymbolTable) (int32, bool) {
	cExpr := C.CString(expr)
	defer C.free(unsafe.Pointer(cExpr))

	var canEval C.bool = true

	res := C.eval(cExpr, symbTable, &canEval)
	return int32(res), bool(canEval)
}

// InstructionStream.c/h

func arxsmInitInstrStream() *C.InstructionStream {
	return C.initInstrStream()
}

func arxsmGetInstr(instrStream *C.InstructionStream, addr C.uint32_t) *C.instr_obj_t {
	return C.getInstr(instrStream, addr)
}

func arxsmDisplayInstrStream(instrStream *C.InstructionStream) {
	C.displayInstrStream(instrStream, false)
}

func arxsmDeleteInstrStream(instrStream *C.InstructionStream) {
	C.deleteInstrStream(instrStream)
}

// lexer-parser.c/h

func arxsmHandleLabel(symbTable *C.SymbolTable, sectTable *C.SectionTable, tok string, save string) (string, string) {
	cTok := C.CString(tok)
	cSave := C.CString(save)

	defer C.free(unsafe.Pointer(cTok))
	defer C.free(unsafe.Pointer(cSave))

	C.handleLabel(symbTable, sectTable, &cTok, &cSave)

	return C.GoString(cTok), C.GoString(cSave)
}

func arxsmHandleDirective(symbTable *C.SymbolTable, sectTable *C.SectionTable, dataTable *C.DataTable, directive string, args string) {
	cDirective := C.CString(directive)
	cArgs := C.CString(args)

	defer C.free(unsafe.Pointer(cDirective))
	// All other handlers that use args create new data
	// Except in setDirective where after strtok on args for the second time for the expr
	// `expr` is set directly to the symbol entry (see comment on arxsmInitSymbEntry())
	// So args cannot be freed, although changes will be made to make expr be allocd
	// defer C.free(unsafe.Pointer(cArgs))

	C.handleDirective(symbTable, sectTable, dataTable, cDirective, cArgs)
}

func arxsmHandleInstruction(instrStream *C.InstructionStream, symbTable *C.SymbolTable, sectTable *C.SectionTable, instr string, args string) {
	cInstr := C.CString(instr)
	cArgs := C.CString(args)

	defer C.free(unsafe.Pointer(cInstr))
	// I will be assuming that anything in args will be allocated by sub-handlers
	// Meaning I am assuming freeing will be save
	defer C.free(unsafe.Pointer(cArgs))

	C.handleInstruction(instrStream, symbTable, sectTable, cInstr, cArgs)
}

// preprocessor.c/h

func arxsmPreprocess(line string, len int64) string {
	cLine := C.CString(line)
	defer C.free(unsafe.Pointer(cLine))

	res := C.preprocess(cLine, C.ssize_t(len))
	return C.GoString(res)
}

// SectionTable.c/h

func arxsmInitSectionTable() *C.SectionTable {
	return C.initSectionTable()
}

func arxsmDisplaySectionTable(sectTable *C.SectionTable) {
	C.displaySectionTable(sectTable)
}

func arxsmDeleteSectionTable(sectTable *C.SectionTable) {
	C.deleteSectionTable(sectTable)
}

// SymbolTabel.c/h

func arxsmInitSymbTable() *C.SymbolTable {
	return C.initSymbTable()
}

func arxsmInitSymbEntry(name string, expr string, value int32, flags uint32) *C.symb_entry_t {
	cName := C.CString(name)
	cExpr := C.CString(expr)
	defer C.free(unsafe.Pointer(cName))
	// Needed to comment it out as `initSymbEntry` assumes `expr` has been allocd and uses the pointer passed in
	// Keeping this would free it, leading to segfaults when later attempts on accessing `expr`
	// defer C.free(unsafe.Pointer(cExpr))

	return C.initSymbEntry(cName, cExpr, C.int32_t(value), C.uint32_t(flags))
}

func arxsmAddSymbEntry(symbTable *C.SymbolTable, symbEntry *C.symb_entry_t) {
	C.addSymbEntry(symbTable, symbEntry)
}

func arxsmGetSymbEntry(symbTable *C.SymbolTable, name string) *C.symb_entry_t {
	cName := C.CString(name)
	defer C.free(unsafe.Pointer(cName))

	return C.getSymbEntry(symbTable, cName)
}

func arxsmDisplaySymbTable(symbTable *C.SymbolTable) {
	C.displaySymbTable(symbTable)
}

func arxsmDeleteSymbTable(symbTable *C.SymbolTable) {
	C.deleteSymbTable(symbTable)
}

// END
// utils

const (
	RESET  = "\033[0m"
	RED    = "\033[31m"
	GREEN  = "\033[32m"
	YELLOW = "\033[33m"
)

func fromSliceGetString(str [8]byte) string {
	cStr := (**C.char)(unsafe.Pointer(&str[0]))

	return C.GoString(*cStr)
}

func fromCharPGetString(str *C.char) string {
	return C.GoString(str)
}

func getFloatArr(fArr [8]byte, count int) []float32 {
	cFloat := *(**C.float)(unsafe.Pointer(&fArr[0]))

	return unsafe.Slice((*float32)(unsafe.Pointer(cFloat)), count)
}

func getStringArr(sArr **C.char) []string {
	var arr []string
	temp := sArr

	for *temp != nil {
		if *temp != (*C.char)(unsafe.Pointer(uintptr(0xFEEDFAED))) {
			arr = append(arr, C.GoString(*temp))
		}
		temp = (**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(temp)) + unsafe.Sizeof(*sArr)))
	}

	return arr
}

// https://willsena.dev/golang-how-to-test-code-that-exits-or-crashes/
func runFork(t *testing.T, testName string) (string, string, error) {
	cmd := exec.Command(os.Args[0], fmt.Sprintf("-test.run=%v", testName))
	cmd.Env = append(os.Environ(), "FORK=1")

	var stdoutB, stderrB bytes.Buffer
	cmd.Stdout = &stdoutB
	cmd.Stderr = &stderrB

	err := cmd.Run()

	return stdoutB.String(), stderrB.String(), err
}