/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#pragma once
#include "sudoku_stat.h"

typedef struct {
	Sudoku_t i, j;
}Sudoku_step;

typedef struct {
	Sudoku_t board[SUDOKU_SIZE][SUDOKU_SIZE];
	wchar_t wstr[SUDOKU_SIZE * SUDOKU_SIZE * 2 + 1];
	Sudoku_stat stat;
	Sudoku_step step[SUDOKU_CELL + 1];
	Sudoku_t cur_step;
}Sudoku;

typedef struct {
	unsigned long long step;
	double used_time;
	bool solvable;
}Sudoku_solve_t;

typedef enum {
	Sudoku_load_success,
	Sudoku_fopen_failure,
	Sudoku_early_eof,
	Sudoku_invalid_value,
	Sudoku_illegal
}Sudoku_error_t;

typedef struct {
	Sudoku_error_t error;
	union {
		Sudoku_t read;
		Sudoku_t count;
		struct {
			Sudoku_t val;
			Sudoku_t i;
			Sudoku_t j;
		};
	};
}Sudoku_load_t;

#define SUDOKU_INITIALIZER \
	{ { SUDOKU_EMPTY },{ 0 },{ 0 },{ 0 },0 }

#define SUDOKU_WSTRCH(i, j) ((i) * SUDOKU_SIZE * 2 + (j) * 2)

/* Print the board to the console. */
SUDOKU_API(void) Sudoku_print(Sudoku *sudoku);

/*
 * Load sudoku board from unicode-format "wpath".
 */
SUDOKU_API(Sudoku_load_t) Sudoku_wload(Sudoku *sudoku, const wchar_t *wpath);

/*
 * @print: whether visualize the result
 * @rev:   false: start from 1
 *         true:  start from 9
 */
SUDOKU_API(Sudoku_solve_t) Sudoku_solve(Sudoku *sudoku, bool print, bool rev);
