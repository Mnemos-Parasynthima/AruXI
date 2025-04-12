package assemblerTests

import "testing"


func TestHandleLabel(t *testing.T) {
	symbTable := arxsmInitSymbTable()
	sectTable := arxsmInitSectionTable()

	// Simulate a file
	// .glob _init
	_init := arxsmInitSymbEntry("_init", "", 0, 0b00000110)
	arxsmAddSymbEntry(symbTable, _init)

	// .data

	// @0x0 DAT: .byte 0x2, 0xa, 0xe
	t.Run("0", func(t *testing.T) {
		tok := "DAT"
		args := ".byte 0x2, 0xa, 0xe"

		expectedTok := ".byte"
		expectedSave := "0x2, 0xa, 0xe"
		expectedAddr := 0x0

		actualTok,actualSave := arxsmHandleLabel(symbTable, sectTable, tok, args)

		if expectedTok != actualTok {
			t.Errorf("%sExpected tok: (%s), Actual tok: (%s)%s", RED, expectedTok, actualTok, RESET)
		} else if expectedSave != actualSave {
			t.Errorf("%sExpected save: (%s), Actual save: (%s)%s", RED, expectedSave, actualSave, RESET)
		} else {
			t.Logf("%sActual tok: (%s), Actual save: (%s)%s", GREEN, actualTok, actualSave, RESET)

			// Now check for the label entry
			// What matters is that its value is set and the flag of DEFINED set
			entry := arxsmGetSymbEntry(symbTable, tok)

			if entry.anon0[0] != byte(expectedAddr) {
				t.Errorf("%sExpected address of %s: %d, Actual address: %d%s", RED, tok, expectedAddr, entry.anon0[0], RESET)
			} else {
				if ((entry.flags>>0) & 0b1) != 0b1 {
					t.Errorf("%sExpected: DEFINED, Actual: NOT DEFINED%s", RED, RESET)
				}
			}
		}
	})

	sectTable.entries[0].lp += 3;
	// @0x3 .byte 0x0
	sectTable.entries[0].lp++;
	// @0x4 STRG:
	t.Run("1", func(t *testing.T) {
		tok := "STRG"
		args := ""

		expectedTok := ""
		expectedSave := ""
		expectedAddr := 0x4

		actualTok,actualSave := arxsmHandleLabel(symbTable, sectTable, tok, args)

		if expectedTok != actualTok {
			t.Errorf("%sExpected tok: (%s), Actual tok: (%s)%s", RED, expectedTok, actualTok, RESET)
		} else if expectedSave != actualSave {
			t.Errorf("%sExpected save: (%s), Actual save: (%s)%s", RED, expectedSave, actualSave, RESET)
		} else {
			t.Logf("%sActual tok: (%s), Actual save: (%s)%s", GREEN, actualTok, actualSave, RESET)

			// Now check for the label entry
			// What matters is that its value is set and the flag of DEFINED set
			entry := arxsmGetSymbEntry(symbTable, tok)

			if entry.anon0[0] != byte(expectedAddr) {
				t.Errorf("%sExpected address of %s: %d, Actual address: %d%s", RED, tok, expectedAddr, entry.anon0[0], RESET)
			} else {
				if ((entry.flags>>0) & 0b1) != 0b1 {
					t.Errorf("%sExpected: DEFINED, Actual: NOT DEFINED%s", RED, RESET)
				}
			}
		}
	})

	// @0x4 .string "1234321" % last byte is null
	sectTable.entries[0].lp += 12;

	// .text
	sectTable.activeSection = 3
	// _init:
	t.Run("2", func(t *testing.T) {
		tok :="_init"
		args := ""

		expectedTok := ""
		expectedSave := ""
		expectedAddr := 0x0

		actualTok,actualSave := arxsmHandleLabel(symbTable, sectTable, tok, args)
		// Since tok ("_init") was placed by global earlier
		// the only changes to be made is the value and the flags

		if expectedTok != actualTok {
			t.Errorf("%sExpected tok: (%s), Actual tok: (%s)%s", RED, expectedTok, actualTok, RESET)
		} else if expectedSave != actualSave {
			t.Errorf("%sExpected save: (%s), Actual save: (%s)%s", RED, expectedSave, actualSave, RESET)
		} else {
			t.Logf("%sActual tok: (%s), Actual save: (%s)%s", GREEN, actualTok, actualSave, RESET)

			// Now check for the label entry
			entry := arxsmGetSymbEntry(symbTable, tok)

			if entry.anon0[0] != byte(expectedAddr) {
				t.Errorf("%sExpected address of %s: 0x%x, Actual address: 0x%x%s", RED, tok, expectedAddr, entry.anon0[0], RESET)
			} else {
				// Check all the flags
				expectedFlags := 0b00110111

				if entry.flags != _Ctype_uint32_t(expectedFlags) {
					t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, entry.flags, RESET)
				}
			}
		}
	})

	// @0x0 mv x0, x1
	// @0x4 mv x2, #0x2e
	// @0x8 add x0, x2, x2
	// @0xc b LOOP
	loop := arxsmInitSymbEntry("LOOP", "", 0x0, 0b00000010)
	arxsmAddSymbEntry(symbTable, loop)

	// @0x10 ret
	// @0x14 LOOP: ld x4, [sp]
	sectTable.entries[3].lp = 0x14
	t.Run("3", func(t *testing.T) {
		tok :="LOOP"
		args := "ld x4, [sp]"

		expectedTok := "ld"
		expectedSave := "x4, [sp]"
		expectedAddr := 0x14

		actualTok,actualSave := arxsmHandleLabel(symbTable, sectTable, tok, args)
		// Since tok "LOOP" was referenced by instruction `b LOOP`
		// Check that the flags were updated properly

		if expectedTok != actualTok {
			t.Errorf("%sExpected tok: (%s), Actual tok: (%s)%s", RED, expectedTok, actualTok, RESET)
		} else if expectedSave != actualSave {
			t.Errorf("%sExpected save: (%s), Actual save: (%s)%s", RED, expectedSave, actualSave, RESET)
		} else {
			t.Logf("%sActual tok: (%s), Actual save: (%s)%s", GREEN, actualTok, actualSave, RESET)

			// Now check for the label entry
			entry := arxsmGetSymbEntry(symbTable, tok)

			if entry.anon0[0] != byte(expectedAddr) {
				t.Errorf("%sExpected address of %s: %d, Actual address: %d%s", RED, tok, expectedAddr, entry.anon0[0], RESET)
			} else {
				// Check all the flags
				expectedFlags := 0b00110011

				if entry.flags != _Ctype_uint32_t(expectedFlags) {
					t.Errorf("%sExpected flags: 0b%b, Actual flags: 0b%b%s", RED, expectedFlags, entry.flags, RESET)
				}
			}
		}
	})

	arxsmDeleteSymbTable(symbTable)
	arxsmDeleteSectionTable(sectTable)
}