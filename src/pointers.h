#pragma once
#include <allocator.h>
#include <map>
#include <vector>
#include <debug.h>

struct _Data
{
	void* ptr = nullptr;
	size_t refs = 0;
};

struct _IdRange
{
	size_t min;
	size_t max;
};

std::map<size_t, _Data> _sharedDataById;
std::vector<_IdRange> _acquiredIds;

template <class T>
class SharedPtr
{
public:
	SharedPtr()
	{
		DPRINTF("SharedPtr()");
	}
	
	SharedPtr(T* ptr)
	{
		id = getNextId();
		init(ptr);
		DPRINTF("SharedPtr(%p)", ptr);
	}

	SharedPtr & operator=(const SharedPtr & ptr)
	{
		DPRINTF("SharedPtr = SharedPtr(%p)", getPtr());
		if(ptr.id == id) {
			return *this;
		}
		if(getRefCount() == 1)
		{
			delPtr();
			setPtr(ptr.getPtr());
		}
		else
		{
			decRefCount();
			id = ptr.id;
			incRefCount();
		}
		return *this;
	}

	SharedPtr & operator=(T* ptr) const
	{
		DPRINTF("SharedPtr = %p", ptr);
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

	SharedPtr(const SharedPtr<T> & ptr)
	{
		DPRINTF("SharedPtr(%p) copy", ptr.getPtr());
		id = ptr.id;
		incRefCount();
	}

	~SharedPtr()
	{
		DPRINTF("~SharedPtr(%p)", getPtr());
		decRefCount();
		if(getRefCount() == 0) {
			delPtr();
			freeId();
		}
	}
	
	T& operator*()
	{
		return *getPtr();
	}
	
	T* operator->()
	{
		return getPtr();
	}
	
private:
	

	size_t getNextId()
	{
		if(_acquiredIds.size() == 0)
		{
			_acquiredIds.push_back({.min=0, .max=0});
			return 0;
		}
		
		if(_acquiredIds.front().min > 0)
		{
			_acquiredIds.front().min--;
			return _acquiredIds[0].min;
		}
		
		for(size_t i = 0; i < (_acquiredIds.size()-1); ++i)
		{
			if(_acquiredIds[i].max + 1 != _acquiredIds[i].min) {
				_acquiredIds[i].max++;
				return _acquiredIds[i].max;
			}
		}
		
		_acquiredIds.back().max++;
		return _acquiredIds.back().max;
	}

	void freeId()
	{
		for(auto i = _acquiredIds.begin(); i != _acquiredIds.end(); i++)
		{
			if(i->min <= id && id <= i->max) {
				if(i->min == i->max)
				{
					_acquiredIds.erase(i);
					return;
				}
				
				if(i->min == id)
				{
					i->min++;
					return;
				}
				
				if(i->max == id)
				{
					i->max--;
					return;
				}
				
				
				_acquiredIds.insert(i, {.min = id+1, .max = i->max});
				i->max = id;
			}
		}
	}
	
	void delPtr()
	{
		delete ((T*)_sharedDataById[id].ptr);
	}
	
	void setPtr(T* ptr)
	{
		_sharedDataById[id].ptr = (void*)ptr;
	}
	
	T* getPtr() const
	{
		return (T*)_sharedDataById[id].ptr;
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
	
	void init(T* ptr)
	{
		_sharedDataById[id].ptr = (void*)ptr;
		_sharedDataById[id].refs = 1;
	}
	
	size_t id;
};
