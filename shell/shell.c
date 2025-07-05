#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include "emSignal.h"
#include "shmem.h"
#include "diagnostics.h"
#include "signalHandler.h"
#include "parser.h"
#include "environment.h"


const char* PROMPT = "ash> ";
signal_t* sigMem = NULL;
EnvVar* shellEnv; // Array of environmental variables


typedef enum {
	SH_ERR,
	SH_HELP,
	SH_EXIT,
	SH_RUN,
	SH_VIEW,
	SH_DEBUG,
	SH_ENV,
	SH_PATH
} action_t;



/**
 * Handle SIGINT (Ctrl+C) by simply ignoring it.
 * @param signum 
 */
void handleSIGINT(int signum) {
	tcflush(STDIN_FILENO, TCIFLUSH);
}

/**
 * Handle SIGUSR1. SIGUSR1 indicates as a poke to tell the process to check the signal memory.
 * @param signum 
 */
void handleSIGUSR1(int signum) {

}

void showSigState(int SIG, bool showMetadata, bool showPayload) {
	signal_t* signal = GET_SIGNAL(sigMem, SIG);

	char* type;
	switch (SIG)	{
		case UNIVERSAL_SIG: type = "universal"; break;
		case EMU_SHELL_SIG: type = "emu-shell"; break;
		case EMU_CPU_SIG: type = "emu-cpu"; break;
		case SHELL_CPU_SIG: type = "shell-cpu"; break;
		default:	break;
	}

	printf("Type::%s\n", type);
	printf("\t.Interrupts = 0x%x\n", signal->interrupts);
	printf("\t.IntEnable = 0x%x\n", signal->intEnable);
	printf("\t.AckMask = 0x%x\n", signal->ackMask);
	printf("\t.PayloadValid = 0x%x\n", signal->payloadValid);

	if (showMetadata) {
		// Metadata depends on the signal interrupt itself
		uint8_t execInt =  SIG_GET(signal->interrupts, emSIG_EXEC_IDX);
		if (execInt) {
			execprog_md metadata = signal->metadata.execprog;
			printf("\tEXEC Metadata:\n");
			printf("\t .Entry = 0x%x\n", metadata.entry);
		}
	}
}

action_t viewState(int argc, char** argv) {
	if (argc > 3) {
		dLog(D_NONE, DSEV_WARN, "Invalid argument count!");
		return SH_ERR;
	}

	bool showPayloadMetadata = false;
	bool showPayload = false;
	char* type = NULL;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0) showPayloadMetadata = true;
		else if (strcmp(argv[i], "-v") == 0) showPayload = true;
		else if (*(argv[i]) == '-' ) {
			dLog(D_NONE, DSEV_WARN, "Option not found!");
			return SH_ERR;
		} else type = argv[i];
	}

	if (showPayload && !showPayloadMetadata) {
		dLog(D_NONE, DSEV_WARN, "Cannot show payload if no metadata!");
		return SH_ERR;
	}

	if (type) {
		if (strcmp(type, "universal") == 0) showSigState(UNIVERSAL_SIG, showPayloadMetadata, showPayload);
		else if (strcmp(type, "emu-shell") == 0) showSigState(EMU_SHELL_SIG, showPayloadMetadata, showPayload);
		else if (strcmp(type, "emu-cpu") == 0) showSigState(EMU_CPU_SIG, showPayloadMetadata, showPayload);
		else if (strcmp(type, "shell-cpu") == 0) showSigState(SHELL_CPU_SIG, showPayloadMetadata, showPayload);
		else {
			dLog(D_NONE, DSEV_WARN, "No such type `%s`!", type);
			return SH_ERR;
		}
	} else {
		showSigState(UNIVERSAL_SIG, showPayloadMetadata, showPayload);
		showSigState(EMU_SHELL_SIG, showPayloadMetadata, showPayload);
		showSigState(EMU_CPU_SIG, showPayloadMetadata, showPayload);
		showSigState(SHELL_CPU_SIG, showPayloadMetadata, showPayload);
	}

	return SH_VIEW;
}

