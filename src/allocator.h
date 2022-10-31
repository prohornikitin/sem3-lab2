#pragma once
#include <debug.h>
#include <stddef.h>
#include <stdint.h>

class Allocator
{
public:
	Allocator(void* memory, size_t size);

	void* alloc(size_t size, size_t alignment);
	void* alloc(size_t size);
	void free(void* ptr);
private:
	char* memory = nullptr;
	size_t size;

	struct Zone {
		uint64_t size;
		bool isFree;
	};

	void mergeFreeZones(Zone* first);
};

