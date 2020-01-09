#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <Windows.h>
#include "sudoku.h"
#include "sudoku_timing.h"

#define INC(val) ((val) + 1)
#define DEC(val) ((val) - 1)

static void Sudoku_ostream(Sudoku *sudoku, FILE *const ostream)
{
	assert(ostream != NULL);
	fwprintf(ostream, L"%ls", sudoku->wstr);
}

void Sudoku_print(Sudoku *sudoku)
{
	Sudoku_ostream(sudoku, stdout);
}

bool Sudoku_store(Sudoku *sudoku, const char *path)
{
	FILE *stream = fopen(path, "wb");
	if (stream == NULL) {
		perror(path);
		return false;
	}
	Sudoku_ostream(sudoku, stream);
	fclose(stream);
	return true;
}

bool Sudoku_wstore(Sudoku *sudoku, const wchar_t *wpath)
{
	FILE *stream = _wfopen(wpath, L"wb");
	if (stream == NULL) {
		_wperror(wpath);
		return false;
	}
	Sudoku_ostream(sudoku, stream);
	fclose(stream);
	return true;
}

static void Sudoku_strrf(Sudoku *sudoku)
{
	Sudoku_t i, j;
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX - 1; j++) {
			sudoku->wstr[SUDOKU_WSTRCH(i, j)] = sudoku->board[i][j] + L'0';
			sudoku->wstr[SUDOKU_WSTRCH(i, j) + 1] = L' ';
		}
		sudoku->wstr[SUDOKU_WSTRCH(i, j)] = sudoku->board[i][j] + L'0';
		sudoku->wstr[SUDOKU_WSTRCH(i, j) + 1] = L'\n';
	}
	sudoku->wstr[i * SUDOKU_SIZE * 2] = L'\0';
}

static bool Sudoku_istream(Sudoku *sudoku, FILE *const istream)
{
	assert(istream != NULL);
	Sudoku_t i, j, count = 0;
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX; j++) {
			int fstatus = fscanf(istream, "%"SUDOKU_IOFMT, &sudoku->board[i][j]);
			if (fstatus == EOF) {
				fwprintf(stderr, L"Early EOF. Expecting %"SUDOKU_WIOFMT
					L" numbers. Load %"SUDOKU_WIOFMT
					L" instead\n", SUDOKU_CELL, count);
				return false;
			}
			Sudoku_t val = sudoku->board[i][j];
			if ((val != SUDOKU_EMPTY) &&
				((val < SUDOKU_MIN || val > SUDOKU_MAX) ||
				(++(sudoku->stat.rowstat[i][DEC(val)]) == 2) ||
					(++(sudoku->stat.colstat[j][DEC(val)]) == 2) ||
					(++(sudoku->stat.blkstat[SUDOKU_BLK(i, j)][DEC(val)]) == 2)))
			{
				fwprintf(stderr, L"An error occurred at row %"SUDOKU_WIOFMT
					L" column %"SUDOKU_WIOFMT L", value: %"SUDOKU_WIOFMT
					L". Illegal sudoku\n",
					INC(i), INC(j), val);
				return false;
			}
			count++;
		}
	}
	Sudoku_strrf(sudoku);
	return true;
}

bool Sudoku_load(Sudoku *sudoku, const char *path)
{
	FILE *stream = fopen(path, "rb");
	if (stream == NULL) {
		perror(path);
		return false;
	}
	int ret = Sudoku_istream(sudoku, stream);
	fclose(stream);
	return ret;
}

bool Sudoku_wload(Sudoku *sudoku, const wchar_t *wpath)
{
	FILE *stream = _wfopen(wpath, L"rb");
	if (stream == NULL) {
		_wperror(wpath);
		return false;
	}
	int ret = Sudoku_istream(sudoku, stream);
	fclose(stream);
	return ret;
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

static void Sudoku_stepadd(Sudoku *sudoku, Sudoku_t i, Sudoku_t j)
{
	sudoku->step[sudoku->cur_step].i = i;
	sudoku->step[sudoku->cur_step].j = j;
	++sudoku->cur_step;
}

double Sudoku_solve(Sudoku *sudoku, bool print, bool rev)
{
	Sudoku_t i, j;
	HANDLE handle = NULL;
	CONSOLE_SCREEN_BUFFER_INFO buff_info;
	CONSOLE_CURSOR_INFO cursor_info;
	if (print) {
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(handle, &buff_info);
		GetConsoleCursorInfo(handle, &cursor_info);
		cursor_info.bVisible = FALSE;
		SetConsoleCursorInfo(handle, &cursor_info);
	}
	TIMING_BEGIN();
	for (i = 0; i < SUDOKU_MAX; i++) {
		for (j = 0; j < SUDOKU_MAX; j++) {
			if (sudoku->board[i][j] != SUDOKU_EMPTY) {
				/* The cell already has a number. Skip. */
				continue;
			}
			Sudoku_t val;
			if (rev) {
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
				if (rev) {
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
				/*
				 * This is critical. We are trying to find next
				 * suitable number that can fill in its cell.
				 */
				assert(--sudoku->cur_step >= 0);
				Sudoku_t prev_i = sudoku->step[sudoku->cur_step].i;
				Sudoku_t prev_j = sudoku->step[sudoku->cur_step].j;
				i = prev_i, j = prev_j;
				Sudoku_t prev_val = sudoku->board[i][j];
				if (rev) {
					start_val = DEC(prev_val);
				}
				else {
					start_val = INC(prev_val);
				}
				LOOP_FIND_PROPER(start_val, &sudoku->stat, i, j) {
					if (rev) {
						start_val--;
					}
					else {
						start_val++;
					}
				}
				if (start_val > SUDOKU_MAX || start_val < SUDOKU_MIN) {
					/* We got stuck at previous step. Keep going back. */
					STAT_RESTORE(&sudoku->stat, i, j, prev_val);
					sudoku->board[i][j] = SUDOKU_EMPTY;
					sudoku->wstr[SUDOKU_WSTRCH(i, j)] = SUDOKU_EMPTY + L'0';
					goto label_prevstep;
				}
				else {
					/*
					 * Now we got the other suitable number to
					 * fill in its cell. We keep exploring. At
					 * the same time, we record this step and
					 * refresh sudoku status.
					 */
					STAT_RESTORE(&sudoku->stat, i, j, prev_val);
					sudoku->board[i][j] = start_val;
					sudoku->wstr[SUDOKU_WSTRCH(i, j)] = start_val + L'0';
					if (print) {
						Sudoku_print(sudoku);
					}
					Sudoku_stepadd(sudoku, i, j);
					STAT_REFRESH(&sudoku->stat, i, j, start_val);
				}
			}
			else {
				/*
				 * We found a suitable number to fill in this cell.
				 * We keep exporing, record this step, and refresh
				 * sudoku status.
				 */
				sudoku->board[i][j] = val;
				sudoku->wstr[SUDOKU_WSTRCH(i, j)] = val + L'0';
				if (print) {
					Sudoku_print(sudoku);
				}
				Sudoku_stepadd(sudoku, i, j);
				STAT_REFRESH(&sudoku->stat, i, j, val);
			}
			if (print) {
				SetConsoleCursorPosition(handle, buff_info.dwCursorPosition);
			}
		}
	}
	TIMING_END();
	if (print) {
		cursor_info.bVisible = TRUE;
		SetConsoleCursorInfo(handle, &cursor_info);
		buff_info.dwCursorPosition.Y += SUDOKU_SIZE;
		SetConsoleCursorPosition(handle, buff_info.dwCursorPosition);
	}
	return USED_TIME();
}
