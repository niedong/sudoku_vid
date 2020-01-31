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
	Sudoku_t board[SUDOKU_SIZE][SUDOKU_SIZE];
	Sudoku_stat stat;
}Sudoku;

typedef unsigned long long Sudoku_u64t;

typedef struct {
	Sudoku_u64t step;
	bool solvable;
}Sudoku_solve_t;

/* Definition of various loading result. */
typedef enum {
	Sudoku_load_success,
	Sudoku_fopen_failure,
	Sudoku_early_eof,
	Sudoku_invalid_value,
	Sudoku_illegal
}Sudoku_error_t;

typedef struct {
	Sudoku_error_t code;
	union {
		Sudoku_t read;
		Sudoku_t count;
		struct {
			Sudoku_t i;
			Sudoku_t j;
			Sudoku_t val;
		};
	};
}Sudoku_load_t;

/* This can hold as much as about 3e+38. Pretty much enough. */
typedef struct {
	Sudoku_u64t fst;
	Sudoku_u64t snd;
}Sudoku_u128t;

typedef struct {
	Sudoku_u128t ans;
}Sudoku_dfs_t;

typedef struct {
	bool rev;
}Sudoku_solve_args;

#define SUDOKU_INITIALIZER \
	{ { SUDOKU_EMPTY },{ 0 } }

#define SUDOKU_SOLVEARGS_INITIALIZER \
    { true };

SUDOKU_API(void) Sudoku_print(Sudoku *sudoku);

SUDOKU_API(Sudoku_load_t) Sudoku_load(Sudoku *sudoku, const char *path);

SUDOKU_API(Sudoku_solve_t) Sudoku_solve(Sudoku *sudoku, Sudoku_solve_args *args);

/* Sudoku_u128t printing macro */
#define SUDOKU_U128T_PRINT(extra, p_u128) \
    do { \
        (p_u128)->fst == 0 ? printf(extra "%llu\n", (p_u128)->snd) : \
            printf(extra "%llu%llu\n", (p_u128)->fst, (p_u128)->snd); \
	} while (0)

#define SUDOKU_U128T_RAW_PRINT(p_u128) SUDOKU_U128T_PRINT("", p_u128)

SUDOKU_API(bool) Sudoku_u128t_eq(Sudoku_u128t *p_u128, Sudoku_u64t val);

SUDOKU_API(Sudoku_dfs_t) Sudoku_dfs(Sudoku *sudoku);
