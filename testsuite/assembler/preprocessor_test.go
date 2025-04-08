package assemblerTests

import "testing"


func TestPreprocessor(t *testing.T) {
	t.Run("0", func(t *testing.T) {
		input := " 	\tARU:	 \t\n"
		expected := "ARU:"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("1", func(t *testing.T) {
		input := "ARU:\n"
		expected := "ARU:"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("2", func(t *testing.T) {
		input := "\t LA: add x0, x0, x1 % this is a comment\t\t\n"
		expected := "LA: add x0, x0, x1"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("3", func(t *testing.T) {
		input := "\n"
		expected := ""
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("4", func(t *testing.T) {
		input := "\t\t\t%\n"
		expected := ""
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("5", func(t *testing.T) {
		input := "\t\t.set la, 0x2  % la symbol\n"
		expected := ".set la, 0x2"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("6", func(t *testing.T) {
		input := "% comment line only\n"
		expected := ""
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("7", func(t *testing.T) {
		input := "\t\t% some comment\n"
		expected := ""
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("8", func(t *testing.T) {
		input := "\t\tret"
		expected := "ret"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("9", func(t *testing.T) {
		input := "\t\t  \t \n"
		expected := ""
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("10", func(t *testing.T) {
		input := "  DAT: .byte 0x1, 0x2, 0x1, 0x4    \t\n"
		expected := "DAT: .byte 0x1, 0x2, 0x1, 0x4"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("11", func(t *testing.T) {
		input := "  .text LAB:    \t\n"
		expected := ".text LAB:"
		// Note that ".text LAB:" is not a valid line but that is for later
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})

	t.Run("12", func(t *testing.T) {
		input := "\t\t mul x0, x1, x2%comment\n"
		expected := "mul x0, x1, x2"
	
		t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)
	
		actual := arxsmPreprocess(input, int64(len(input)))
	
		if actual != expected {
			t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
		} else {
			t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
		}
	})
}