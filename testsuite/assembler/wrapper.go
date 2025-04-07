package assemblerTests

/*
#cgo LDFLAGS: -L../../out -larxsm
#include <stdlib.h>
#include "../../headers/assembler/DataTable.h"
#include "../../headers/assembler/evaluator.h"
#include "../../headers/assembler/InstructionStream.h"
#include "../../headers/assembler/lexer-parser.h"
#include "../../headers/assembler/preprocessor.h"
#include "../../headers/assembler/SymbolTable.h"
*/
import "C"
import "unsafe"

func arxsmEval(expr string, symbTable *C.SymbolTable) int32 {
 cExpr := C.CString(expr)
 defer C.free(unsafe.Pointer(cExpr))

 var canEval C.bool

 res := C.eval(cExpr, symbTable, &canEval)
 return int32(res)
}


func arxsmInitSymbTable() *C.SymbolTable {
	return C.initSymbTable()
}

func arxsmDeleteSymbTable(symbTable *C.SymbolTable) {
	C.deleteSymbTable(symbTable)
}