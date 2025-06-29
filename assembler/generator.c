#include <stdio.h>
#include <stdlib.h>

#include "generator.h"
#include "assemblerError.h"


static void generateConst(DataTable* dataTable, AEFbin* bin) {
	uint8_t* constSect = malloc(sizeof(uint8_t) * bin->header.constSize);
	//

	uint32_t pos = 0x0;
	for (int i = 0; i < dataTable->cSize; i++) {
		data_entry_t* entry = dataTable->constEntries[i];

		for (int b = 0; b < entry->size; b++) {
			constSect[pos] = entry->data.bytes[b];
			pos++;
		}
	}

	bin->constSect = constSect;
}

static void generateData(DataTable* dataTable, AEFbin* bin) {
	uint8_t* dataSect = malloc(sizeof(uint8_t) * bin->header.dataSize);
	//

	uint32_t pos = 0x0;
	for (int i = 0; i < dataTable->dSize; i++) {
		data_entry_t* entry = dataTable->dataEntries[i];

		for (int b = 0; b < entry->size; b++) {
			dataSect[pos] = entry->data.bytes[b];
			pos++;
		}
	}

	bin->dataSect = dataSect;
}

static void generateText(InstructionStream* instrStream, AEFbin* bin) {
	uint32_t* textSect = malloc(sizeof(uint32_t) * bin->header.textSize);
	//

	uint32_t pos = 0x0;
	for (int i = 0; i < instrStream->size; i++) {
		instr_obj_t* instr = instrStream->instructions[i];

		textSect[pos] = instr->encoding;
		pos++;
	}

	bin->textSect = textSect;
}

AEFbin* generateBinary(InstructionStream* instrStream, SymbolTable* symbTable, DataTable* dataTable, SectionTable* sectTable) {
	AEFheader header = {
		.ID = {0xAE, 'A', 'E', 'F'},
		.entry = getSymbEntry(symbTable, "_init")->value,
		.bssSize = sectTable->entries[2].size,
		.constOff = 32,
		.constSize = sectTable->entries[1].size,
		.dataOff = 32 + sectTable->entries[1].size,
		.dataSize = sectTable->entries[0].size,
		.textOff = 32 + sectTable->entries[1].size + sectTable->entries[0].size,
		.textSize = sectTable->entries[3].size
	};


	AEFbin* bin = malloc(sizeof(AEFbin));
	//

	bin->header = header;


	generateConst(dataTable, bin);
	generateData(dataTable, bin);
	generateText(instrStream, bin);

	return bin;
}

void writeBinary(AEFbin* bin, char* outbin) {
	FILE* outfile = fopen(outbin, "wb");
	if (!outfile) handleError(ERR_IO, FATAL, "Could not open %s!\n", outbin);

	fwrite(&bin->header.ID, sizeof(uint8_t), 4, outfile);
	fwrite(&bin->header.entry, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.constOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.constSize, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.dataOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.dataSize, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.textOff, sizeof(uint32_t), 1, outfile);
	fwrite(&bin->header.textSize, sizeof(uint32_t), 1, outfile);
	fwrite(bin->constSect, sizeof(uint8_t), bin->header.constSize, outfile);
	fwrite(bin->dataSect, sizeof(uint8_t), bin->header.dataSize, outfile);
	fwrite(bin->textSect, sizeof(uint8_t), bin->header.textSize, outfile);

	printf("Wrote to %s!\n", outbin);

	fclose(outfile);
	free(bin->constSect);
	free(bin->dataSect);
	free(bin->textSect);
	free(bin);
}