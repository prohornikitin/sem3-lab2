#pragma once
#include <stddef.h>
#include <stdint.h>
#include <functional>

class Allocator
{
public:
	Allocator(void* memory, size_t size);

	void* Alloc(size_t size, size_t alignment);
	void* Alloc(size_t size);
	void* AllocArray(size_t size);
	void Free(void* ptr);
	
	template<class T>
	std::function<void(T*)> GetDeleter() {
		return [&](T* o){
			o->~T();
			Free(o);
		};
	}
private:
	char* memory = nullptr;
	size_t size;

	struct Zone {
		uint64_t size;
		bool isFree;
	};

	void MergeFreeZones(Zone* first);
};


void* operator new(size_t size, Allocator & a) noexcept;
void* operator new(size_t size, std::align_val_t alignment, Allocator & a) noexcept;
void* operator new[](size_t size, Allocator & a) noexcept;
void* operator new[](size_t size, std::align_val_t alignment, Allocator & a) noexcept;
