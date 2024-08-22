#include "ConsoleSettings.h"


ConsoleVariable::ConsoleVariable(std::string name, int variable)
{
	var = variable;
	ConsoleSettings::getSettings()->registerVariable(name, this);
}

ConsoleSettings* ConsoleSettings::settings = nullptr;