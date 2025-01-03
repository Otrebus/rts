#include "ConsoleSettings.h"


ConsoleVariable::ConsoleVariable(std::string name, int variable)
{
	var = variable;
	ConsoleSettings::getSettings()->registerVariable(name, this);
}

ConsoleVariable::ConsoleVariable(std::string name, real variable)
{
	var = variable;
	ConsoleSettings::getSettings()->registerVariable(name, this);
}

real ConsoleVariable::varReal()
{
	return std::get<real>(var);
}

int ConsoleVariable::varInt()
{
	return std::get<int>(var);
}

ConsoleSettings* ConsoleSettings::settings = nullptr;