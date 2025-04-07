package assemblerTests

import "testing"


func TestPreprocessor(t *testing.T) {
	result := 4

	if result != 2 {
		t.Errorf("Expected 2, got %d", result)
	}
}