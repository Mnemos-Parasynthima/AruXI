package assemblerTests

import "testing"
import "fmt"


var symbTable *_Ctype_SymbolTable


func TestMain(m *testing.M) {
	symbTable = arxsmInitSymbTable()

	symbEntry := arxsmInitSymbEntry("size", "2+3+(0x2*4)", 13, 0b00001001)

	arxsmAddSymbEntry(symbTable, symbEntry)

	fmt.Printf("Current state of the symbol entry at Tests Entry:\n")	
	arxsmDisplaySymbTable(symbTable)

	m.Run()

	arxsmDeleteSymbTable(symbTable)
}


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
}

func TestEvalWithTable(t *testing.T) {
	input := "(1+2*3)+size"
	expected := 20

	actual,canEval := arxsmEval(input, symbTable)

	if actual != int32(expected) && canEval == true {
		t.Errorf("%sWhen canEval::Expected: `%d`, Actual: `%d`%s", RED, expected, actual, RESET)
	} else if canEval == false {
		t.Errorf("%sExpected: canEval, Actual: not canEval%s", RED, RESET)
	} else {
		t.Logf("%sActual: `%d`%s", GREEN, actual, RESET)
	}
}