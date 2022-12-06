#pragma once
#include <debug.h>
#include <memory>

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

	T * GetPtr() const
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
