#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <Windows.h>
#include "sudoku.h"
#include "sudoku_wmain.h"

typedef struct {
	Sudoku *sudoku;
	int argc;
	wchar_t **wargv;
}Sudoku_arg;

#define MIN_ARGCNT 2
#define MAX_ARGCNT 3

#define REVOP true
#define SUDOKU_UNREACHABLE() abort()

static CONSOLE_SCREEN_BUFFER_INFO Sudoku_buffinfo;

/*
 * Handler when user presses 'ctrl+c'. Cleanly exit the program
 */
static void Sudoku_interupt_handler(int signal_code)
{
	HANDLE handle;
	switch (signal_code) {
	case SIGINT:
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cursor_info;
		GetConsoleCursorInfo(handle, &cursor_info);
		cursor_info.bVisible = TRUE;
		SetConsoleCursorInfo(handle, &cursor_info);

		/*
		 * Stop from printing sudoku board
		 */
		CloseHandle(handle);

		handle = GetStdHandle(STD_ERROR_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO buff_info;
		GetConsoleScreenBufferInfo(handle, &buff_info);
		buff_info.dwCursorPosition.X = 0;
		buff_info.dwCursorPosition.Y = Sudoku_buffinfo.dwCursorPosition.Y + SUDOKU_SIZE;
		SetConsoleCursorPosition(handle, buff_info.dwCursorPosition);
		fwprintf(stderr, L"KeyboardInterrupt\n");

		/*
		 * We don't use any heap memory at all. It's just fine to exit directly
		 */
		exit(EXIT_SUCCESS);

	default:
		break;
	}
}

static void Sudoku_usage(void)
{
	static const wchar_t *Sudoku_usagewstr =
		SUDOKU_DOC(L"Usage: sudoku_vid  [-h | --help] [<load path>[--print]]");
	fwprintf(stderr, L"%ls\n", Sudoku_usagewstr);
}

static void Sudoku_helper(void)
{
	static const wchar_t *Sudoku_helpwstr =
		SUDOKU_DOC(L"Copyright (c) 2020 niedong\n"
			L"License under MIT License. For more information, visit "
			L"'https://github.com/niedong/sudoku_vid/blob/master'\n\n"
			L"'sudoku_vid' is a command-line program that aims to visualize "
			L"the process of solving sudoku\n");
	fwprintf(stderr, L"%ls\n", Sudoku_helpwstr);
}

static int Sudoku_proc(Sudoku_arg *args)
{
	if (args->argc == MIN_ARGCNT &&
		(wcscmp(args->wargv[MIN_ARGCNT - 1], L"--help") == 0) ||
		(wcscmp(args->wargv[MIN_ARGCNT - 1], L"-h") == 0))
	{
		Sudoku_helper();
		Sudoku_usage();
		return 0;
	}
	wchar_t *load_path = args->wargv[1];
	Sudoku_t read = Sudoku_wload(args->sudoku, load_path);
	if (read != SUDOKU_LDFAIL) {
		wprintf(L"Successfully load sudoku from '%ls'\nGiven number: %"SUDOKU_WIOFMT L"\n",
			load_path, read);
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(handle, &Sudoku_buffinfo);
	}
	else {
		Sudoku_usage();
		return 0;
	}
	switch (args->argc) {
	case 2:
		Sudoku_solve(args->sudoku, false, REVOP);
		Sudoku_print(args->sudoku);
		break;
	case 3:
		if (wcscmp(L"--print", args->wargv[2]) != 0) {
			fwprintf(stderr, L"Unknown option: %ls\n", args->wargv[2]);
			Sudoku_usage();
		}
		else {
			Sudoku_solve(args->sudoku, true, REVOP);
		}
		break;
	default:
		SUDOKU_UNREACHABLE();
	}
	return 0;
}

int Sudoku_wmain(int argc, wchar_t **wargv)
{
	if (argc < MIN_ARGCNT || argc > MAX_ARGCNT) {
		Sudoku_usage();
		return 0;
	}
	signal(SIGINT, Sudoku_interupt_handler);
	Sudoku sudoku = SUDOKU_INITIALIZER;
	Sudoku_arg sudoku_arg = { &sudoku,argc,wargv };
	return Sudoku_proc(&sudoku_arg);
}
