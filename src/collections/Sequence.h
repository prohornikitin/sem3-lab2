#pragma once

#include <functional>
#include <iostream>

template <class T>
class Sequence
{
public:
	virtual Sequence<T> * Copy() const = 0;
	virtual Sequence<T> * EmptyOfTheSameType() const = 0;

	bool friend operator==(const Sequence<T> & a, const Sequence<T> & b)
	{
		if(a.GetLength() != b.GetLength())
		{
			return false;
		}

		for(size_t i = 0; i < a.GetLength(); ++i)
		{
			if(a.Get(i) != b.Get(i))
			{
				return false;
			}
		}
		return true;
	}

	friend std::ostream & operator<<(std::ostream & s, const Sequence<T> & seq)
	{
		s << "{";
		if(seq.GetLength() > 0)
		{
			s << seq[0];
			for(size_t i = 1; i < seq.GetLength(); ++i)
			{
				s << ", " << seq[i];
			}
		}
		s << "}";
		return s;
	}

	T operator[](size_t index) const
	{
		return Get(index);
	}

	virtual T & operator[](size_t index) = 0;
	virtual T Get(size_t index) const = 0;
	virtual void Set(size_t index, T value)
	{
		(*this)[index] = value;
	}

	T GetFirst() const
	{
		Get(0);
	}

	T GetLast() const
	{
		Get(GetLength() - 1);
	}

	virtual Sequence<T> * GetSubsequence(size_t startIndex, size_t endIndex) const = 0;

	virtual size_t GetLength() const = 0;

	void Append(T item)
	{
		InsertAt(item, GetLength());
	}

	void Prepend(T item)
	{
		InsertAt(item, 0);
	}

	virtual void InsertAt(T item, size_t index) = 0;
	virtual void RemoveAt(size_t index) = 0;

	Sequence<T> * Concat(Sequence<T> * other) const
	{
		Sequence<T> * result = Copy();
		for(size_t i = 0; i < other->GetLength(); ++i)
		{
			result->Append(other->Get(i));
		}
		return result;
	}

	Sequence<T> * Map(std::function<T(const T &)> f) const
	{
		Sequence<T> * sequence = EmptyOfTheSameType();
		for(size_t i = 0; i < GetLength(); ++i)
		{
			sequence->Append(f(Get(i)));
		}
		return sequence;
	}

	Sequence<T> * Where(std::function<bool(const T &)> f) const
	{
		Sequence<T> * sequence = EmptyOfTheSameType();
		for(size_t i = 0; i < GetLength(); ++i)
		{
			T buf = Get(i);
			if(f(buf))
			{
				sequence->Append(buf);
			}
		}
		return sequence;
	}

	T Reduce(std::function<T(const T & val, const T & acc)> f, const T & initial)
	{
		T val = initial;
		for(size_t i = 0; i < GetLength(); ++i)
		{
			val = f(Get(i), val);
		}
		return val;
	}

	virtual ~Sequence() {}
};
