/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#pragma once
#include <stddef.h> /* For size_t */
#include <stdbool.h> /* For bool type */
#include <assert.h> /* For assertion */

/*
 * --SUDOKU_BLKSIZE--
 *  _____ _____ _____ _____ _____ _____ _____ _____ _____
 * |    1|    2|    3|    4|    5|    6|    7|    8|    9|
 *
 * ------------------------SUDOKU_SIZE--------------------
 */

/* Size of sudoku block. Default is 3 * 3. */
#define SUDOKU_BLKSIZE 3

/* Size of sudoku board. Default is 9 * 9. */
#define SUDOKU_SIZE 9

/* Compiler check. */
#if SUDOKU_BLKSIZE * SUDOKU_BLKSIZE != SUDOKU_SIZE
#error SUDOKU_SIZE should be a square of SUDOKU_BLKSIZE
#endif

#define SUDOKU_CELL (SUDOKU_SIZE * SUDOKU_SIZE)

/* The way to treat sudoku number. */
typedef unsigned int Sudoku_t;

#define SUDOKU_IOFMT "%u"

/*
 * Empty cell.
 * Do not modify this unless you know what you're doing
 */
#define SUDOKU_EMPTY 0

/* Minimum number in sudoku. */
#define SUDOKU_MIN 1

/* Maximum number in sudoku. */
#define SUDOKU_MAX SUDOKU_SIZE

/* Sudoku document macro */
#define	SUDOKU_DOC(str) str

/* Sudoku API macro */
#define SUDOKU_API(T) T

#define INC(val) ((val) + 1)

#define DEC(val) ((val) - 1)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

/* No operation */
#define SUDOKU_NOP() ((void)0)

#define TO_STRING(x) #x

#define CONCAT(x, y) x ## y
