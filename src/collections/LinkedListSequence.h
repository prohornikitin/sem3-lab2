#pragma once

#include <initializer_list>

#include "LinkedList.h"
#include "Sequence.h"

template <class T>
class LinkedListSequence : public Sequence<T>
{
public:
	LinkedListSequence() :
		list(new LinkedList<T>)
	{}

	LinkedListSequence(std::initializer_list<T> init) :
		list(new LinkedList<T>(init))
	{}

	LinkedListSequence(const LinkedListSequence<T> & sequence) :
		list(new LinkedList<T>(*sequence.list))
	{}

	virtual Sequence<T> * Copy() const override
	{
		return new LinkedListSequence<T>(*this);
	}

	virtual Sequence<T> * EmptyOfTheSameType() const override
	{
		return new LinkedListSequence<T>();
	}

	virtual T Get(size_t index) const override
	{
		return list->Get(index);
	}

	virtual T & operator[](size_t index) override
	{
		return list->GetRef(index);
	}

	virtual void RemoveAt(size_t index) override
	{
		list->RemoveAt(index);
	}

	virtual Sequence<T> * GetSubsequence(size_t startIndex, size_t endIndex) const override
	{
		return new LinkedListSequence<T>(list->GetSubList(startIndex, endIndex));
	}

	virtual size_t GetLength() const override
	{
		return list->GetLength();
	}

	virtual void InsertAt(T item, size_t index) override
	{
		return list->InsertAt(item, index);
	}

	virtual ~LinkedListSequence()
	{
		delete list;
	}

private:
	LinkedList<T> * list;

	LinkedListSequence(LinkedList<T> * list) :
		list(list)
	{}
};
