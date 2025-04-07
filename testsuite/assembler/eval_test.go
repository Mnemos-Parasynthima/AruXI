package assemblerTests

import "testing"


var symbTable *_Ctype_SymbolTable


// func TestMain(m *testing.M) {
// 	symbTable = arxsmInitSymbTable()

// 	symbEntry := arxsmInitSymbEntry("size", "2+3+(0x2*4)", 13, 0b00001001)

// 	arxsmAddSymbEntry(symbTable, symbEntry)

// 	fmt.Printf("Current state of the symbol entry at Tests Entry:\n")	
// 	arxsmDisplaySymbTable(symbTable)

// 	m.Run()

// 	arxsmDeleteSymbTable(symbTable)
// }


func TestEvalBasic(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := "1 + 2 * 3"
		expected := 7
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "2+3+(0x2*4)"
		expected := 13
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "4	+8/2"
		expected := 8
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "2*3	+4"
		expected := 10
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "4*(1+2	)"
		expected := 12
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("5", func(t *testing.T) {
		input := "0xa0 + 0x1 * (3/2) + ((2*1)+3*(4/2))"
		expected := 169
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("6", func(t *testing.T) {
		input := "3 	+ 5 *(2			-	8)"
		expected := -27
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})
}

func TestEvalWithUnary(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := "-2+3*(~2 + -4) / (2 + -3)"
		expected := 19
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "- 12/	(4 + 2*	-	3)"
		expected := 6
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "(7-4) * ~ (5 - 3)"
		expected := -9
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "6 * (-(3  + 1) / 2)"
		expected := -12
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "(	(2+3) * (4	-2)) - (8 	/4)"
		expected := 8
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})
}

func TestEvalWithShifts(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := "((	2+ 3)<< 1) - (8	>>2)"
		expected := 8
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "6<< (2+1) + (10	-5) >> 2"
		expected := 384
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "6<< (2+1) + (10	-5) >> 2"
		expected := 384
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "((8 +4)>>	2)+((6 <<1) - (3+	1))"
		expected := 11
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "((5-3)<< (4-2	)) - (7>>	1)"
		expected := 5
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})
}

func TestEvalWithBitwise(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := "( (5 & 3)^ (8 |2)) - (6&	4)"
		expected := 7
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := " ((	(3|1) &2) ^ (4 & 2)) + (7   ^ 3)"
		expected := 6
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "(5 | (3 & (8^2))) - (6 & (4|1)	)"
		expected := 3
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "(((2 | 3) & (4 ^ 1)) ^ ((6 & 3) * 2)) - (7 | 8)"
		expected := -10
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := " ((7 ^ 5 | 3) & 4) + (6 ^ 2)"
		expected := 4
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})
}

func TestEvalArith(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := " ((5 + ~(3 << 1)) ^ 	(12>> 2)) - (6 & (4|1))"
		expected := -7
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "((-8 >> 1) + ~(4|3) ^((6&2) << 2))"
		expected := -4
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := " (((7- 3) * 2) & (~(5 ^ 2))) | ( (8/2) + 6)"
		expected := 10
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "( ( (2|1) & ~(3<<1) ) ^ (4+(-6)) ) + (-(7 >> 2))"
		expected := -2
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "( ( (-8-4)>> 1 ) | (6 * (~(3+1)) ) ) ^ ((5 & 3) / 2)"
		expected := -6
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("5", func(t *testing.T) {
		input := " ( ~((3<<2)|5) & (6 ^ (3*4)) ) + ((8 >> 4) - ( 4&1 ))"
		expected := 2
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("6", func(t *testing.T) {
		input := "(( 5 & ~(2 - 3)) ^ (8 | 1 >> 2)) + (-7 << 3*2)"
		expected := -440
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})
}

func TestEvalSymbDefined(t *testing.T) {
	symbTable = arxsmInitSymbTable()

	// .set label1 14 @ data, local, no reference
	symb1 := arxsmInitSymbEntry("symb1", "", 14, 0b00001001)
	arxsmAddSymbEntry(symbTable, symb1)

	// LABEL1: @ text, local, no reference
	label1 := arxsmInitSymbEntry("label1", "", 0x2004, 0b00110001)
	arxsmAddSymbEntry(symbTable, label1)

	// LABEL2: @ text, global, reference
	label2 := arxsmInitSymbEntry("label2", "", 0x2020, 0b00110111)
	arxsmAddSymbEntry(symbTable, label2)

	// LABEL3: @ data, global, reference
	label3 := arxsmInitSymbEntry("label3", "", 0x200a2400, 0b00000111)
	arxsmAddSymbEntry(symbTable, label3)

	arxsmDisplaySymbTable(symbTable)

	t.Run("0", func(t *testing.T) {
		input := "2 + (symb1 << 5)"
		expected := 450
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}

		reference := ((symb1.flags>>1) & 0b1)

		if reference != 0b1 {
			t.Errorf("%sExpected Reference Bit set%s", RED, RESET)
		} else {
			t.Logf("%sReference Bit set%s", GREEN, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "label1 + (0x4 * 2)"
		expected := 0x200c
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}

		reference := ((label1.flags>>1) & 0b1)

		if reference != 0b1 {
			t.Errorf("%sExpected Reference Bit set%s", RED, RESET)
		} else {
			t.Logf("%sReference Bit set%s", GREEN, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "label1"
		expected := 0x2004
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "symb1"
		expected := 14
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "label1 + symb1"
		expected := 0x2012
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("5", func(t *testing.T) {
		input := "(label1 + symb1) + 0x2"
		expected := 0x2014
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("6", func(t *testing.T) {
		input := "symb1 >> 2 * 1"
		expected := 3
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("7", func(t *testing.T) {
		input := "~symb1"
		expected := -15
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("8", func(t *testing.T) {
		input := "(label1 + label2) << 2"
		expected := 0x10090
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("9", func(t *testing.T) {
		input := "(label1 + label2) + (symb1+2)"
		expected := 0x4034
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("10", func(t *testing.T) {
		input := "(label3 >> 13) & 0x7ffff"
		expected := 0x10051
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	t.Run("11", func(t *testing.T) {
		input := "(label3 >> 0) & 0x1fff"
		expected := 0x400
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual,canEval := arxsmEval(input, symbTable)
	
		if actual != int32(expected) && canEval == true {
			t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
		} else if canEval == false {
			t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
}



// func TestEvalWithTable(t *testing.T) {
// 	input := "(1+2*3)+size"
// 	expected := 20

// 	actual,canEval := arxsmEval(input, symbTable)

// 	if actual != int32(expected) && canEval == true {
// 		t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
// 	} else if canEval == false {
// 		t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
// 	} else {
// 		t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
// 	}
// }