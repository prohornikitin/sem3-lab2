#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <new>
#include <collections/TreeDictionary.h>
#include <pointers.h>

using namespace std;


char mem[1024];
Allocator a(mem, 1024);


class Obj
{
static size_t maxId;
public:
	size_t id;
	
	Obj() {
		id = maxId+1;
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
	
	
	void* operator new(size_t size) noexcept
	{
		return a.Alloc(size);
	}
	
	void* operator new(size_t size, std::align_val_t alignment) noexcept
	{
		return a.Alloc(size, (size_t)alignment);
	}
	
	void* operator new[](size_t size) noexcept
	{
		return a.Alloc(size);
	}
	
	void* operator new[](size_t size, std::align_val_t alignment) noexcept
	{
		return a.Alloc(size, (size_t)alignment);
	}
	
	void operator delete(void* ptr) noexcept
	{
		a.Free(ptr);
	}
	
	void operator delete[](void* ptr) noexcept
	{
		a.Free(ptr);
	}
	
	void operator delete(void* ptr, std::align_val_t alignment) noexcept
	{
		a.Free(ptr);
	}
	
	void operator delete[](void* ptr, std::align_val_t alignment) noexcept
	{
		a.Free(ptr);
	}
};
size_t Obj::maxId = 0;

class HugeObj : public Obj
{
	size_t a[100];
};

int main()
{
	printf("\n");
	
	SharedPtr a = new Obj();
	a->F();
	printf("\n");
	
	SharedPtr b = new HugeObj();
	printf("\n");
	
	Obj* array = new Obj[10];
	printf("\n");
	delete[] array;
	printf("\n");
	
	Obj* tooLargeArray = new Obj[100];
	printf("\n");
}
