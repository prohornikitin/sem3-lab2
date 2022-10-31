#define DEBUG

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <pointers.h>

using namespace std;







char mem[1024];
Allocator a(mem, 1024);

class Test
{
static size_t maxId;
public:
	size_t id;
	Test() {
		id = maxId+1;
		maxId++;
		printf("Test %d создан\n", id);
	}
	
	~Test()
	{
		printf("Test %d умер\n", id);
	}
	
	//~ operator new()
	//~ {
		
	//~ }
};
size_t Test::maxId = 0;

int main()
{
	//~ DPRINTF("mem=%p\n", (void*)mem);
	//~ DPRINTF("%s\n", "alloc");
	//~ void* m1 = a.alloc(sizeof(int), sizeof(int));
	//~ DPRINTF("%s\n", "free");
	//~ a.free(m1);
	
	//~ a.free(a.alloc(7, sizeof(int)));
	//~ a.free(a.alloc(7, sizeof(int)));
	//~ a.free(a.alloc(7, sizeof(int)));
}
