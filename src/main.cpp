#define DEBUG

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <pointers.h>
#include <collections/TreeDictionary.h>

using namespace std;


char mem[1024];
Allocator a(mem, 1024);

class Test
{
static size_t maxId;
public:
	size_t id;
	size_t dummy1;
	
	Test() {
		id = maxId+1;
		maxId++;
		printf("Test %zu создан\n", id);
	}
	
	~Test()
	{
		printf("Test %zu умер\n", id);
	}
	
	void f() {
		printf("Test::f()\n");
	}
	
	void* operator new(size_t size)
	{
		return a.alloc(size);
	}
	
	void operator delete(void* ptr) throw()
	{
		a.free(ptr);
	}
};
size_t Test::maxId = 0;

int main()
{
	SharedPtr<Test> a = new Test();
}
