/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "sudoku.h"
#include "sudoku_main.h"

/* Instruction that can't be reached */
#define SUDOKU_UNREACHABLE() abort()

/*
 * Handler when user presses 'ctrl+c'. Cleanly exit the program
 */
static void Sudoku_interrupt_handler(int signal_code)
{
	switch (signal_code) {
	case SIGINT:
		fprintf(stderr, "KeyboardInterrupt\n");
		/*
		 * We don't use any heap memory at all. It's just fine to exit directly
		 */
		exit(EXIT_SUCCESS);

	default:
		break;
	}
}

static bool Sudoku_solve_proc(Sudoku *sudoku, Sudoku_solve_args *args)
{
	Sudoku_solve_t result = Sudoku_solve(sudoku, args);
	if (result.solvable == false) {
		printf("The sudoku is not solvable!\n");
		return false;
	}
	printf("Finished. Used step: %llu\n", result.step);
	Sudoku_print(sudoku);
	return true;
}

static bool Sudoku_load_proc(Sudoku *sudoku, Sudoku_load_t result, char *load_path)
{
	switch (result.code) {
	case Sudoku_load_success:
		printf("Successfully load sudoku from '%s'\n"
			"Given number: %u\nOriginal:\n", load_path, result.read);
		Sudoku_print(sudoku);
		putchar('\n');
		/* Set up signal handler only when successfully loaded sudoku */
		signal(SIGINT, Sudoku_interrupt_handler);
		break;
	case Sudoku_fopen_failure:
		perror(load_path);
		break;
	case Sudoku_early_eof:
		fprintf(stderr, "Early EOF: expecting %u numbers, "
			"load %u instead\n", SUDOKU_CELL, result.count);
		break;
	case Sudoku_invalid_value:
		fprintf(stderr, "Invalid number: row: %u, column: %u, value: %u\n",
			INC(result.i), INC(result.j), result.val);
		break;
	case Sudoku_illegal:
		fprintf(stderr, "Illegal sudoku: row: %u, column: %u. "
			"Multiple occurrences of %u\n",
			INC(result.i), INC(result.j), result.val);
		break;
	default:
		SUDOKU_UNREACHABLE();
	}
	if (result.code != Sudoku_load_success) {
		fprintf(stderr, "\nLoad sudoku from '%s' failed\n", load_path);
		return false;
	}
	return true;
}

static void Sudoku_usage(void)
{
	static const char *Sudoku_usagewstr =
		SUDOKU_DOC("Usage: sudoku_vid [-h | --help] [<load_path> [--norev] [--all]]");
	fprintf(stderr, "%s\n", Sudoku_usagewstr);
}

typedef struct {
	char *option;
	char *explain;
}Sudoku_wdoc_t;

static const Sudoku_wdoc_t Sudoku_helpmsg[] = {
    {SUDOKU_DOC("--norev"), SUDOKU_DOC("Start from 1 to 9 instead of 9 to 1")},
    {SUDOKU_DOC("--all  "), SUDOKU_DOC("Find all possible solution")}
};

static void Sudoku_print_helpmsg(void)
{
	fprintf(stderr, "\nOptions:\n");
	size_t len = ARRAY_SIZE(Sudoku_helpmsg);
	size_t i;
	for (i = 0; i < len; i++) {
		fprintf(stderr, "  %s\t\t\t%s\t\n",
			Sudoku_helpmsg[i].option, Sudoku_helpmsg[i].explain);
	}
}

typedef struct {
	Sudoku *sudoku;
	int argc;
	char **argv;
}Sudoku_arg;

static int Sudoku_proc(Sudoku_arg *args)
{
	char *load_path = args->argv[1];
	Sudoku_load_t result = Sudoku_load(args->sudoku, load_path);
	bool succ = Sudoku_load_proc(args->sudoku, result, load_path);
	if (succ == false) {
		return 0;
	}
	Sudoku_solve_args slv_args = SUDOKU_SOLVEARGS_INITIALIZER;
	bool all = false;
	int i;
	for (i = 2; i < args->argc; i++) {
		char *op = args->argv[i];
		if (strcmp(op, "--norev") == 0) {
			slv_args.rev = false;
		}
		else if (strcmp(op, "--all") == 0) {
			all = true;
		}
		else {
			fprintf(stderr, "Unknown option: '%s'\n", op);
			Sudoku_usage();
			return 0;
		}
	}
	if (all) {
		Sudoku_dfs_t ret = Sudoku_dfs(args->sudoku);
		if (Sudoku_u128t_eq(&ret.ans, 0)) {
			printf("The sudoku is not solvable!\n");
			return 0;
		}
		else {
			SUDOKU_U128T_PRINT("Finished. Total solution: ", &ret.ans);
		}
		if (!Sudoku_u128t_eq(&ret.ans, 1)) {
			printf("Possible solution:\n");
		}
		Sudoku_print(args->sudoku);
	}
	else {
		Sudoku_solve_proc(args->sudoku, &slv_args);
	}
	return 0;
}

int Sudoku_main(int argc, char **argv)
{
	if (argc == 1) {
		fwprintf(stderr, L"sudoku_vid: Missing load path\n\n"
			L"Try 'sudoku_vid --help' for more information\n");
		return 0;
	}
	if (argc == 2 &&
		strcmp("--help", argv[1]) == 0 ||
		strcmp("-h", argv[1]) == 0)
	{
		Sudoku_usage();
		Sudoku_print_helpmsg();
		return 0;
	}
	Sudoku sudoku = SUDOKU_INITIALIZER;
	Sudoku_arg args = { &sudoku,argc,argv };
	return Sudoku_proc(&args);
}
