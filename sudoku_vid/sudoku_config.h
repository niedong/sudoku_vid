/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#pragma once
#include <stddef.h> /* For size_t */
#include <wchar.h> /* For unicode support */
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
typedef char Sudoku_t;

/* Sudoku I/O format. */
#define SUDOKU_IOFMT "hhd"

/* Sudoku unicode I/O format. */
#define SUDOKU_WIOFMT L"hhd"

/* Empty cell. */
#define SUDOKU_EMPTY 0

/* Minimum number in sudoku. */
#define SUDOKU_MIN 1

/* Maximum number in sudoku. */
#define SUDOKU_MAX SUDOKU_SIZE

/* Sudoku document macro */
#define	SUDOKU_DOC(str) str

/* Sudoku API macro */
#define SUDOKU_API(T) T

/* Instruction that can't be reached */
#define SUDOKU_UNREACHABLE() abort()

/* No operation */
#define SUDOKU_NOP() ((void)0)
