#include <gtest/gtest.h>
#include <allocator.h>

static char data[1024];


#define ASSERT_ALIGNMENT(ptr, alignment) \
	ASSERT_EQ(((uintptr_t)ptr) & (alignment-1), 0);

TEST(Allocator, allocAlign16)
{
	Allocator al(data, 1024);
	void* ptr = al.alloc(5, 16);
	ASSERT_ALIGNMENT(ptr, 16);
}


TEST(Allocator, allocAlign3)
{
	Allocator al(data, 1024);
	void* ptr = al.alloc(5, 3);
	ASSERT_ALIGNMENT(ptr, 3);
}


TEST(Allocator, allocSize7)
{
	Allocator al(data, 1024);
	void* ptr = al.alloc(7);
	ASSERT_ALIGNMENT(ptr, 16);
}


TEST(Allocator, allocAllTheMemoryFragments)
{
	Allocator al(data, 1024);
	al.alloc(48);
	al.alloc(90);
	for(size_t i=0; i < 256; ++i)
	{
		al.alloc(4);
	}
	ASSERT_EQ(al.alloc(1), nullptr);
}


TEST(Allocator, allocWithNoMemory)
{
	Allocator al(data, 0);
	ASSERT_EQ(al.alloc(1), nullptr);
}
