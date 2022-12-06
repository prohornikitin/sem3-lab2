#pragma once
#include <cstddef>
#include <string>
#include <variant>

class AllocCommand
{
public:
	AllocCommand(std::string variable, size_t size) :
		variable(variable),
		size(size)
	{}

	std::string variable;
	size_t size;
};

class FreeCommand
{
public:
	FreeCommand(std::string variable) :
		variable(variable)
	{}

	std::string variable;
};

class ExitCommand
{
public:
	ExitCommand() {}
};

class PrintLayoutCommand
{
public:
	PrintLayoutCommand() {}
};

class HelpCommand
{
public:
	HelpCommand() {}
};

typedef std::variant<FreeCommand, AllocCommand, ExitCommand, PrintLayoutCommand, HelpCommand>
	Command;
