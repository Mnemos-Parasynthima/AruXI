package assemblerTests

import "testing"


func TestEval(t *testing.T) {
	symbTable := arxsmInitSymbTable()

	result := arxsmEval("1 + 2 * 3", symbTable)
	if result != 9 {
		t.Errorf("Expected 9, got %d", result)
	} else {
		t.Log("Success")
	}
}