action_t handleEnv(int argc, char** argv) {
	if (argc > 5 || argc < 2) {
		dLog(D_NONE, DSEV_WARN, "Invalid argument count!");
		return SH_ERR;
	}

	char* subcmd = argv[0];

	if (strcmp(subcmd, "show") == 0) {
		if (argc != 2) {
			dLog(D_NONE, DSEV_WARN, "Unexpected argument `%s`", argv[2]);
			return SH_ERR;
		}

		char* arg = argv[1];
		if (strcmp(arg, "--all") == 0) {
			printEnv();
		} else if (*arg == '-') {
			dLog(D_NONE, DSEV_WARN, "No such option!");
			return SH_ERR;
		} else {
			EnvVar* envVar = getEnv(arg);

			if (!envVar) {
				dLog(D_NONE, DSEV_WARN, "No such `%s` environmental variable!", arg);
				return SH_ERR;
			}

			printf("%s: %s\n", envVar->key, envVar->value);
		}
	} else if (strcmp(subcmd, "set") == 0) {
		if (argc != 5) {
			dLog(D_NONE, DSEV_WARN, "Invalid argument count!");
			return SH_ERR;
		}

		int key = -1;
		int value = -1;
		char* keyStr = NULL;
		char* valueStr = NULL;
		
		for (int i = 1; i < argc; i++) {
			char* arg = argv[i];

			if (strcmp(arg, "-k") == 0) key = i;
			else if (strcmp(arg, "-v") == 0) value = i;
		}

		if (key == (value+1) || key == (value-1)) {
			dLog(D_NONE, DSEV_WARN, "Invalid option placing!");
			return SH_ERR;
		}

		if (key == -1) {
			dLog(D_NONE, DSEV_WARN, "No key option found!");
			return SH_ERR;
		} else keyStr = argv[key+1];

		if (value == -1) {
			dLog(D_NONE, DSEV_WARN, "No value option found!");
			return SH_ERR;
		} else valueStr = argv[value+1];

		setEnv(keyStr, valueStr);
	} else {
		dLog(D_NONE, DSEV_WARN, "Invalid subcommand!");
		return SH_ERR;
	}

	return SH_ENV;
}

action_t handlePath(int argc, char** argv) {
	if (argc > 2 || argc < 1) {
		dLog(D_NONE, DSEV_WARN, "Invalid argument count!");
		return SH_ERR;
	}

	char* subcmd = argv[0];

	EnvVar* PATH = NULL;
	if (strcmp(subcmd, "show") == 0) {
		if (argc > 1) {
			dLog(D_NONE, DSEV_WARN, "Unexpected argument `%s`", argv[1]);
			return SH_ERR;
		}

		PATH = getEnv("PATH");
		printf("%s: %s\n", PATH->key, PATH->value);
	} else if (strcmp(subcmd, "add") == 0) {
		if (argc > 2) {
			dLog(D_NONE, DSEV_WARN, "Unexpected argument `%s`", argv[2]);
			return SH_ERR;
		}

		appendPath(argv[1]);
	} else {
		dLog(D_NONE, DSEV_WARN, "Invalid subcommand!");
		return SH_ERR;
	}

	return SH_PATH;
}

void exitHelp() {
	printf("exit:\n");
	printf("\t`exit`: Exits the entire emulator.\n");
}

void viewHelp() {
	printf("view:\n");
	printf("\t`view [-p] [-v]`: View the contents of all the signals in the signal memory. \
If `-p`, it shows the payload metadata depending on the active signal interrupt.\n\
\t\tIf `-v` is included, it shows the actual payload, if any, through injected pointers.\n");
	printf("\t`view [type] [-p] [-v]`: Similar as just `view` but it displays all the signals. \
Allowed `type`s are: `universal`, `emu-shell`, `emu-cpu`, and `shell-cpu`.\n");
}

