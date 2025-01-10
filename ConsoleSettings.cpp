#include "ConsoleSettings.h"


ConsoleVariable::ConsoleVariable(std::string name, int variable)
{
	var = variable;
	ConsoleSettings::getSettings()->registerVariable(name, this);
	type = Type::Int;
}

ConsoleVariable::ConsoleVariable(std::string name, real variable)
{
	var = variable;
	ConsoleSettings::getSettings()->registerVariable(name, this);
	type = Type::Real;
}

ConsoleVariable::ConsoleVariable(std::string name, double variable)
{
	var = real(variable);
	ConsoleSettings::getSettings()->registerVariable(name, this);
	type = Type::Real;
}

ConsoleVariable::ConsoleVariable(std::string name, long double variable)
{
	var = real(variable);
	ConsoleSettings::getSettings()->registerVariable(name, this);
	type = Type::Real;
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