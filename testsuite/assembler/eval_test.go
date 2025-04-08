package assemblerTests

import "testing"


var symbTable *_Ctype_SymbolTable


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

// Tests that on encountering an undefined symbol, it makes sure it results as so
// and that the referenced empty entries are also created.
func TestEvalSymbUndefined(t *testing.T) {
	symbTable = arxsmInitSymbTable()

	expectedFlags := 0b00000010

	t.Run("0", func(t *testing.T) {
		input := "symb1"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)

		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}

		// Makes sure the entry was added
		entry := arxsmGetSymbEntry(symbTable, input)
		if entry == nil {
			t.Errorf("%sExpected: entry for %s, Actual: nil%s", RED, input, RESET)
		} else {
			// Now that the entry is present, make sure it's flags are correct
			flags := entry.flags

			if int(flags) != expectedFlags {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, flags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, flags, RESET)
			}
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "label1 + 0x4 * (0x2 + 4)"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}

		// Makes sure the entry was added
		entry := arxsmGetSymbEntry(symbTable, "label1")
		if entry == nil {
			t.Errorf("%sExpected: entry for %s, Actual: nil%s", RED, "label1", RESET)
		} else {
			// Now that the entry is present, make sure it's flags are correct
			flags := entry.flags

			if int(flags) != expectedFlags {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, flags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, flags, RESET)
			}
		}
	})

	t.Run("2", func(t *testing.T) {
		// Although there are entries for symb1 and label1, they are not defined, so treat it as a fail
		input := "symb1 + label1 * (2 << 1)"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "symb1 + symb2 * label1 + (2 | 4)"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)

		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}

		// Makes sure the entry was added
		entry := arxsmGetSymbEntry(symbTable, "symb2")
		if entry == nil {
			t.Errorf("%sExpected: entry for %s, Actual: nil%s", RED, "symb2", RESET)
		} else {
			// Now that the entry is present, make sure it's flags are correct
			flags := entry.flags

			if int(flags) != expectedFlags {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, flags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, flags, RESET)
			}
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "symb1 * (label1 << symb2)"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)

		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	arxsmDisplaySymbTable(symbTable)
	arxsmDeleteSymbTable(symbTable)
}

func TestEvalSymbExpr(t *testing.T) {
	symbTable = arxsmInitSymbTable()

	// .set symb1 "symb2+(2<<4)" (34) @ data, local, no reference
	symb1 := arxsmInitSymbEntry("symb1", "symb2 + (2 << 4)", 0, 0b10001001)
	arxsmAddSymbEntry(symbTable, symb1)

	// .set symb2 0x2 @ data, local, reference
	symb2 := arxsmInitSymbEntry("symb2", "", 0x2, 0b00001011)
	arxsmAddSymbEntry(symbTable, symb2)

	// .set symb3 0xa0 @ bss, local, no reference
	symb3 := arxsmInitSymbEntry("symb3", "", 0xa0, 0b00101001)
	arxsmAddSymbEntry(symbTable, symb3)

	// addr label2 0x40000 @ test, local, no reference
	label2 := arxsmInitSymbEntry("label2", "", 0x40000, 0b00110001)
	arxsmAddSymbEntry(symbTable, label2)

	// .set symb4 "label2<<4)+0x4" (4) @ const, local, no reference
	symb4 := arxsmInitSymbEntry("symb4", "(label2<<4)+0x4", 0, 0b10011001)	
	arxsmAddSymbEntry(symbTable, symb4)

	arxsmDisplaySymbTable(symbTable)

	t.Run("0", func(t *testing.T) {
		input := "symb1 | (1 << 2)"
		expected := 38

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
		input := "label1 + symb3"

		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)

		_,canEval := arxsmEval(input, symbTable)

		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}

		// Makes sure the entry was added
		entry := arxsmGetSymbEntry(symbTable, "label1")
		if entry == nil {
			t.Errorf("%sExpected: entry for %s, Actual: nil%s", RED, "label1", RESET)
		} else {
			// Now that the entry is present, make sure it's flags are correct
			expectedFlags := 0b00000010
			flags := entry.flags

			if int(flags) != expectedFlags {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, flags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, flags, RESET)
			}
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "symb4 & 0b111"
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

	t.Run("checkEntries", func(t *testing.T) {
		symb1ref := (symb1.flags>>1) & 0b1
		symb3ref := (symb3.flags>>1) & 0b1
		label2ref := (label2.flags>>1) & 0b1
		symb4ref := (symb4.flags>>1) & 0b1
		label1ref := (arxsmGetSymbEntry(symbTable, "label1").flags>>1) & 0b1

		if symb1ref != 0b1 && symb3ref != 0b1 && label2ref != 0b1 && symb4ref != 0b1 && label1ref != 0b1 {
			t.Errorf("%sExpected: All entries to have Reference Bit, Actual: Not all have Reference Bit%s", RED, RESET)
		} else {
			t.Logf("%sActual: All entries have Reference Bit%s", GREEN, RESET)
		}
	})

	arxsmDisplaySymbTable(symbTable)
	arxsmDeleteSymbTable(symbTable)
}

func TestEvalMalformed(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := "((3 + 2 )"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "3 / "
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "3 ++ 2"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "()"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "( 3 | 4 >> 2 ) * []"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("5", func(t *testing.T) {
		input := "((4 / 2 )]"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})

	t.Run("6", func(t *testing.T) {
		input := "2 // 4 + -2"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		_,canEval := arxsmEval(input, symbTable)
	
		if canEval == true {
			t.Errorf("%sExpected: not canEval, Actual: canEval%s", RED, RESET)
		} else {
			t.Logf("%sActual: not canEval%s", GREEN, RESET)
		}
	})
}