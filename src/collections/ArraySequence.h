#pragma once

#include <initializer_list>

#include "DynamicArray.h"
#include "Sequence.h"

template <class T>
class ArraySequence : public Sequence<T>
{
public:
	ArraySequence() :
		array(new DynamicArray<T>(0))
	{}

	ArraySequence(std::initializer_list<T> init) :
		array(new DynamicArray<T>(init))
	{}

	ArraySequence(size_t count) :
		array(new DynamicArray<T>(count))
	{}

	ArraySequence(const ArraySequence<T> & sequence) :
		array(new DynamicArray<T>(*sequence.array))
	{}

	virtual Sequence<T> * Copy() const override
	{
		return new ArraySequence<T>(*this);
	}

	virtual Sequence<T> * EmptyOfTheSameType() const override
	{
		return new ArraySequence<T>();
	}

	virtual T Get(size_t index) const override
	{
		return array->Get(index);
	}

	virtual T & operator[](size_t index) override
	{
		return (*array)[index];
	}

	virtual Sequence<T> * GetSubsequence(size_t startIndex, size_t endIndex) const override
	{
		DynamicArray<T> * subArray = new DynamicArray<T>(endIndex - startIndex + 1);
		for(size_t i = startIndex; i <= endIndex; ++i)
		{
			subArray->Set(i - startIndex, Get(i));
		}
		return new ArraySequence<T>(subArray);
	}

	virtual size_t GetLength() const override
	{
		return array->GetSize();
	}

	virtual void InsertAt(T item, size_t index) override
	{
		if(index <= array->GetSize())
		{
			array->Resize(array->GetSize() + 1);
			for(size_t i = index; i < array->GetSize() - 1; ++i)
			{
				(*array)[i + 1] = (*array)[i];
			}
			(*array)[index] = item;
		}
		else
		{
			array->Resize(index + 1);
			(*array)[index] = item;
		}
	}

	virtual void RemoveAt(size_t index) override
	{
		for(size_t i = index + 1; i < GetLength(); ++i)
		{
			(*array)[i - 1] = (*array)[i];
		}
		array->Resize(array->GetSize() - 1);
	}

	virtual ~ArraySequence()
	{
		delete array;
	}

private:
	DynamicArray<T> * array;

	ArraySequence(DynamicArray<T> * array) :
		array(array)
	{}
};
