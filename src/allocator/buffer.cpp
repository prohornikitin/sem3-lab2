#include <allocator/buffer.h>

#include <exception>

Buffer::Zone::Zone() {}
Buffer::Zone::Zone(size_t size, bool isFree) :
	size(size),
	isFree(isFree)
{}

Buffer::Buffer(void * memory, size_t size)
{
	this->memory = (char *)memory;
	this->size = size;
	zones.Append(Zone(size));
}

void Buffer::MarkFree(size_t i)
{
	zones[i].isFree = true;
}

void Buffer::MarkOccupied(size_t i)
{
	zones[i].isFree = false;
}

std::optional<size_t> Buffer::FindZone(std::function<bool(Zone z, uintptr_t addr)> func) const
{
	uintptr_t addr = (uintptr_t)memory;
	for(size_t i = 0; i < zones.GetLength(); ++i)
	{
		if(func(zones.Get(i), (uintptr_t)addr))
		{
			return i;
		}
		addr += zones.Get(i).size;
	}
	return {};
}

void Buffer::IterateThroughZones(std::function<void(Zone z, uintptr_t addr)> func) const
{
	uintptr_t addr = (uintptr_t)memory;
	for(size_t i = 0; i < zones.GetLength(); ++i)
	{
		func(zones.Get(i), (uintptr_t)addr);
		addr += zones.Get(i).size;
	}
}

void Buffer::SplitFreeZoneIntoTwoIfRemain(size_t i, size_t sizeOfFirst)
{
	if(!zones[i].isFree)
	{
		throw std::invalid_argument("To split zone into two it must be free");
	}
	if(sizeOfFirst == 0)
	{
		throw std::invalid_argument("Size of zone cannot be 0");
	}

	size_t newZoneSize = zones[i].size - sizeOfFirst;
	if(newZoneSize > 0)
	{
		Zone newZone(newZoneSize);
		zones.InsertAt(newZone, i + 1);
		zones[i].size = sizeOfFirst;
	}
}

void Buffer::MergeFreeZones(size_t firstZoneIndex)
{
	for(size_t i = firstZoneIndex + 1; i < zones.GetLength(); ++i)
	{
		if(!zones[i].isFree)
		{
			break;
		}
		zones[firstZoneIndex].size += zones[i].size;
		zones.RemoveAt(i);
	}
}
