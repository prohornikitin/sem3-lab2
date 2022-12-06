#include <collection/ArraySequence.h>

class Buffer
{
public:
	Buffer(char * memory, size_t size)
	{
		this->memory = memory;
		this->size = size;
	}

	Zone & operator[](size_t i)
	{
		return zones[i];
	}

	void RemoveZone(size_t i)
	{
		// TODO
	}

	void SplitZoneIntoTwo(size_t i)
	{
		// TODO
	}

	struct Zone
	{
		size_t size;
		bool isFree;
	};

private:
	char * memory = nullptr;
	size_t size = 0;

	ArraySequence<Zone> zones;
};
