#include <allocator.h>


static char* align(char* ptr, size_t alignment)
{
	return ptr + (((uintptr_t)ptr) & (alignment-1));
}

Allocator::Allocator(void* memory, size_t size)
{
	if(size <= sizeof(Zone))
	{
		this->size = 0;
		memory = nullptr;
		return;
	}
	Zone* zone = (Zone*)align((char*)memory, alignof(Zone));
	zone->size = size - sizeof(Zone);
	zone->isFree = true;
	this->memory = align((char*)memory, alignof(Zone));
	this->size = size - sizeof(Zone);
}

void* Allocator::alloc(size_t size, size_t alignment)
{
	char* i = memory;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
		i += sizeof(Zone);
		if(!zone->isFree)
		{
			i += zone->size;
			continue;
		}
		char* aligned = align(i, alignment);
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

void* Allocator::alloc(size_t size)
{
	size_t alignment = 1;
	while(size > alignment)
	{
		alignment *= 2;
	}
	return alloc(size, alignment);
}

void Allocator::mergeFreeZones(Zone* first)
{
	char* i = (char*)first;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
		if(!zone->isFree)
		{
			break;
		}
		i += zone->size + sizeof(Zone);
	}
	first->size = i - ((char*)first) - sizeof(Zone);
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
