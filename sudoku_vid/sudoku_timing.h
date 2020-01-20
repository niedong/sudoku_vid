/*
 * Copyright(c) 2020 niedong
 *
 * License under MIT license. For more information, visit
 *
 * https://github.com/niedong/sudoku_vid/blob/master/LICENSE
 */

#pragma once

/*
 * High-precision timing based on Windows API
 *
 *
 * Example usage:
 *      TIMING_BEGIN();
 *      ...Do something...
 *      TIMING_END();
 *      double used_time = USED_TIME();
 *      ...Do something with 'used_time'...
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
