#include <gtest/gtest.h>
#include <pointers.h>

class Obj
{
public:
	static size_t count;

	Obj()
	{
		count++;
	}

	Obj(const Obj & obj)
	{
		count++;
	}

	bool True()
	{
		return true;
	}

	~Obj()
	{
		count--;
	}
};

size_t Obj::count = 0;

TEST(SharedPtr, simpleWrap)
{
	{
		SharedPtr shared = new Obj();
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, getPtr)
{
	{
		Obj * ptr = new Obj();
		ASSERT_EQ(Obj::count, 1);
		SharedPtr shared = ptr;
		ASSERT_EQ(ptr, shared.GetPtr());
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, memberAccess)
{
	{
		SharedPtr shared = new Obj();
		ASSERT_TRUE(shared->True());
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, dereference)
{
	{
		SharedPtr shared = new Obj();
		ASSERT_EQ(Obj::count, 1);
		Obj valueCopy = *shared;
		ASSERT_EQ(Obj::count, 2);
		ASSERT_TRUE(valueCopy.True());
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, copy)
{
	{
		SharedPtr shared = new Obj();
		ASSERT_EQ(Obj::count, 1);
		{
			SharedPtr sharedCopy = shared;
			ASSERT_EQ(Obj::count, 1);
		}
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, assignPtrToEmpty)
{
	{
		SharedPtr<Obj> shared;
		shared = new Obj();
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, assignPtrToAnother)
{
	{
		SharedPtr shared = new Obj();
		ASSERT_EQ(Obj::count, 1);
		Obj * ptr = new Obj();
		ASSERT_EQ(Obj::count, 2);
		shared = ptr;
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, assignSharedToEmpty)
{
	{
		SharedPtr shared1 = new Obj();
		ASSERT_EQ(Obj::count, 1);
		SharedPtr<Obj> shared2;
		shared2 = shared1;
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(SharedPtr, assignSharedToAnother)
{
	{
		SharedPtr shared1 = new Obj();
		ASSERT_EQ(Obj::count, 1);
		SharedPtr shared2 = new Obj();
		ASSERT_EQ(Obj::count, 2);
		shared2 = shared1;
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, simpleWrap)
{
	{
		UniquePtr unique = new Obj();
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, move)
{
	{
		UniquePtr unique = new Obj();
		UniquePtr unique2 = std::move(unique);
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, assignPtrToEmpty)
{
	{
		UniquePtr<Obj> unique;
		unique = new Obj();
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, assignPtrToAnother)
{
	{
		UniquePtr unique = new Obj();
		ASSERT_EQ(Obj::count, 1);
		unique = new Obj();
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, getPtr)
{
	{
		Obj * ptr = new Obj();
		ASSERT_EQ(Obj::count, 1);
		UniquePtr unique = ptr;
		ASSERT_EQ(ptr, unique.GetPtr());
		ASSERT_EQ(Obj::count, 1);
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, memberAccess)
{
	{
		SharedPtr unique = new Obj();
		ASSERT_TRUE(unique->True());
	}
	ASSERT_EQ(Obj::count, 0);
}

TEST(UniquePtr, dereference)
{
	{
		SharedPtr unique = new Obj();
		ASSERT_EQ(Obj::count, 1);
		Obj valueCopy = *unique;
		ASSERT_EQ(Obj::count, 2);
		ASSERT_TRUE(valueCopy.True());
	}
	ASSERT_EQ(Obj::count, 0);
}
