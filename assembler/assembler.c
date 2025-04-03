#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>


int main(int argc, char const* argv[]) {
	char* infile = NULL;
	char* outbin = "out.ar";
	int opt;

	while ((opt = getopt(argc, (char* const*) argv, "o:")) != -1) {
		switch (opt) {
			case 'o':
				outbin = optarg;
				break;
			case '?':
				if (optopt == 'o') fprintf(stderr, "Optionc -%c requires an argument.\n", optopt);
				else if (isprint(optopt)) fprintf(stderr, "Unknown option '-%c'.\n", optopt);
		}
	}

	const char* allowedExts[] = { ".s" };

	for (int i = optind; i < argc; i++) {
		// Checking input file ends in .s

		const char* dot = strrchr(argv[i], '.');
		if (!dot || dot == argv[i] || *(dot-1) == '/') continue;

		for (int j = 0; j < 1; j++) {
			if (strcmp(dot, allowedExts[j]) != 0) continue;

			infile = (char*) argv[i];
			break;
		}
	}

	if (!infile) {
		fprintf(stderr, "Input file is not a valid assembly file.\n");
		exit(-1);
	}


	return 0;
}