#pragma once
#include <cstdlib>
#include <cstdio>

#ifdef DEBUG
#	define DPRINTF(...) \
		std::fprintf(stderr, __VA_ARGS__); \
		std::fprintf(stderr, "\n");
#else
#	define DPRINTF(...)
#endif

#define FATAL(...) \
	std::fprintf(stderr, __VA_ARGS__); \
	std::fprintf(stderr, "\n"); \
	std::exit(1);
