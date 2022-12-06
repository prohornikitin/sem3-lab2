#include <allocator/allocator.h>
#include <allocator/buffer.h>
#include <collections/TreeDictionary.h>
#include <pointers/pointers.h>
#include <ui/commands.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <variant>

using namespace std;

std::ostream & operator<<(std::ostream & out, const Buffer & buffer)
{
	out << "┌───────────────┬────────────┬───────────┐\n"
		   "│    address    │    size    │   state   │\n"
		   "├───────────────┼────────────┼───────────┤\n";
	buffer.IterateThroughZones([&](Buffer::Zone z, uintptr_t addr) {
		out << std::left << "│ " << std::setw(13) << (void *)addr << " │ " << std::setw(10)
			<< z.size << " │ " << std::setw(9) << (z.isFree ? "free" : "occupied") << " │\n";
	});
	out << "└───────────────┴────────────┴───────────┘\n";
	return out;
}

Command GetCommand()
{
	using namespace std;

	while(true)
	{
		cout << ">";
		string line;
		getline(cin, line);

		stringstream ss(line);
		string command;
		ss >> command;

		if(command == "alloc")
		{
			string varName;
			ss >> varName;
			size_t size;
			ss >> size;
			if(ss)
			{
				return AllocCommand(varName, size);
			}
			cout << "'alloc' should have two arguments: 'variable name', 'allocation size'\n";
		}
		else if(command == "free")
		{
			string varName;
			ss >> varName;
			if(ss)
			{
				return FreeCommand(varName);
			}
			cout << "'free' should have one argument: 'variable name'\n";
		}
		else if(command == "exit")
		{
			return ExitCommand();
		}
		else if(command == "print")
		{
			return PrintLayoutCommand();
		}
		else if(command == "help")
		{
			return HelpCommand();
		}
		else
		{
			cout << "command does not exists\n";
		}
	}
}

int main()
{
	using std::holds_alternative;

	char * memory = new char[1024];
	Allocator allocator(memory, 1024);

	TreeDictionary<std::string, void *> vars;

	while(true)
	{
		auto cmd = GetCommand();

		if(holds_alternative<ExitCommand>(cmd))
		{
			break;
		}

		if(holds_alternative<FreeCommand>(cmd))
		{
			auto variable = std::get<FreeCommand>(cmd).variable;
			auto ptr = vars.Get(variable);
			if(ptr)
			{
				allocator.Free(*ptr);
				vars.Remove(variable);
			}
			else
			{
				cout << "No such variable\n";
			}
		}

		if(holds_alternative<AllocCommand>(cmd))
		{
			auto args = std::get<AllocCommand>(cmd);
			if(vars.Contains(args.variable))
			{
				cout << "Variable with this name already exists\n";
			}
			else
			{
				vars.Add(args.variable, allocator.Alloc(args.size));
			}
		}

		if(holds_alternative<PrintLayoutCommand>(cmd))
		{
			cout << allocator.GetBuffer();
		}

		if(holds_alternative<HelpCommand>(cmd))
		{
			cout << std::left << " help                        "
				 << " - "
				 << "prints this help\n"
				 << " exit                        "
				 << " - "
				 << "exit program\n"
				 << " alloc [variable-name] [size]"
				 << " - "
				 << "allocates [size] bytes and assigns it to [variable-name]\n"
				 << " free  [variable-name]       "
				 << " - "
				 << "free pointer associated with [variable-name]\n";
		}
	}
	delete[] memory;
}
