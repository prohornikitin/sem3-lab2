#include <collections/TreeDictionary.h>
#include <pointers.h>
#include <allocator.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <new>

using namespace std;

class Obj
{
	static size_t maxId;

public:
	size_t id;

	Obj()
	{
		id = maxId + 1;
		maxId++;
		printf("Obj %zu создан\n", id);
	}

	~Obj()
	{
		printf("Obj %zu уничтожается\n", id);
	}

	void F()
	{
		printf("Obj %zu F()\n", id);
	}
};

size_t Obj::maxId = 0;

class HugeObj : public Obj
{
	size_t a[100];
};

int main()
{
	char mem[1024];
	Allocator a(mem, 1024);

	printf("\n");
	SharedPtr s1(new(a) Obj(), a.GetDeleter<Obj>());
	s1->F();
	printf("\n");

	SharedPtr s2(new(a) HugeObj(), a.GetDeleter<HugeObj>());
	printf("\n");

	/*	Obj* array = new(a) Obj[10];
		printf("\n");
		delete[] (a) array;
		printf("\n");

		Obj* tooLargeArray = new(a) Obj[100];
		printf("\n");*/
}
