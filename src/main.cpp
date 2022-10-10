#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <algorithm>

using namespace std;


class Allocator
{
public:
	Allocator(void* memory, size_t size);

	void* alloc(size_t size, size_t alignment);
	void free(void* ptr);
private:
	char* memory = nullptr;
	size_t size;
	bool initialized = false;

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
	printf("Allocator()\n");
	Zone* zone = (Zone*)align((char*)memory, alignof(Zone));
	zone->size = size;
	zone->isFree = true;
	this->memory = align((char*)memory, alignof(Zone));
	this->size = size - sizeof(Zone);
	initialized = true;
}

void* Allocator::alloc(size_t size, size_t alignment)
{
	if(!initialized)
	{
		return malloc(size);
	}
//	printf("this->size=%zu, size=%zu, alignment=%zu\n", this->size, size, alignment);
	char* i = memory;
	while(i < memory + this->size)
	{
		Zone* zone = (Zone*)i;
//		printf("zone:{size:%ld,%d}\n", zone->size, zone->isFree);
		i += sizeof(Zone);
		if(!zone->isFree)
		{
			i += zone->size;
			continue;
		}
		char* aligned = align(i, alignment);
//		printf("unaligned=%p, aligned=%p\n", (void*)i, (void*)aligned);
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
			newZone->size = zone->size - sizeof(Zone) - (aligned + size - i);
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
//		printf("zone:{size:%ld,%d}\n", zone->size, zone->isFree);
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

char mem[1024];
Allocator a(mem, 1024);

void* operator new(size_t count)
{
	void* res = a.alloc(count, min(count, (size_t)__STDCPP_DEFAULT_NEW_ALIGNMENT__));
	if(res == nullptr) {
		throw bad_alloc();
	}
	return res;
}
/*
void operator delete(void* ptr)
{
	a.free(ptr);
}*/

void* operator new[](size_t count) noexcept
{
	void* res = a.alloc(count, min(count, (size_t)__STDCPP_DEFAULT_NEW_ALIGNMENT__));
	if(res == nullptr) {
		throw bad_alloc();
	}
	return res;
}

void operator delete[](void* ptr)
{
	a.free(ptr);
}

void* operator new(size_t count, std::align_val_t alignment) noexcept
{
	void* res = a.alloc(count, (size_t)alignment);
	if(res == nullptr) {
		throw bad_alloc();
	}
	return res;
}

void operator delete(void* ptr, std::align_val_t _)
{
	a.free(ptr);
}

void* operator new[](size_t count, std::align_val_t alignment)
{
	void* res = a.alloc(count, (size_t)alignment);
	if(res == nullptr) {
		throw bad_alloc();
	}
	return res;
}

void operator delete[](void* ptr, std::align_val_t _)
{
	a.free(ptr);
}
int main()
{
	printf("mem=%p\n", (void*)mem);
/*	auto* a = new int();
	delete a;*/
}
