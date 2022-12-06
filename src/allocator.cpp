#include <allocator.h>
#include <debug.h>

static char * Align(char * ptr, size_t alignment)
{
	return ptr + (((uintptr_t)ptr) & (alignment - 1));
}

Allocator::Allocator(void * memory, size_t size)
{
	DPRINTF("Allocator(%p, %zu)", memory, size);
	if(size <= sizeof(Zone))
	{
		this->size = 0;
		memory = nullptr;
		return;
	}
	Zone * zone = (Zone *)Align((char *)memory, alignof(Zone));
	zone->size = size - sizeof(Zone);
	zone->isFree = true;
	this->memory = Align((char *)memory, alignof(Zone));
	this->size = size - sizeof(Zone);
}

void * Allocator::Alloc(size_t size, size_t alignment)
{
	char * i = memory;
	while(i < memory + this->size)
	{
		Zone * zone = (Zone *)i;
		i += sizeof(Zone);
		if(!zone->isFree)
		{
			i += zone->size;
			continue;
		}
		char * aligned = Align(i, alignment);
		size_t available_size = i + zone->size - aligned;
		if(available_size <= size)
		{
			i += zone->size;
			continue;
		}
		if((available_size - size) >= (3 * sizeof(Zone)))
		{
			Zone * newZone = (Zone *)Align(aligned + size, alignof(Zone));
			newZone->isFree = true;
			newZone->size = (uintptr_t)zone + zone->size - (uintptr_t)newZone;
			zone->size = ((uintptr_t)newZone) - ((uintptr_t)zone) - sizeof(Zone);
		}
		zone->isFree = false;
		DPRINTF("memory %p allocated size=%zu, requestedSize=%zu", (void *)aligned, zone->size,
				size);
		return (void *)aligned;
	}
	DPRINTF("Not enough memory, allocator will return nullptr\n");
	return nullptr;
}

void * Allocator::Alloc(size_t size)
{
	return Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
}

void Allocator::MergeFreeZones(Zone * first)
{
	char * i = (char *)first;
	while(i < memory + this->size)
	{
		Zone * zone = (Zone *)i;
		if(!zone->isFree)
		{
			break;
		}
		i += zone->size + sizeof(Zone);
	}
	first->size = i - ((char *)first) - sizeof(Zone);
}

void Allocator::Free(void * ptr)
{
	if(ptr == nullptr)
	{
		DPRINTF("nullptr passed to Allocator::free()");
		return;
	}

	char * i = memory;
	Zone * firstFreeOfChunk = nullptr;
	while(i < memory + this->size)
	{
		Zone * zone = (Zone *)i;
		char * end = i + sizeof(Zone) + zone->size;

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
				MergeFreeZones(firstFreeOfChunk);
			}
			else
			{
				MergeFreeZones(zone);
			}
			DPRINTF("memory %p sucessfully freed", ptr);
			return;
		}

		i += zone->size + sizeof(Zone);
	}
	DPRINTF("memory %p was allocated by another allocator and so cannot be freed", ptr);
}

void * operator new(size_t size, Allocator & a) noexcept
{
	return a.Alloc(size);
}

void * operator new(size_t size, std::align_val_t alignment, Allocator & a) noexcept
{
	return a.Alloc(size, (size_t)alignment);
}

void * operator new[](size_t size, Allocator & a) noexcept
{
	return a.Alloc(size);
}

void * operator new[](size_t size, std::align_val_t alignment, Allocator & a) noexcept
{
	return a.Alloc(size, (size_t)alignment);
}
