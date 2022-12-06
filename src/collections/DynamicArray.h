#pragma once

#include <algorithm>
#include <initializer_list>
#include <stdexcept>

template <class T>
class DynamicArray
{
public:
	DynamicArray(std::initializer_list<T> init)
	{
		Resize(init.size());

		size_t i = 0;
		for(auto k : init)
		{
			Set(i, k);
			i++;
		}
	}

	DynamicArray(T * items, size_t count)
	{
		Resize(count);
		this->size = count;
		std::copy(items, items + count, array);
	}

	DynamicArray(size_t size)
	{
		Resize(size);
	}

	DynamicArray(const DynamicArray<T> & source)
	{
		Resize(source.size);
		this->size = source.size;
		std::copy(source.array, source.array + size, array);
	}

	T & operator[](size_t index)
	{
		if(index >= size)
		{
			throw std::out_of_range("Invalid index " + std::to_string(index));
		}
		return array[index];
	}

	size_t GetSize() const
	{
		return size;
	}

	T Get(size_t index) const
	{
		if(index >= size)
		{
			throw std::out_of_range("Invalid index " + std::to_string(index));
		}
		return array[index];
	}

	void Set(size_t index, T value)
	{
		(*this)[index] = value;
	}

	void Resize(size_t newSize)
	{
		T * buff = array;
		array = new T[newSize];
		std::copy(buff, buff + std::min(size, newSize), array);
		delete[] buff;
		size = newSize;
	}

	~DynamicArray()
	{
		delete[] array;
	}

private:
	T * array = nullptr;
	size_t size = 0;
};
