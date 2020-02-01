/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"

typedef struct {
	Sudoku_t i, j;
}Sudoku_step;

static void Sudoku_ostream(Sudoku *sudoku, FILE *const ostrm)
{
	assert(ostrm != NULL);
	Sudoku_t i, j;
	for (i = 0; i < SUDOKU_MAX; i++) {
		j = 0;
		fprintf(ostrm, SUDOKU_IOFMT, sudoku->board[i][j]);
		for (j = 1; j < DEC(SUDOKU_MAX); j++) {
			fprintf(ostrm, " "SUDOKU_IOFMT, sudoku->board[i][j]);
		}
		fprintf(ostrm, " "SUDOKU_IOFMT"\n", sudoku->board[i][j]);
	}
}

void Sudoku_print(Sudoku *sudoku)
{
	Sudoku_ostream(sudoku, stdout);
}

static Sudoku_load_t Sudoku_istream(Sudoku *sudoku, FILE *const istrm)
{
	assert(istrm != NULL);
	Sudoku_t i, j, read_cnt = 0, cnt = 0;
	Sudoku_load_t rst = { 0 };
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX; j++) {
			int fstatus = fscanf(istrm, SUDOKU_IOFMT, &sudoku->board[i][j]);
			if (fstatus == EOF) {
				rst.code = Sudoku_early_eof;
				rst.count = cnt;
				return rst;
			}
			cnt++;
			Sudoku_t val = sudoku->board[i][j];
			if (val == SUDOKU_EMPTY) {
				continue;
			}
			if (val < SUDOKU_MIN || val > SUDOKU_MAX) {
				rst.code = Sudoku_invalid_value;
			}
			else if ((++(sudoku->stat.rowstat[i][DEC(val)]) == 2) ||
				(++(sudoku->stat.colstat[j][DEC(val)]) == 2) ||
				(++(sudoku->stat.blkstat[SUDOKU_BLK(i, j)][DEC(val)]) == 2))
			{
				rst.code = Sudoku_illegal;
			}
			else {
				read_cnt++;
				continue;
			}
			rst.i = i;
			rst.j = j;
			rst.val = val;
			return rst;
		}
	}
	rst.code = Sudoku_load_success;
	rst.read = read_cnt;
	return rst;
}

Sudoku_load_t Sudoku_load(Sudoku *sudoku, const char *path)
{
	Sudoku_load_t result = { 0 };
	FILE *strm = fopen(path, "rb");
	if (strm == NULL) {
		result.code = Sudoku_fopen_failure;
		return result;
	}
	result = Sudoku_istream(sudoku, strm);
	fclose(strm);
	return result;
}

bool Sudoku_store(Sudoku *sudoku, const char *path)
{
	FILE *strm = fopen(path, "wb");
	if (strm == NULL) {
		return false;
	}
	Sudoku_ostream(sudoku, strm);
	fclose(strm);
	return true;
}

static void Sudoku_addstep(Sudoku_step *step, Sudoku_t i, Sudoku_t j,
                           Sudoku_t *p_cur)
{
	Sudoku_t cur_step = *p_cur;
	step[cur_step].i = i;
	step[cur_step].j = j;
	(*p_cur)++;
}

#define LOOP_FIND_PROPER(val, stat, i, j) \
	while (((val) >= SUDOKU_MIN && (val) <= SUDOKU_MAX) && \
    ((stat)->rowstat[i][DEC(val)] || \
     (stat)->colstat[j][DEC(val)] || \
      (stat)->blkstat[SUDOKU_BLK(i, j)][DEC(val)]))

#define STAT_REFRESH(stat, i, j, val) \
	do { \
		assert(++((stat)->rowstat[i][DEC(val)]) == 1); \
		assert(++((stat)->colstat[j][DEC(val)]) == 1); \
		assert(++((stat)->blkstat[SUDOKU_BLK(i, j)][DEC(val)]) == 1); \
	} while (0)

#define STAT_RESTORE(stat, i, j, val) \
    do { \
		assert(--((stat)->rowstat[i][DEC(val)]) == 0); \
		assert(--((stat)->colstat[j][DEC(val)]) == 0); \
		assert(--((stat)->blkstat[SUDOKU_BLK(i, j)][DEC(val)]) == 0); \
	} while (0)

static bool Sudoku_preslv(Sudoku *sudoku)
{
	Sudoku_t i, j;
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX; j++) {
			if (sudoku->board[i][j] != SUDOKU_EMPTY) {
				continue;
			}
			Sudoku_t val = SUDOKU_MIN;
			LOOP_FIND_PROPER(val, &sudoku->stat, i, j) {
				val++;
			}
			if (val > SUDOKU_MAX) {
				return false;
			}
		}
	}
	return true;
}

static void Sudoku_place(Sudoku *sudoku, Sudoku_t i, Sudoku_t j, Sudoku_t val)
{
	sudoku->board[i][j] = val;
	STAT_REFRESH(&sudoku->stat, i, j, val);
}

static void Sudoku_remove(Sudoku *sudoku, Sudoku_t i, Sudoku_t j, Sudoku_t val)
{
	sudoku->board[i][j] = SUDOKU_EMPTY;
	STAT_RESTORE(&sudoku->stat, i, j, val);
}

