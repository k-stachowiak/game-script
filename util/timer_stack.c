/* Copyright (C) 2015 Krzysztof Stachowiak */

#include "collection.h"

#if defined(__unix__) || defined(linux)
#	if __STDC_VERSION__ >= 199901L
#		define _XOPEN_SOURCE 600
#	else
#		define _XOPEN_SOURCE 500
#	endif
#endif

#if defined(_WIN32) || defined(WIN32)

#include <windows.h>

static struct StartStack {
	LARGE_INTEGER *data;
	int size, cap;
} starts = { NULL, 0, 0 };

void ts_start(void)
{
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	ARRAY_APPEND(starts, start);
}

long long ts_ustop(void)
{
	LARGE_INTEGER start, end, freq, elapsed;

	start = starts.data[starts.size - 1];
	ARRAY_POP(starts);

	QueryPerformanceCounter(&end);

	QueryPerformanceFrequency(&freq);

	elapsed.QuadPart = end.QuadPart - start.QuadPart;
	elapsed.QuadPart *= 1000000 / freq.QuadPart;

	return elapsed.QuadPart;
}

void ts_deinit(void)
{
	ARRAY_FREE(starts);
}

#elif defined(__unix__) || defined(linux)

#include <time.h>

static struct StartStack {
	struct timespec *data;
	int size, cap;
} starts = { NULL, 0, 0 };

void ts_start(void)
{
	struct timespec start;
	clock_gettime(CLOCK_MONOTONIC, &start);
	ARRAY_APPEND(starts, start);
}

long long ts_ustop(void)
{
	struct timespec start, end;

	start = starts.data[starts.size - 1];
	ARRAY_POP(starts);

	clock_gettime(CLOCK_MONOTONIC, &end);

	return
		((end.tv_sec * 1000000) + (end.tv_nsec / 1000)) -
		((start.tv_sec * 1000000) + (start.tv_nsec / 1000));
}

void ts_deinit(void)
{
	ARRAY_FREE(starts);
}

#endif

