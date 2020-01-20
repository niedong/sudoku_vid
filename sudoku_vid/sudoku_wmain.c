/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "sudoku.h"
#include "sudoku_wmain.h"

typedef struct {
	Sudoku *sudoku;
	int argc;
	wchar_t **wargv;
}Sudoku_arg;

#define SUDOKU_MAX_ARGCNT 4

/* Instruction that can't be reached */
#define SUDOKU_UNREACHABLE() abort()

static CONSOLE_SCREEN_BUFFER_INFO Sudoku_buffinfo;

/*
 * Handler when user presses 'ctrl+c'. Cleanly exit the program
 */
static void Sudoku_interrupt_handler(int signal_code)
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
		SUDOKU_DOC(L"Usage: sudoku_vid  [-h | --help] [<load path> [--print] [--norev]]");
	fwprintf(stderr, L"%ls\n", Sudoku_usagewstr);
}

static void Split_line(void)
{
	wchar_t split[SUDOKU_SIZE * 2];
	wmemset(split, L'-', SUDOKU_SIZE * 2 - 1);
	split[SUDOKU_SIZE * 2 - 1] = L'\0';
	wprintf(L"%ls\n", split);
}

static bool Sudoku_solve_proc(Sudoku *sudoku, bool print, bool rev)
{
	Sudoku_solve_t result = Sudoku_solve(sudoku, print, rev);
	if (result.solvable == false) {
		wprintf(L"The sudoku is not solvable!\n");
		return false;
	}
	wprintf(L"Finished. Used step: %llu. Used time: %e\n",
		result.step, result.used_time);
	if (print == false) {
		Sudoku_print(sudoku);
	}
	return true;
}

static int Sudoku_proc(Sudoku_arg *args)
{
	if (args->argc == 2 &&
		(wcscmp(args->wargv[1], L"--help") == 0) ||
		(wcscmp(args->wargv[1], L"-h") == 0))
	{
		Sudoku_usage();
		return 0;
	}
	wchar_t *load_path = args->wargv[1];
	Sudoku_load_t result = Sudoku_wload(args->sudoku, load_path);
	switch (result.code) {
	case Sudoku_load_success:
		wprintf(L"Successfully load sudoku from '%ls'\n"
			L"Given number: %u\nOriginal:\n", load_path, result.read);
		Sudoku_print(args->sudoku);
		Split_line();
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(handle, &Sudoku_buffinfo);
		/* Set up signal handler only when successfully loaded sudoku */
		signal(SIGINT, Sudoku_interrupt_handler);
		break;
	case Sudoku_fopen_failure:
		_wperror(load_path);
		break;
	case Sudoku_early_eof:
		fwprintf(stderr, L"Early EOF: expecting %u numbers,"
			L" load %u instead\n", SUDOKU_CELL, result.count);
		break;
	case Sudoku_invalid_value:
		fwprintf(stderr, L"Invalid number: row: %u, column: %u, value: %u\n",
			INC(result.i), INC(result.j), result.val);
		break;
	case Sudoku_illegal:
		fwprintf(stderr, L"Illegal sudoku: row: %u, column: %u."
			L" Multiple occurrences of %u\n",
			INC(result.i), INC(result.j), result.val);
		break;
	default:
		SUDOKU_UNREACHABLE();
	}
	if (result.code != Sudoku_load_success) {
		fwprintf(stderr, L"\nLoad sudoku from '%ls' failed\n", load_path);
		return 0;
	}
	switch (args->argc) {
	case 2:
		Sudoku_solve_proc(args->sudoku, false, true);
		break;
	case 3:
		if (wcscmp(L"--print", args->wargv[2]) == 0) {
			Sudoku_solve_proc(args->sudoku, true, true);
		}
		else if (wcscmp(L"--norev", args->wargv[2]) == 0) {
			Sudoku_solve_proc(args->sudoku, false, false);
		}
		else {
			fwprintf(stderr, L"Unknown option: %ls\n", args->wargv[2]);
			Sudoku_usage();
		}
		break;
	case 4:
		SUDOKU_NOP();
		bool print = false, rev = true;
		int arg_start = 2;
		while (arg_start < args->argc) {
			wchar_t *op = args->wargv[arg_start];
			if (wcscmp(L"--print", op) == 0) {
				print = true;
			}
			else if (wcscmp(L"--norev", op) == 0) {
				rev = false;
			}
			else {
				fwprintf(stderr, L"Unknown option: %ls\n", op);
				Sudoku_usage();
				return 0;
			}
			arg_start++;
		}
		Sudoku_solve_proc(args->sudoku, print, rev);
		break;
	default:
		SUDOKU_UNREACHABLE();
	}
	return 0;
}

int Sudoku_wmain(int argc, wchar_t **wargv)
{
	if (argc == 1) {
		fwprintf(stderr, L"sudoku_vid: Missing load path\n\n"
			L"Try 'sudoku_vid --help' for more information\n");
		return 0;
	}
	if (argc > SUDOKU_MAX_ARGCNT) {
		fwprintf(stderr, L"Too many arguments\n");
		Sudoku_usage();
		return 0;
	}
	Sudoku sudoku = SUDOKU_INITIALIZER;
	Sudoku_arg sudoku_arg = { &sudoku,argc,wargv };
	return Sudoku_proc(&sudoku_arg);
}
