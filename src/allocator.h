#pragma once
#include <stddef.h>
#include <stdint.h>

class Allocator
{
public:
	Allocator(void* memory, size_t size);

	void* Alloc(size_t size, size_t alignment);
	void* Alloc(size_t size);
	void* AllocArray(size_t size);
	void Free(void* ptr);
private:
	char* memory = nullptr;
	size_t size;

	struct Zone {
		uint64_t size;
		bool isFree;
	};

	void MergeFreeZones(Zone* first);
};

