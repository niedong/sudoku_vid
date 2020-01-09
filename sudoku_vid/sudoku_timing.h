#ifndef SUDOKU_TIMING_H
#define SUDOKU_TIMING_H

/*
 * Example usage:
 *		TIMING_BEGIN();
 *		...Do something...
 *		Timing end();
 *		double used_time = USED_TIME();
 */

#include <Windows.h>

/* Begin timing. */
#define TIMING_BEGIN() \
    LARGE_INTEGER __start__, __freq__, __end__; \
    QueryPerformanceFrequency(&__freq__); \
    QueryPerformanceCounter(&__start__); \

/* End timing. */
#define TIMING_END() \
    QueryPerformanceCounter(&__end__);

/* Get used time. */
#define USED_TIME() \
	((double)(__end__.QuadPart - __start__.QuadPart) / (double)(__freq__.QuadPart))

#endif /* !SUDOKU_TIMING_H */
