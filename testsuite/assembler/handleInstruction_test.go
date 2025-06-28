package assemblerTests

import (
	"os"
	"testing"

	"github.com/google/go-cmp/cmp"
)


func TestHandleI(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	
	sectTable.activeSection = 2;

	// Test valid instructions
	// @0x0 add X0, X1, #0xff
	t.Run("0", func(t *testing.T) {
		instr := "add"
		args := "X0, X1, #0xff"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x0)
		expectedInstr := "add"
		expectedOperands := []string{"X0", "X1", "#0xff"}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// @0x4 not X5, 	 #10
	t.Run("1", func(t *testing.T) {
		instr := "not"
		args := "X5, 	 #10"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x4)
		expectedInstr := "not"
		expectedOperands := []string{"X5", "#10"}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// @0x8 cmp   X7, #0x4
	t.Run("2", func(t *testing.T) {
		instr := "cmp"
		args := "X7, #0x4"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x8)
		expectedInstr := "cmp"
		expectedOperands := []string{"xz", "X7", "#0x4"}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// @0xc mv X10  , #0xc
	t.Run("3", func(t *testing.T) {
		instr := "mv"
		args := "X10  , #0xc"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0xc)
		expectedInstr := "mv"
		expectedOperands := []string{"X10", "#0xc"}		

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// @0x10 mvn X20	 , #0xff
	t.Run("4", func(t *testing.T) {
		instr := "mvn"
		args := "X20\t , #0xff"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x10)
		expectedInstr := "mvn"
		expectedOperands := []string{"X20", "#0xff"}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// @0x14 nop
	t.Run("5", func(t *testing.T) {
		instr := "nop"
		args := ""

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x14)
		expectedInstr := "nop"
		expectedOperands := []string{"xz", "xz", "#0"}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualInstr := fromCharPGetString(entry.instr)
			if (actualInstr != expectedInstr) {
				t.Errorf("%sExpected instruction: %s, Actual instruction: %s%s", RED, expectedInstr, actualInstr, RESET)
			} else {
				t.Logf("%sActual instruction: %s%s", GREEN, actualInstr, RESET)
			}

			actualOperands := getStringArr(entry.operands)

			if (!cmp.Equal(actualOperands, expectedOperands)) {
				t.Errorf("%sExpected operands: %v, Actual operands: %v%s", RED, expectedOperands, actualOperands, RESET)
			} else {
				t.Logf("%sActual operands: %v%s", GREEN, actualOperands, RESET)
			}
		}
	})

	sectTable.entries[3].lp += 4

	// Test invalid instructions
	// Note that invalid instructions are limited, see comment in `handleIR`
	// @0x18 nop x6, #0x1
	t.Run("6", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "nop"
			args := "x6, #0x1"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/6")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Unexpected operands: `x6`\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x18 add
	t.Run("7", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "add"
			args := ""
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/7")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: No destination register for add!\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x18 mv #0x18
	t.Run("8", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "mv"
			args := "#0x18"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/8")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID REGISTER ERROR: Register #0x18 is not a valid register!\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandeR(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleM(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleBi(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleBu(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleBc(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleS(t *testing.T) {}

func TestHandleNone(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}