#include <collections/ArraySequence.h>
#include <collections/LinkedListSequence.h>
#include <gtest/gtest.h>

#include <exception>
#include <initializer_list>

using std::initializer_list;
using std::unique_ptr;

TEST(LinkedListSequenceTest, InitFromInitalizerList)
{
	initializer_list l = {1, 2, 3};
	LinkedListSequence<int> seq = l;
	ASSERT_EQ(seq.GetLength(), l.size());

	size_t i = 0;
	for(auto v : l)
	{
		ASSERT_EQ(seq[i], v);
		i++;
	}
}

TEST(LinkedListSequenceTest, Get_Exception)
{
	LinkedListSequence<int> seq = {1, 2, 3};
	EXPECT_THROW(seq.Get(4), std::out_of_range);
}

TEST(LinkedListSequenceTest, Prepend)
{
	LinkedListSequence<int> seq = {1, 2, 3};
	seq.Prepend(0);
	ASSERT_EQ(seq[0], 0);
}

TEST(LinkedListSequenceTest, Append)
{
	LinkedListSequence<int> seq = {1, 2, 3};
	seq.Append(4);
	ASSERT_EQ(seq[3], 4);
}

TEST(LinkedListSequenceTest, Insert_AtRandom)
{
	LinkedListSequence<int> seq = {1, 2, 3};
	seq.InsertAt(4, 2);
	ASSERT_EQ(seq[2], 4);
}

TEST(LinkedListSequenceTest, Insert_BeyongTheEnd)
{
	LinkedListSequence<int> seq = {1, 2, 3};
	seq.InsertAt(10, 10);
	ASSERT_EQ(seq.GetLength(), 11);
	ASSERT_EQ(seq[10], 10);
}

TEST(LinkedListSequenceTest, GetSubsequence)
{
	LinkedListSequence<int> seq = {1, 2, 3, 4, 5};
	auto subseq = unique_ptr<Sequence<int>>(seq.GetSubsequence(2, 4));
	LinkedListSequence<int> expected = {3, 4, 5};
	ASSERT_EQ(*subseq, expected);
}

TEST(LinkedListSequenceTest, Concat_withArraySequence)
{
	LinkedListSequence<int> seq1 = {1, 2, 3, 4, 5};
	ArraySequence<int> seq2 = {6, 7, 8, 9};
	auto result = unique_ptr<Sequence<int>>(seq1.Concat(&seq2));
	ASSERT_EQ(result->GetLength(), 9);
	for(size_t i = 0; i < seq1.GetLength(); ++i)
	{
		ASSERT_EQ(seq1[i], (*result)[i]);
	}
	for(size_t i = 0; i < seq2.GetLength(); ++i)
	{
		ASSERT_EQ(seq2[i], (*result)[seq1.GetLength() + i]);
	}
}

TEST(LinkedListSequenceTest, Concat_withLinkedListSequence)
{
	LinkedListSequence<int> seq1 = {1, 2, 3, 4, 5};
	LinkedListSequence<int> seq2 = {6, 7, 8, 9};
	auto result = unique_ptr<Sequence<int>>(seq1.Concat(&seq2));
	ASSERT_EQ(result->GetLength(), 9);
	for(size_t i = 0; i < seq1.GetLength(); ++i)
	{
		ASSERT_EQ(seq1[i], (*result)[i]);
	}
	for(size_t i = 0; i < seq2.GetLength(); ++i)
	{
		ASSERT_EQ(seq2[i], (*result)[seq1.GetLength() + i]);
	}
}

TEST(LinkedListSequenceTest, Map)
{
	LinkedListSequence<int> seq = {1, 2, 3, 4, 5};
	auto result = unique_ptr<Sequence<int>>(seq.Map([](int x) { return x + 1; }));
	LinkedListSequence<int> expected = {2, 3, 4, 5, 6};
	ASSERT_EQ(*result, expected);
}

TEST(LinkedListSequenceTest, Where)
{
	LinkedListSequence<int> seq = {1, 2, 3, 4, 5};
	auto result = unique_ptr<Sequence<int>>(seq.Where([](int x) { return x % 2 == 0; }));
	LinkedListSequence<int> expected = {2, 4};
	ASSERT_EQ(*result, expected);
}

TEST(LinkedListSequenceTest, Reduce)
{
	LinkedListSequence<int> seq = {1, 2, 3, 4, 5};
	int result = seq.Reduce([](int val, int acc) { return acc + val; }, 0);
	ASSERT_EQ(result, 1 + 2 + 3 + 4 + 5);
}

TEST(LinkedListSequenceTest, Set)
{
	LinkedListSequence<int> seq = {1, 2, 3, 4, 5};
	seq[2] = 0;
	ASSERT_EQ(seq[2], 0);
}
