#include <allocator/allocator.h>

std::ostream & operator<<(std::ostream & out, const Buffer & buffer);

static uintptr_t Align(uintptr_t ptr, size_t alignment)
{
	return ptr + (((uintptr_t)ptr) & (alignment - 1));
}

Allocator::Allocator(void * memory, size_t size) :
	buffer(memory, size)
{}

void * Allocator::Alloc(size_t size, size_t alignment)
{
	uintptr_t addr;
	auto maybeIndex = buffer.FindZone([&, alignment](Buffer::Zone z, uintptr_t startAddr) {
		size_t alignOffset = (Align(startAddr, alignment) - startAddr);
		if(!z.isFree || z.size <= alignOffset) {
			return false;
		}
		size_t sizeAfterAlign = z.size - alignOffset;
		if(sizeAfterAlign < size)
		{
			return false;
		}
		addr = startAddr;
		return true;
	});

	if(!maybeIndex)
	{
		return nullptr;
	}

	size_t zoneI = *maybeIndex;
	size_t requiredSize = size + (Align(addr, alignment) - addr);
	buffer.SplitFreeZoneIntoTwoIfRemain(zoneI, requiredSize);
	buffer.MarkOccupied(zoneI);
	return (void *)Align(addr, alignment);
}

void * Allocator::Alloc(size_t size)
{
	return Alloc(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
}

void Allocator::Free(void * ptr)
{
	size_t addr = (uintptr_t)ptr;
	auto maybeIndex = buffer.FindZone([=](Buffer::Zone z, uintptr_t startAddr) {
		return (startAddr <= addr && addr < (startAddr + z.size));
	});
	if(!maybeIndex)
	{
		throw std::invalid_argument("Cannot free ptr");
	}

	size_t zoneI = *maybeIndex;
	buffer.MarkFree(zoneI);
	buffer.MergeFreeZones(zoneI);
}

const Buffer & Allocator::GetBuffer()
{
	return buffer;
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
