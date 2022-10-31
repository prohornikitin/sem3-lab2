#include <collections/TreeDictionary.h>
#include <gtest/gtest.h>


TEST(TreeDictionary, initFromInitializerList)
{
	TreeDictionary<int, int> dict = {{1,1}, {2,2}};
	
	for(size_t i = 1; i <= 2; ++i)
	{
		auto opt = dict.Get(i);
		ASSERT_TRUE(opt.has_value());
		ASSERT_EQ(opt.value(), i);
	}
}


TEST(TreeDictionary, assignInitializerList)
{
	std::initializer_list<std::pair<int, int>> list = {{1,1}, {2,2}};
	TreeDictionary<int, int> dict = list;
	for(size_t i = 1; i <= 2; ++i)
	{
		auto opt = dict.Get(i);
		ASSERT_TRUE(opt.has_value());
		ASSERT_EQ(opt.value(), i);
	}
}


TEST(TreeDictionary, Equality)
{
	TreeDictionary<int, int> a = {{1,1}, {2,2}};
	TreeDictionary<int, int> b = {{1,1}, {2,2}};
	ASSERT_EQ(a, b);
}


TEST(TreeDictionary, Inequality)
{
	TreeDictionary<int, int> a = {{1,1}, {2,2}};
	TreeDictionary<int, int> b = {{1,2}, {2,3}};
	ASSERT_NE(a, b);
}


TEST(TreeDictionary, GetCount)
{
	TreeDictionary<int, int> a = {{1,1}, {2,2}};
	ASSERT_EQ(a.GetCount(), 2);
}


TEST(TreeDictionary, AddAndGet)
{
	TreeDictionary<int, int> dict;
	dict.Add(1,1);
	auto opt = dict.Get(1);
	ASSERT_TRUE(opt.has_value());
	ASSERT_EQ(opt.value(), 1);
}


TEST(TreeDictionary, GetMissing)
{
	TreeDictionary<int, int> dict = {{2,2}};
	auto opt = dict.Get(1);
	ASSERT_FALSE(opt.has_value());
}


TEST(TreeDictionary, ContainsPresent)
{
	TreeDictionary<int, int> dict = {{1,1}};
	ASSERT_TRUE(dict.Contains(1));
}


TEST(TreeDictionary, ContainsMissing)
{
	TreeDictionary<int, int> dict = {{1,1}};
	ASSERT_FALSE(dict.Contains(2));
}


TEST(TreeDictionary, Remove)
{
	TreeDictionary<int, int> dict = {{1,1}};
	ASSERT_TRUE(dict.Contains(1));
	dict.Remove(1);
	ASSERT_FALSE(dict.Contains(1));
}


TEST(TreeDictionary, Copy)
{
	TreeDictionary<int, int> dict = {{1,1}, {2,2}};
	TreeDictionary<int, int> copy = dict;
	ASSERT_EQ(dict, copy);
}

TEST(TreeDictionary, CopyAssign)
{
	TreeDictionary<int, int> dict = {{1,1}, {2,2}};
	TreeDictionary<int, int> copy;
	copy = dict;
	ASSERT_EQ(dict, copy);
}
