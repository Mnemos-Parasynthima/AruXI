package assemblerTests

import "testing"


func TestDirectiveData(t *testing.T)  {
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	// SectionTable initially starts as 0 (data section)
	// Change it manually to see the effect
	sectTable.activeSection = 10 // 10 is not a section

	// .data LAB: .byte 0x2
	t.Run("0", func(t *testing.T) {
		directive := "data"
		args := "LAB: .byte 0x2"
	
		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 0 {
			t.Errorf("%sExpected section: 0, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	sectTable.activeSection = 10

	// .data
	t.Run("1", func(t *testing.T) {
		directive := "data"
		args := ""

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 0 {
			t.Errorf("%sExpected section: 0, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveConst(t *testing.T) {
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	// .const LAB: .string "Const Sect"
	t.Run("0", func(t *testing.T) {
		directive := "const"
		args := "LAB: .string \"Const Sect\""
	
		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 1 {
			t.Errorf("%sExpected section: 1, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	sectTable.activeSection = 10 // Change to see the effect

	// .const
	t.Run("1", func(t *testing.T) {
		directive := "const"
		args := ""

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 1 {
			t.Errorf("%sExpected section: 1, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveBss(t *testing.T) {
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	// .bss LAB: .zero 4, 2
	t.Run("0", func(t *testing.T) {
		directive := "bss"
		args := "LAB: .zero 4, 2"
	
		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 2 {
			t.Errorf("%sExpected section: 2, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	sectTable.activeSection = 10 // Change to see the effect

	// .bss
	t.Run("1", func(t *testing.T) {
		directive := "bss"
		args := ""

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 2 {
			t.Errorf("%sExpected section: 2, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveText(t *testing.T) {
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	// .text LAB: ret"
	t.Run("0", func(t *testing.T) {
		directive := "text"
		args := "LAB: ret"
	
		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 3 {
			t.Errorf("%sExpected section: 3, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	sectTable.activeSection = 10 // Change to see the effect

	// .text
	t.Run("1", func(t *testing.T) {
		directive := "text"
		args := ""

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	
		if sectTable.activeSection != 3 {
			t.Errorf("%sExpected section: 3, Actual section: %d%s", RED, sectTable.activeSection, RESET)
		} else {
			t.Logf("%sActual section: %d%s", GREEN, sectTable.activeSection, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveSet(t *testing.T) {
	t.SkipNow()
	directive := "set"

	/**
	For .set, it tests on:
	- An entry is added or updated, fields are to be proper
	- If the expression could not be solved, stores the expression
	- If it could be solve, stores the value
	*/

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 3
	// @text .set SY,0x2+0x3*(0x1<<2)
	t.Run("0", func(t *testing.T) {
		args := "SY,0x2+0x3*(0x1<<2)"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "SY")
		expectedValue := 14
		expectedFlags := 0b00111001

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualValue := entry.anon0[0]
			if (actualValue != byte(expectedValue)) {
				t.Errorf("%sExpected value: %d, Actual value: %d%s", RED, expectedValue, actualValue, RESET)
			} else {
				t.Logf("%sActual value: %d%s", GREEN, actualValue, RESET)
			}

			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})

	// @text .set SYS, 	SY + (1 << 2)
	t.Run("1", func(t *testing.T) {
		args := "SYS, 	SY + (1 << 2)"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "SYS")
		expectedValue := 18
		expectedFlags := 0b00111001

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualValue := entry.anon0[0]
			if (actualValue != byte(expectedValue)) {
				t.Errorf("%sExpected value: %d, Actual value: %d%s", RED, expectedValue, actualValue, RESET)
			} else {
				t.Logf("%sActual value: %d%s", GREEN, actualValue, RESET)
			}

			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})

	sectTable.activeSection = 0
	// @data .set DA		, 	0xfff | (REF + 2)
	t.Run("2", func(t *testing.T) {
		args := "DA		, 	0xfff | (REF + 2)"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "DA")
		expectedExpr := " 0xfff | (REF + 2)"
		expectedFlags := 0b10001001

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualExpr := getString(entry.anon0)
			if actualExpr != expectedExpr {
				t.Errorf("%sExpected expr: %s, Actual expr: %s%s", RED, expectedExpr, actualExpr, RESET)
			} else {
				t.Logf("%sActual expr: %s%s", GREEN, actualExpr, RESET)
			}

			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})
	
	// Make reference to CH
	symb := arxsmInitSymbEntry("CH", "", 0, 0b00000010)
	arxsmAddSymbEntry(symbTable, symb)

	sectTable.activeSection = 2
	// @bss .set CH 	,0xff0
	t.Run("3", func(t *testing.T) {
		args := "CH 	,0xff0"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "CH")
		expectedValue := 0xff0
		expectedFlags := 0b00101011

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualValue := entry.anon0[0]
			if (actualValue != byte(expectedValue)) {
				t.Errorf("%sExpected value: %d, Actual value: %d%s", RED, expectedValue, actualValue, RESET)
			} else {
				t.Logf("%sActual value: %d%s", GREEN, actualValue, RESET)
			}

			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})

	// The following will cause `setDirective` to exit(-1) due to redefinition
	t.Run("REDEFINED_ERR", func(t *testing.T) {
		t.Skipf("%sRunning this would intentionally fail the tests.%s", YELLOW, RESET)
		// ...
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveGlob(t *testing.T) {
	directive := "glob"

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	// .glob _init
	t.Run("0", func(t *testing.T) {
		args := "_init"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "_init")
		expectedFlags := 0b00000110

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})

	// @text GLOB_FUNC:
	symb := arxsmInitSymbEntry("GLOB_FUNC", "", 0x400, 0b00110001)
	arxsmAddSymbEntry(symbTable, symb)

	// .glob GLOB_FUNC
	t.Run("1", func(t *testing.T) {
		args := "GLOB_FUNC"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetSymbEntry(symbTable, "GLOB_FUNC")
		expectedValue := 0x400
		expectedFlags := 0b00110111

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualValue := entry.anon0[0]
			if (actualValue != byte(expectedValue)) {
				t.Errorf("%sExpected value: %d, Actual value: %d%s", RED, expectedValue, actualValue, RESET)
			} else {
				t.Logf("%sActual value: %d%s", GREEN, actualValue, RESET)
			}

			actualFlags := entry.flags
			if (actualFlags != _Ctype_uint32_t(expectedFlags)) {
				t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, actualFlags, RESET)
			} else {
				t.Logf("%sActual flags: 0b%b%s", GREEN, actualFlags, RESET)
			}
		}
	})

	// .glob lab0, lab1
	t.Run("MULTIPLE_ARGS_ERR", func(t *testing.T) {
		t.Skipf("%sRunning this would intentionally fail the tests.%s", YELLOW, RESET)
		args := "lab0, lab1"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		// Expected to fail
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveEnd(t *testing.T) {
	t.Skipf("%sDIRECTIVE NOT IMPLEMENTED%s", YELLOW, RESET)
}

func TestDirectiveString(t *testing.T) {
	directive := "string"
	expectedType := 0

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 1
	// @const .string "This is a string"
	t.Run("0", func(t *testing.T) {
		args := "This is a string"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x0)
		expectedSize := 17
		expectedString := "This is a string"

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}

			actualString := getString(entry.data)
			if (actualString != expectedString) {
				t.Errorf("%sExpected string: %s, Actual string: %s%s", RED, expectedString, actualString, RESET)
			} else {
				t.Logf("%sActual string: %s%s", GREEN, actualString, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x11 // 17
		actualLP := sectTable.entries[1].lp
		
		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	sectTable.activeSection = 0
	// @data .string "This string can be changed!\n"
	t.Run("1", func(t *testing.T) {
		args := "This string can be changed!\n"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 0, 0x0)
		expectedSize := 29
		expectedString := "This string can be changed!\n"

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}

			actualString := getString(entry.data)
			if (actualString != expectedString) {
				t.Errorf("%sExpected string: %s, Actual string: %s%s", RED, expectedString, actualString, RESET)
			} else {
				t.Logf("%sActual string: %s%s", GREEN, actualString, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x1d // 29
		actualLP := sectTable.entries[0].lp
		
		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @data .string "Yet another modifiable string here\n"
	t.Run("2", func(t *testing.T) {
		args := "Yet another modifiable string here\n"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 0, 0x1d)
		expectedSize := 36
		expectedString := "Yet another modifiable string here\n"

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}

			actualString := getString(entry.data)
			if (actualString != expectedString) {
				t.Errorf("%sExpected string: %s, Actual string: %s%s", RED, expectedString, actualString, RESET)
			} else {
				t.Logf("%sActual string: %s%s", GREEN, actualString, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x41 // 29 + 36
		actualLP := sectTable.entries[0].lp
		
		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @text .string "Hello there", "This is invalid"
	t.Run("3", func(t *testing.T) {
		t.SkipNow()
		args := "\"Hello there This is invalid\""

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)
	})

	// @bss .string "This cannot be here"
	t.Run("INVALID_SECTION_ERR", func(t *testing.T) {
		t.Skipf("%sRunning this would intentionally fail the tests%s", YELLOW, RESET)
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveByte(t *testing.T) {
	directive := "byte"
	expectedType := 1

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 1
	// @const .byte 0x2, (0x2 << 4) | 0x1
	t.Run("0", func(t *testing.T) {
		args := "0x2, (0x2 << 4) | 0x1"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x0)
		expectedSize := 2

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x2
		actualLP := sectTable.entries[1].lp
		
		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @const .byte 0xff, 	0x00, 	0x10	, 0xaa
	t.Run("1", func(t *testing.T) {
		args := "0xff, 	0x00, 	0x10	, 0xaa"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x2)
		expectedSize := 4

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x6
		actualLP := sectTable.entries[1].lp
		
		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveHword(t *testing.T) {
	directive := "hword"
	expectedType := 2

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 1
	// @const .hword 0x20ff, (0x2 << 8) | 0x1
	t.Run("0", func(t *testing.T) {
		args := "0x20ff, (0x2 << 8) | 0x1"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x0)
		expectedSize := 4

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x4
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @const .hword 0xffff, 	0x1000, 	0x1000+0x1	, 0xaaee
	t.Run("1", func(t *testing.T) {
		args := "0xffff, 	0x1000, 	0x1000+0x1	, 0xaaee"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x4)
		expectedSize := 8

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0xc
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveWord(t *testing.T) {
	directive := "word"
	expectedType := 3

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 1
	// @const .word 0xffffffff, (0x2 << 16) | 0x100000
	t.Run("0", func(t *testing.T) {
		args := "0xffffffff, (0x2 << 16) | 0x100000"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x0)
		expectedSize := 8

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x8
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @const .word 0xffff, 	0x1000, 	0x1000+0x1	, 0xaaee
	t.Run("1", func(t *testing.T) {
		args := "0xffff, 	0x1000, 	0x1000+0x1	, 0xaaee"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x8)
		expectedSize := 16

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x18
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveFloat(t *testing.T) {
	directive := "float"
	expectedType := 4

	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()
	dataTable := arxsmInitDataTable()

	sectTable.activeSection = 1
	// @const .float 0.32, 1.23
	t.Run("0", func(t *testing.T) {
		args := "0.32, 1.23"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x0)
		expectedSize := 8
		expectedData := [2]float32{0.32, 1.23}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}

			actualData := ([2]float32)(getFloatArr(entry.data, 2))
			if (actualData != expectedData) {
				t.Errorf("%sExpected data: %f, Actual data: %f%s", RED, expectedData, actualData, RESET)
			} else {
				t.Logf("%sActual data: %f%s", GREEN, actualData, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x8
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	// @const .float 13.23, 3.141569, 0.000001
	t.Run("1", func(t *testing.T) {
		args := "13.23, 3.141569, 0.000001"

		arxsmHandleDirective(symbTable, sectTable, dataTable, directive, args)

		entry := arxsmGetDataEntry(dataTable, 1, 0x8)
		expectedSize := 12
		expectedData := [3]float32{13.23, 3.141569, 0.000001}

		if (entry == nil) {
			t.Errorf("%sExpected: entry, Actual: nil%s", RED, RESET)
		} else {
			actualType := entry._type
			if (actualType != _Ctype_uint8_t(expectedType)) {
				t.Errorf("%sExpected type: %d, Actual type: %d%s", RED, expectedType, actualType, RESET)
			} else {
				t.Logf("%sActual type: %d%s", GREEN, actualType, RESET)
			}

			actualSize := entry.size
			if (actualSize != _Ctype_uint32_t(expectedSize)) {
				t.Errorf("%sExpected size: %d, Actual size: %d%s", RED, expectedSize, actualSize, RESET)
			} else {
				t.Logf("%sActual size: %d%s", GREEN, actualSize, RESET)
			}

			actualData := ([3]float32)(getFloatArr(entry.data, 3))
			if (actualData != expectedData) {
				t.Errorf("%sExpected data: %f, Actual data: %f%s", RED, expectedData, actualData, RESET)
			} else {
				t.Logf("%sActual data: %f%s", GREEN, actualData, RESET)
			}
		}

		// Make sure LP changed
		expectedLP := 0x14
		actualLP := sectTable.entries[1].lp

		if (actualLP != _Ctype_uint32_t(expectedLP)) {
			t.Errorf("%sExpected LP: 0x%x, Actual LP: 0x%x%s", RED, expectedLP, actualLP, RESET)
		} else {
			t.Logf("%sActual LP: 0x%x%s", GREEN, actualLP, RESET)
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
	arxsmDeleteDataTable(dataTable)
}

func TestDirectiveZero(t *testing.T) {
	t.Skipf("%sDIRECTIVE NOT IMPLEMENTED%s", YELLOW, RESET)
}

func TestDirectiveFill(t *testing.T) {
	t.Skipf("%sDIRECTIVE NOT IMPLEMENTED%s", YELLOW, RESET)
}

func TestDirectiveAlign(t *testing.T) {
	t.Skipf("%sDIRECTIVE NOT IMPLEMENTED%s", YELLOW, RESET)
}