void envHelp(bool path) {
	if (path) {
		printf("path:\n");
		printf("\t`path show`: Shows the path variable.\n");
		printf("\t`path add [path]`: Appends the given path string to the existing one.\
The path must be separated by ':'.\n");
	} else {
		printf("env:\n");
		printf("\t`env show --all`: Shows all the current environmental variables.\n");
		printf("\t`env show [env_name]`: Shows the given environmental variable, if it exists.\n");
		printf("\t`env set -k [key_name] -v [value]`: Sets an environmental variabel with the provided name and value.\n\
\t\tIf it exists, its value gets overwritten. Note this also applies to the path. \
The options and its parameters can be in any order together but must remain continuous.\n\
\t\tThat is, `-k -v` is not allowed, similarly `[key_name] [value] -k -v`.\n");
	}
}

void helpHelp() {
	printf("help:\n");
	printf("\t`help [cmd]`: Show the help message. If `[cmd]`, \
it displays details regarding that command. Otherwise, it shows a brief overview of all commands.\n");
}

action_t shellHelp(int argc, char** argv) {
	if (argc > 1) {
		dLog(D_NONE, DSEV_WARN, "Found unexpected argument!");
		return SH_ERR;
	}

	char* cmd = argv[0];

	if (cmd) {
		printf("Command Help::");
		if (strcmp(cmd, "exit") == 0) exitHelp();
		else if (strcmp(cmd, "view") == 0) viewHelp();
		else if (strcmp(cmd, "env") == 0) envHelp(false);
		else if (strcmp(cmd, "path") == 0) envHelp(true);
		else if (strcmp(cmd, "help") == 0) helpHelp();
		// else if (strcmp(cmd, "debug") == 0) debugHelp();
		else {
			dLog(D_NONE, DSEV_WARN, "No such command!");
			return SH_ERR;
		}
	} else {
		printf("Help Overview:\n");
		printf("exit\n");
		printf("view [-p]? [-v]?\n");
		printf("view [type] [-p]? [-v]?\n");
		printf("env show --all\n");
		printf("env show [env_name]\n");
		printf("env set -k [key_name] -v [value]\n");
		printf("path show\n");
		printf("path add [path]\n");
		printf("help [cmd]\n");
		printf("[program]\n");
		// printf("debug\n");
	}

	return SH_HELP;
}

action_t runProgram(int argc, char** argv) {
	if (argc < 1) {
		dLog(D_NONE, DSEV_WARN, "Program not found!");
		return SH_ERR;
	}

	char* program = argv[0];

	// Check that the program is valid in path
	EnvVar* path = getEnv("PATH");

	char* pathsave = NULL;
	char* pathStr = pathtok(path->value, &pathsave);
	while (pathStr) {
		dDebug(DB_BASIC, "path: %s", pathStr);
		// Look for program in path


		pathStr = pathtok(NULL, &pathsave);
	}

	signal_t* shellEmuSignal = GET_SIGNAL(sigMem, EMU_SHELL_SIG);
	loadprog_md metadata = {
		.program = program,
		.argv = argv
	};
	int ret = setLoadSignal(shellEmuSignal, &metadata);
	if (ret == -1) dFatal(D_ERR_SIGNAL, "No access for load signal!");

	return SH_RUN;
}

action_t eval(Command* cmd) {
	if (strcmp(cmd->command, "exit") == 0) {
		if (cmd->argc != 0) {
			dLog(D_NONE, DSEV_WARN, "Found unexpected arguments!");
			return SH_ERR;
		}
		return SH_EXIT;
	} else if (strcasecmp(cmd->command, "view") == 0) return viewState(cmd->argc, cmd->argv);
	else if (strcasecmp(cmd->command, "env") == 0) return handleEnv(cmd->argc, cmd->argv);
	else if (strcasecmp(cmd->command, "path") == 0) return handlePath(cmd->argc, cmd->argv);
	else if (strcasecmp(cmd->command, "help") == 0) return shellHelp(cmd->argc, cmd->argv);
	else return runProgram(cmd->argc, cmd->argv);
}

