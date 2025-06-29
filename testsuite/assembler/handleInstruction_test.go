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

	// @0x18 xor x15, x1, XORD
	t.Run("6", func(t *testing.T) {
		instr := "xor"
		args := "x15, x1, XORD"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x18)
		expectedInstr := "xor"
		expectedOperands := []string{"x15", "x1", "XORD"}

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

	// @0x1c mv x20, IMM
	t.Run("7", func(t *testing.T) {
		instr := "mv"
		args := "x20, IMM"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x1c)
		expectedInstr := "mv"
		expectedOperands := []string{"x20", "IMM"}

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

	// @0x20 cmp X14	,	 LA
	t.Run("8", func(t *testing.T) {
		instr := "cmp"
		args := "X14\t , LA"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x20)
		expectedInstr := "cmp"
		expectedOperands := []string{"xz", "X14", "LA"}

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

	// Test invalid instructions
	// Note that invalid instructions are limited, see comment in `handleIR`
	// @0x20 nop x6, #0x1
	t.Run("9", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "nop"
			args := "x6, #0x1"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/9")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Unexpected operands: `x6`\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x20 add
	t.Run("10", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "add"
			args := ""
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/10")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: No destination register for add!\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x20 mv #0x18
	t.Run("11", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "mv"
			args := "#0x18"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
		}

		stdout, stderr, err := runFork(t, "HandleI/11")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID REGISTER ERROR: Register #0x18 is not a valid register!\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Register Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x20 mvn X4
	t.Run("12", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "mvn"
			args := "x4"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
			os.Stderr.WriteString("UNREACHABLE: Instruction added")
			os.Exit(1)
		}

		stdout, stderr, err := runFork(t, "HandleI/12")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Expected operands, got nothing\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x20 add X4, x10, x2, #0x10
	t.Run("13", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "add"
			args := "X4, x10, x2, #0x10"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
			os.Stderr.WriteString("UNREACHABLE: Instruction added")
			os.Exit(1)
		}

		stdout, stderr, err := runFork(t, "HandleI/13")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Unexpected operands: `#0x10`\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	arxsmDeleteInstrStream(instrStream)
	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}

func TestHandleR(t *testing.T) {
	instrStream := arxsmInitInstrStream()
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	sectTable.activeSection = 2;

	// Test valid instructions
	// @0x0 add X0, X1, X2
	t.Run("0", func(t *testing.T) {
		instr := "add"
		args := "X0, X1, X2"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x0)
		expectedInstr := "add"
		expectedOperands := []string{"X0", "X1", "X2"}

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

	// @0x4 not X5, 	 X10
	t.Run("1", func(t *testing.T) {
		instr := "not"
		args := "X5, 	 X10"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x4)
		expectedInstr := "not"
		expectedOperands := []string{"X5", "X10"}

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

	// @0x8 cmp   X7, x4
	t.Run("2", func(t *testing.T) {
		instr := "cmp"
		args := "X7, x4"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x8)
		expectedInstr := "cmp"
		expectedOperands := []string{"xz", "X7", "x4"}

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

	// @0xc mv X10  , xr
	t.Run("3", func(t *testing.T) {
		instr := "mv"
		args := "X10  , xr"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0xc)
		expectedInstr := "mv"
		expectedOperands := []string{"X10", "xr"}

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

	// @0x10 mvn X20	 , x0
	t.Run("4", func(t *testing.T) {
		instr := "mvn"
		args := "X20\t , x0"

		arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)

		entry := arxsmGetInstr(instrStream, 0x10)
		expectedInstr := "mvn"
		expectedOperands := []string{"X20", "x0"}

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

	// Test invalid instructions
	// Note that invalid instructions are limited, see comment in `handleIR`
	// @0x10 mvn X4
	t.Run("5", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "mvn"
			args := "x4"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
			os.Stderr.WriteString("UNREACHABLE: Instruction added")
			os.Exit(1)
		}

		stdout, stderr, err := runFork(t, "HandleR/5")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Expected operands, got nothing\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

	// @0x10 add X4, x10, x2, x4
	t.Run("6", func(t *testing.T) {
		if (os.Getenv("FORK") == "1") {
			instr := "add"
			args := "X4, x10, x2, x4"
			
			arxsmHandleInstruction(instrStream, symbTable, sectTable, instr, args)
			os.Stderr.WriteString("UNREACHABLE: Instruction added")
			os.Exit(1)
		}

		stdout, stderr, err := runFork(t, "HandleR/6")
		_ = stdout

		if (err.Error() == "exit status 255" && stderr == "\x1b[31mINVALID SYNTAX ERROR: Unexpected operands: `x4`\n\x1b[0m") {
			t.Logf("%sGot %s with error %s%s", GREEN, err.Error(), stderr, RESET)
		} else {
			t.Errorf("%sExpected Invalid Syntax Error, got %s%s", RED, stderr, RESET)
		}
	})

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