Sudoku_solve_t Sudoku_solve(Sudoku *sudoku, Sudoku_solve_args *args)
{
	Sudoku_solve_t result = { 0,true };
	bool solvable = Sudoku_preslv(sudoku);
	if (solvable == false) {
		result.solvable = false;
		return result;
	}
	Sudoku_t i, j;
	Sudoku_step step[SUDOKU_CELL + 1];
	Sudoku_t cur_step = 0;
	Sudoku_u64t step_cnt = 0;
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX; j++) {
			if (sudoku->board[i][j] != SUDOKU_EMPTY) {
				/* The cell already has a number. Skip. */
				continue;
			}
			Sudoku_t val;
			if (args->rev) {
				val = SUDOKU_MAX;
			}
			else {
				val = SUDOKU_MIN;
			}
			LOOP_FIND_PROPER(val, &sudoku->stat, i, j) {
				/*
				 * Find a number that does not
				 * violate the rules of sudoku.
				 */
				if (args->rev) {
					val--;
				}
				else {
					val++;
				}
			}
			if (val > SUDOKU_MAX || val < SUDOKU_MIN) {
				/*
				 * Now we got stuck, because we cannot find a
				 * number to fill in this cell. We have no way
				 * other than going back to the previous step.
				 */
				Sudoku_t start_val;
			label_prevstep:
				assert(--cur_step >= 0);
				Sudoku_t prev_i = step[cur_step].i;
				Sudoku_t prev_j = step[cur_step].j;
				i = prev_i, j = prev_j;
				Sudoku_t prev_val = sudoku->board[i][j];
				/*
				 * This is critical. We are trying to find next
				 * suitable number that can fill in its cell.
				 */
				if (args->rev) {
					start_val = DEC(prev_val);
				}
				else {
					start_val = INC(prev_val);
				}
				LOOP_FIND_PROPER(start_val, &sudoku->stat, i, j) {
					if (args->rev) {
						start_val--;
					}
					else {
						start_val++;
					}
				}
				if (start_val > SUDOKU_MAX || start_val < SUDOKU_MIN) {
					/* We got stuck at previous step. Keep going back. */
					Sudoku_remove(sudoku, i, j, prev_val);
					goto label_prevstep;
				}
				else {
					/*
					 * Now we got the other suitable number to
					 * fill in its cell. We keep exploring. At
					 * the same time, we record this step and
					 * refresh sudoku status.
					 */
					Sudoku_remove(sudoku, i, j, prev_val);
					Sudoku_place(sudoku, i, j, start_val);
					Sudoku_addstep(step, i, j, &cur_step);
					step_cnt++;
				}
			}
			else {
				/*
				 * We found a suitable number to fill in this cell.
				 * We keep exporing, record this step, and refresh
				 * sudoku status.
				 */
				Sudoku_place(sudoku, i, j, val);
				Sudoku_addstep(step, i, j, &cur_step);
				step_cnt++;
			}
		}
	}
	result.step = step_cnt;
	return result;
}

static Sudoku_u128t *Sudoku_u128t_inc(Sudoku_u128t *p_u128)
{
	p_u128->snd == ULLONG_MAX ? p_u128->snd = 0, p_u128->fst++ : p_u128->snd++;
	return p_u128;
}

bool Sudoku_u128t_eq(Sudoku_u128t *p_u128, Sudoku_u64t val)
{
	return p_u128->fst == 0 && p_u128->snd == val;
}

static void Sudoku_dfs_impl(Sudoku *sudoku, Sudoku_t n,
                            Sudoku_u128t *cnt)
{
	if (n == SUDOKU_CELL) {
		Sudoku_u128t_inc(cnt);
		return;
	}
	Sudoku_t i = n / SUDOKU_SIZE, j = n % SUDOKU_SIZE;
	if (sudoku->board[i][j] != SUDOKU_EMPTY) {
		Sudoku_dfs_impl(sudoku, n + 1, cnt);
	}
	else {
		Sudoku_t val = SUDOKU_MAX;
	label_findnext:
		LOOP_FIND_PROPER(val, &sudoku->stat, i, j) {
			val--;
		}
		if (val >= SUDOKU_MIN) {
			Sudoku_place(sudoku, i, j, val);
			Sudoku_dfs_impl(sudoku, n + 1, cnt);
			Sudoku_remove(sudoku, i, j, val);
			val--;
			goto label_findnext;
		}
	}
}

Sudoku_dfs_t Sudoku_dfs(Sudoku *sudoku)
{
	Sudoku end = SUDOKU_INITIALIZER;
	memcpy(&end, sudoku, sizeof(Sudoku));
	Sudoku_dfs_t ret = { 0 };
	Sudoku_solve_args solve_args = { false };
	Sudoku_solve_t result = Sudoku_solve(&end, &solve_args);
	if (result.solvable == false) {
		return ret;
	}
	Sudoku_dfs_impl(sudoku, 0, &ret.ans);
	/*
	 * This dfs recursion doesn't really solve 'sudoku'. We give it a
	 * default solution instead.
	 */
	memcpy(sudoku, &end, sizeof(Sudoku));
	return ret;
}