int main(int argc, char const* argv[]) {
	tcsetpgrp(STDIN_FILENO, getpid());

	initDiagnostics(stdout, "shell.debug");

	dLog(D_NONE, DSEV_INFO, "[ASH]: Loading...");
	loadEnv();

	// shell only has access to signal memory
	int fd = shm_open(SHMEM_SIG, O_RDWR, 0666);
	if (fd == -1) dFatal(D_ERR_SHAREDMEM, "Could not open shared memory for signal!");

	void* _sigMem = mmap(NULL, SIG_SIZE*4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (_sigMem == MAP_FAILED) dFatal(D_ERR_MEM, "Could not mmap signal memory!");

	close(fd);

	redefineSignal(SIGINT, &handleSIGINT);
	redefineSignal(SIGUSR1, &handleSIGUSR1);

	sigMem = (signal_t*) _sigMem;

	// Block until signal
	signal_t* universalSig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	dDebug(DB_DETAIL, "Universal interrupts before getting ready: 0x%x", universalSig->interrupts);
	dLog(D_NONE, DSEV_INFO, "[ASH]: Will now wait for ready...");
	uint8_t ready = 0x0;
	while (ready != 0x1) ready = SIG_GET(universalSig->interrupts, emSIG_READY_IDX);
	dDebug(DB_DETAIL, "Universal interrupts after getting ready: 0x%x", universalSig->interrupts);

	// At this point, stuff has been set up
	// But the shell must run after the kernel has finished running and the cpu is idle
	// This will happen by the cpu setting the acknowledged on the shell-cpu:exec signal

	signal_t* shellCPUSig = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
	dLog(D_NONE, DSEV_INFO, "[ASH]: Will now wait for exec acknowledged...");
	uint8_t ackd = 0x0;
	while (ackd != 0x1) ackd = SIG_GET(shellCPUSig->ackMask, emSIG_EXEC_IDX);

	while (true) {
		printf("%s", PROMPT);

		char* line = NULL;
		size_t n;
		ssize_t read = getline(&line, &n, stdin);
		*(line + read-1) = '\0';

		Command cmd = parseCommand(line);

		action_t act = eval(&cmd);

		free(cmd.argv);
		free(line);

		// Wait for the program to finish before continuing
		// if (act == SH_RUN) {
		// 	dDebug(DB_BASIC, "Waiting for program to exit...");
		// 	signal_t* sig = GET_SIGNAL(sigMem, SHELL_CPU_SIG);
		// 	uint8_t exited = 0x0;
		// 	while (exited != 0x1) exited = SIG_GET(sig->interrupts, emSIG_EXIT_IDX);
		// 	dDebug(DB_BASIC, "Program has exit");
		// }

		if (act == SH_EXIT)	break;
	}

	dLog(D_NONE, DSEV_INFO, "Shutting down...");
	signal_t* sig = GET_SIGNAL(sigMem, UNIVERSAL_SIG);
	dDebug(DB_DETAIL, "Universal interrupts before setting shutdown: 0x%x", universalSig->interrupts);
	setShutdownSignal(sig);
	dDebug(DB_DETAIL, "Universal interrupts after setting shutdown: 0x%x", universalSig->interrupts);

	// Need to wait for CPU to have ack'd the signal (only after it is done cleaning up)
	ackd = 0x0;
	while (ackd != 0x1) ackd = SIG_GET(sig->ackMask, emSIG_SHUTDOWN_IDX);

	munmap(sigMem, SIG_SIZE*4);
	deleteEnv();

	dLog(D_NONE, DSEV_INFO, "Shell exiting");

	return 0;
}