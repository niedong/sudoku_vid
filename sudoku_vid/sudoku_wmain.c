#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"
#include "sudoku_wmain.h"

typedef struct {
	Sudoku *sudoku;
	int argc;
	wchar_t **wargv;
}Sudoku_arg;

static const wchar_t Sudoku_usagestr[] =
SUDOKU_DOC(L"Usage: sudoku_vid <load path> [store path] [-p | --print]");

#define MIN_ARGCNT 2
#define MAX_ARGCNT 4

#define REVOP true

static void Sudoku_usage(void)
{
	fwprintf(stderr, L"%ls\n", Sudoku_usagestr);
}

static void Sudoku_utime(double utime)
{
	wprintf(L"Finished. Used time: %e\n", utime);
}

static int Sudoku_proc(Sudoku_arg *args)
{
	if (Sudoku_wload(args->sudoku, args->wargv[1])) {
		wprintf(L"Successfully load sudoku from '%ls'\n", args->wargv[1]);
	}
	else {
		return 0;
	}
	switch (args->argc) {
	case 2:
		Sudoku_utime(Sudoku_solve(args->sudoku, false, REVOP));
		Sudoku_print(args->sudoku);
		break;
	case 3:
		if (wcscmp(L"-p", args->wargv[2]) == 0 ||
			wcscmp(L"--print", args->wargv[2]) == 0)
		{
			double utime = Sudoku_solve(args->sudoku, true, REVOP);
			Sudoku_utime(utime);
		}
		else {
			Sudoku_solve(args->sudoku, false, REVOP);
			Sudoku_wstore(args->sudoku, args->wargv[2]);
		}
		break;
	case 4:
		if (wcscmp(L"-p", args->wargv[3]) != 0 &&
			wcscmp(L"--print", args->wargv[3]) != 0)
		{
			fwprintf(stderr, L"Unsupported option: %ls\n", args->wargv[3]);
			Sudoku_usage();
		}
		else {
			Sudoku_utime(Sudoku_solve(args->sudoku, true, REVOP));
			Sudoku_wstore(args->sudoku, args->wargv[2]);
		}
		break;
	default:
		abort();
	}
	return 0;
}

int Sudoku_wmain(int argc, wchar_t **wargv)
{
	if (argc < MIN_ARGCNT || argc > MAX_ARGCNT) {
		Sudoku_usage();
		return 0;
	}
	Sudoku sudoku = SUDOKU_INITIALIZER;
	Sudoku_arg sudoku_arg = { &sudoku,argc,wargv };
	return Sudoku_proc(&sudoku_arg);
}
