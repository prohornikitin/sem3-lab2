#pragma once
#include <debug.h>

class Allocator
{
public:
	Allocator(void* memory, size_t size);

	void* alloc(size_t size, size_t alignment);
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

static char* align(char* ptr, size_t alignment)
{
	return ptr + (((uintptr_t)ptr) & (alignment-1));
}

Allocator::Allocator(void* memory, size_t size)
{
	Zone* zone = (Zone*)align((char*)memory, alignof(Zone));
	zone->size = size - sizeof(Zone);
	zone->isFree = true;
	DPRINTF("Allocator():{addr=%p, size=%ld, isFree=%d}", 
			zone, zone->size, zone->isFree);
	this->memory = align((char*)memory, alignof(Zone));
	this->size = size - sizeof(Zone);
	DPRINTF("Allocator() memory=%p, size=%zu", memory, this->size);
}

void* Allocator::alloc(size_t size, size_t alignment)
{
	DPRINTF("size=%zu, alignment=%zu", size, alignment);
	char* i = memory;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
		DPRINTF("zone:{size:%ld, isFree=%d}", zone->size, zone->isFree);
		i += sizeof(Zone);
		if(!zone->isFree)
		{
			i += zone->size;
			continue;
		}
		char* aligned = align(i, alignment);
		DPRINTF("unaligned=%p, aligned=%p", (void*)i, (void*)aligned);
		size_t available_size = i + zone->size - aligned;
		if(available_size <= size)
		{
			i += zone->size;
			continue;
		}
		if((available_size - size) >= (3 * sizeof(Zone)))
		{
			Zone* newZone = (Zone*)align(aligned + size, alignof(Zone));
			newZone->isFree = true;
			newZone->size = (uintptr_t)zone + zone->size - (uintptr_t)newZone;
			zone->size = ((uintptr_t)newZone) - ((uintptr_t)zone) - sizeof(Zone);
		}
		zone->isFree = false;
		return (void*)aligned;
	}
	return nullptr;
}

void Allocator::mergeFreeZones(Zone* first)
{
	char* i = (char*)first;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
		DPRINTF("zone:{addr=%p, size=%ld, isFree=%d}", 
			zone, zone->size, zone->isFree);
		if(!zone->isFree)
		{
			break;
		}
		i += zone->size + sizeof(Zone);
	}
	first->size = i - ((char*)first) - sizeof(Zone);
	DPRINTF("zone:{addr=%p, size=%ld, isFree=%d}",
		first, first->size, first->isFree);
}

void Allocator::free(void* ptr)
{
	if(ptr == nullptr) {
		return;
	}
	
	char* i = memory;
	Zone* firstFreeOfChunk = nullptr;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
		char* end = i + sizeof(Zone) + zone->size;

		if(zone->isFree && firstFreeOfChunk == nullptr)
		{
			firstFreeOfChunk = zone;
		}

		if(!zone->isFree && firstFreeOfChunk != nullptr)
		{
			firstFreeOfChunk = nullptr;
		}

		if(i < ptr && ptr < end)
		{
			zone->isFree = true;
			if(firstFreeOfChunk != nullptr)
			{
				mergeFreeZones(firstFreeOfChunk);
			}
			else
			{
				mergeFreeZones(zone);
			}
			return;
		}

		i += zone->size + sizeof(Zone);
	}
	free(ptr);
}
