#pragma once
#include <collections/ArraySequence.h>
#include <collections/TreeDictionary.h>
#include <debug.h>

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
		id = GetNextId();
		Init(nullptr);
	}

	SharedPtr(T * ptr, Deleter deleter = defaultDeleter)
	{
		this->deleter = deleter;
		id = GetNextId();
		Init(ptr);
	}

	SharedPtr(const SharedPtr<T> & ptr)
	{
		deleter = ptr.deleter;
		id = ptr.id;
		IncRefCount();
	}

	SharedPtr & operator=(const SharedPtr & ptr)
	{
		deleter = ptr.deleter;
		if(ptr.id == id)
		{
			IncRefCount();
			return *this;
		}

		if(GetRefCount() == 1)
		{
			DelPtr();
			FreeId();
		}
		else
		{
			DecRefCount();
		}
		id = ptr.id;
		IncRefCount();
		return *this;
	}

	SharedPtr & operator=(T * ptr)
	{
		if(GetRefCount() == 1)
		{
			DelPtr();
			SetPtr(ptr);
		}
		else
		{
			DecRefCount();
			id = GetNextId();
			Init(ptr);
		}
		return *this;
	}

	~SharedPtr()
	{
		DecRefCount();
		if(GetRefCount() == 0)
		{
			DelPtr();
			FreeId();
		}
	}

	T * GetPtr() const
	{
		return (T *)_sharedDataById[id].ptr;
	}

	T & operator*()
	{
		return *GetPtr();
	}

	T * operator->()
	{
		return GetPtr();
	}

	operator bool() const
	{
		return GetPtr() != nullptr;
	}

private:
	size_t id;
	Deleter deleter;

	size_t GetNextId() const
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

	void FreeId()
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

	void DelPtr()
	{
		deleter((T *)_sharedDataById[id].ptr);
	}

	void SetPtr(T * ptr)
	{
		_sharedDataById[id].ptr = (void *)ptr;
	}

	size_t GetRefCount() const
	{
		return _sharedDataById[id].refs;
	}

	void IncRefCount()
	{
		_sharedDataById[id].refs++;
	}

	void DecRefCount()
	{
		_sharedDataById[id].refs--;
	}

	void Init(T * ptr)
	{
		_sharedDataById.Add(id, {.ptr = (void *)ptr, .refs = 1});
	}
};
