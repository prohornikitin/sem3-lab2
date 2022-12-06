#pragma once
#include <collections/ArraySequence.h>

#include <functional>
#include <optional>

class Buffer
{
public:
	class Zone
	{
	public:
		Zone();
		Zone(size_t size, bool isFree = true);

		size_t size = 0;
		bool isFree = false;
	};

	Buffer(void * memory, size_t size);

	void MarkFree(size_t i);
	void MarkOccupied(size_t i);

	std::optional<size_t> FindZone(std::function<bool(Zone z, uintptr_t addr)> func) const;
	void IterateThroughZones(std::function<void(Zone z, uintptr_t addr)> func) const;

	void SplitFreeZoneIntoTwoIfRemain(size_t i, size_t sizeOfFirst);
	void MergeFreeZones(size_t firstZoneIndex);

private:
	char * memory = nullptr;
	size_t size = 0;

	ArraySequence<Zone> zones;
};
