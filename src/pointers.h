#pragma once
#include <allocator.h>
#include <collections/ArraySequence.h>
#include <collections/TreeDictionary.h>
#include <debug.h>

#include <memory>
#include <vector>

struct _Data
{
	void * ptr = nullptr;
	size_t refs = 0;
};

struct _IdRange
{
	size_t min;
	size_t max;
};

TreeDictionary<size_t, _Data> _sharedDataById;
ArraySequence<_IdRange> _acquiredIds;

template <class T>
class SharedPtr
{
private:
	using Deleter = std::function<void(T *)>;
	static auto constexpr defaultDeleter = [](T * t) { delete t; };

public:
	SharedPtr(Deleter deleter = defaultDeleter)
	{
		this->deleter = deleter;
		id = getNextId();
		init(nullptr);
	}

	SharedPtr(T * ptr, Deleter deleter = defaultDeleter)
	{
		this->deleter = deleter;
		id = getNextId();
		init(ptr);
	}

	SharedPtr(const SharedPtr<T> & ptr)
	{
		deleter = ptr.deleter;
		id = ptr.id;
		incRefCount();
	}

	SharedPtr & operator=(const SharedPtr & ptr)
	{
		deleter = ptr.deleter;
		if(ptr.id == id)
		{
			incRefCount();
			return *this;
		}

		if(getRefCount() == 1)
		{
			delPtr();
			freeId();
		}
		else
		{
			decRefCount();
		}
		id = ptr.id;
		incRefCount();
		return *this;
	}

	SharedPtr & operator=(T * ptr)
	{
		if(getRefCount() == 1)
		{
			delPtr();
			setPtr(ptr);
		}
		else
		{
			decRefCount();
			id = getNextId();
			init(ptr);
		}
		return *this;
	}

	~SharedPtr()
	{
		decRefCount();
		if(getRefCount() == 0)
		{
			delPtr();
			freeId();
		}
	}

	T * getPtr() const
	{
		return (T *)_sharedDataById[id].ptr;
	}

	T & operator*()
	{
		return *getPtr();
	}

	T * operator->()
	{
		return getPtr();
	}

	operator bool() const
	{
		return getPtr() != nullptr;
	}

private:
	size_t id;
	Deleter deleter;

	size_t getNextId() const
	{
		if(_acquiredIds.GetLength() == 0)
		{
			_acquiredIds.Append({.min = 0, .max = 0});
			return 0;
		}

		if(_acquiredIds[0].min > 0)
		{
			_acquiredIds[0].min--;
			return _acquiredIds[0].min;
		}

		for(size_t i = 0; i < (_acquiredIds.GetLength() - 1); ++i)
		{
			if(_acquiredIds[i].max + 1 != _acquiredIds[i + 1].min)
			{
				_acquiredIds[i].max++;
				return _acquiredIds[i].max;
			}
		}

		_acquiredIds[_acquiredIds.GetLength() - 1].max++;
		return _acquiredIds[_acquiredIds.GetLength() - 1].max;
	}

	void freeId()
	{
		_sharedDataById.Remove(id);
		for(size_t i = 0; i < _acquiredIds.GetLength(); i++)
		{
			_IdRange & range = _acquiredIds[i];
			if(range.min <= id && id <= range.max)
			{
				if(range.min == range.max)
				{
					_acquiredIds.RemoveAt(i);
					return;
				}

				if(range.min == id)
				{
					range.min++;
					return;
				}

				if(range.max == id)
				{
					range.max--;
					return;
				}

				_acquiredIds.InsertAt({.min = id + 1, .max = range.max}, i);
				range.max = id;
			}
		}
	}

	void delPtr()
	{
		deleter((T *)_sharedDataById[id].ptr);
	}

	void setPtr(T * ptr)
	{
		_sharedDataById[id].ptr = (void *)ptr;
	}

	size_t getRefCount() const
	{
		return _sharedDataById[id].refs;
	}

	void incRefCount()
	{
		_sharedDataById[id].refs++;
	}

	void decRefCount()
	{
		_sharedDataById[id].refs--;
	}

	void init(T * ptr)
	{
		_sharedDataById.Add(id, {.ptr = (void *)ptr, .refs = 1});
	}
};

template <class T>
class UniquePtr
{
private:
	using Deleter = std::function<void(T *)>;
	static constexpr auto defaultDeleter = [](T * t) { delete t; };

public:
	UniquePtr() : deleter(defaultDeleter) {}

	UniquePtr(T * ptr, Deleter deleter = defaultDeleter)
	{
		this->deleter = deleter;
		raw = ptr;
	}

	UniquePtr(UniquePtr<T> & ptr) = delete;
	UniquePtr(const UniquePtr<T> & ptr) = delete;

	UniquePtr(UniquePtr<T> && ptr)
	{
		raw = ptr.raw;
		ptr.raw = nullptr;
		deleter = ptr.deleter;
	}

	UniquePtr<T> & operator=(T * newRaw)
	{
		deleter(raw);
		raw = newRaw;
		return *this;
	}

	UniquePtr<T> & operator=(const UniquePtr<T> & ptr) = delete;

	UniquePtr<T> & operator=(UniquePtr<T> && ptr)
	{
		deleter(raw);
		raw = ptr.raw;
		ptr.raw = nullptr;
		deleter = ptr.deleter;
		return *this;
	}

	~UniquePtr()
	{
		deleter(raw);
	}

	T * getPtr() const
	{
		return raw;
	}

	operator bool() const
	{
		return raw != nullptr;
	}

	T & operator*()
	{
		return *raw;
	}

	T * operator->()
	{
		return raw;
	}

private:
	Deleter deleter;
	T * raw = nullptr;
};
