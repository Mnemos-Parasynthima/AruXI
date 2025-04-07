package assemblerTests

import "testing"


func TestPreprocessor(t *testing.T) {
	input := " 	\tARU:	 \t\n"
	expected := "ARU:"

	t.Logf("%sInput string is `%s`%s", YELLOW, input, RESET)

	actual := arxsmPreprocess(input, int64(len(input)))

	if actual != expected {
		t.Errorf("%sExpected: `%s`, Actual: `%s`%s", RED, expected, actual, RESET)
	} else {
		t.Logf("%sActual: `%s`%s", GREEN, actual, RESET)
	}
}