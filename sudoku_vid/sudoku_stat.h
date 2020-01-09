#ifndef SUDOKU_STAT_H
#define SUDOKU_STAT_H

#include "sudoku_config.h"

/*
 * Notice that for a standard sudoku board, there are 9 rows,
 * 9 columns, and 9 blocks. That also means that no matter what
 * the size of sudoku board is, its row count, column count,
 * and block count are all the same.
 *
 *
 * The three 2-dimensional array represent the sudoku status.
 * rowstat: Check if there are distinct numbers in each row.
 *
 *
 * e.g.
 *
 * 1.
 * if (rowstat[1][6] == 0) {
 *		// If number 7 doesn't occur in row 2, we are in here...
 * }
 *
 *
 * 2.
 * if (colstat[4][3] == 1) {
 *		// If number 4 occurs in column 5 once, we are in here...
 * }
 *
 *
 * 3.
 * if (blkstat[7][0] == 2) {
 *		// If number 1 occurs in block 8 twice, we are in here...
 * }
 *
 *
 */

typedef struct {
	Sudoku_t rowstat[SUDOKU_SIZE][SUDOKU_SIZE];
	Sudoku_t colstat[SUDOKU_SIZE][SUDOKU_SIZE];
	Sudoku_t blkstat[SUDOKU_SIZE][SUDOKU_SIZE];
}Sudoku_stat;

/*
 * Row and column are started from 1. "i" and "j" are
 * started from 0. In other words, the coordinate of number
 * at row 3 column 4 is (2, 3).
 *
 * For example, if we want to find which block (i = 4, j = 4)
 * belongs to, then SUDOKU_BLK(4, 4) returns 3 + 3 / 3 = 4.
 * So, it is actually in block 5 if we start from block 1.
 */

#define SUDOKU_BLKSTART(n) ((n) - (n) % SUDOKU_BLKSIZE)

/* Start from 0. */
#define SUDOKU_BLK(i, j) \
	(SUDOKU_BLKSTART(i) + SUDOKU_BLKSTART(j) / SUDOKU_BLKSIZE)

#endif /* !SUDOKU_STAT_H */
