#pragma once
#include <allocator/buffer.h>
#include <stddef.h>
#include <stdint.h>

#include <functional>

class Allocator
{
public:
	Allocator(void * memory, size_t size);

	void * Alloc(size_t size, size_t alignment);
	void * Alloc(size_t size);
	void Free(void * ptr);

	template <class T>
	std::function<void(T *)> GetDeleter()
	{
		return [&](T * o) {
			o->~T();
			Free(o);
		};
	}

	const Buffer & GetBuffer();

private:
	Buffer buffer;
};

void * operator new(size_t size, Allocator & a) noexcept;
void * operator new(size_t size, std::align_val_t alignment, Allocator & a) noexcept;
void * operator new[](size_t size, Allocator & a) noexcept;
void * operator new[](size_t size, std::align_val_t alignment, Allocator & a) noexcept;
