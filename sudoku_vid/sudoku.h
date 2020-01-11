#ifndef SUDOKU_H
#define SUDOKU_H

#include "sudoku_stat.h"
#include "sudoku_step.h"

typedef struct _Sudoku {
	Sudoku_t board[SUDOKU_SIZE][SUDOKU_SIZE];
	wchar_t wstr[SUDOKU_SIZE * SUDOKU_SIZE * 2 + 1];
	Sudoku_stat stat;
	Sudoku_step step[SUDOKU_CELL + 1];
	Sudoku_t cur_step;
}Sudoku;

#define SUDOKU_INITIALIZER \
	{ { SUDOKU_EMPTY },{ 0 },{ 0 },{ 0 },0 }

#define SUDOKU_WSTRCH(i, j) ((i) * SUDOKU_SIZE * 2 + (j) * 2)

#define SUDOKU_LDFAIL (-1)

/* Print the board to the console, if you need this. */
SUDOKU_API(void) Sudoku_print(Sudoku *sudoku);

/*
 * Store the sudoku board to "path".
 * Return true on success, false on failure.
 */
SUDOKU_API(bool) Sudoku_store(Sudoku *sudoku, const char *path);

/*
 * Store the sudoku board to unicode-format "wpath".
 * Return true on success, false on failure.
 */
SUDOKU_API(bool) Sudoku_wstore(Sudoku *sudoku, const wchar_t *wpath);

/*
 * Load sudoku board from "path".
 * Return true on success, false on failure.
 */
SUDOKU_API(Sudoku_t) Sudoku_load(Sudoku *sudoku, const char *path);

/*
 * Load sudoku board from unicode-format "wpath".
 * Return true on success, false on failure.
 */
SUDOKU_API(Sudoku_t) Sudoku_wload(Sudoku *sudoku, const wchar_t *wpath);

/*
 * print: whether visualize the result
 * rev: false: start from 1
 *      true:  start from 9
 */
SUDOKU_API(double) Sudoku_solve(Sudoku *sudoku, bool print, bool rev);

#endif /* !SUDOKU_H */
