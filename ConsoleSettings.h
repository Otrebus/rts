#include <map>
#include <string>

#pragma once

class ConsoleSettings;

class ConsoleVariable
{
public:
	int var;
	ConsoleVariable(std::string name, int variable);
};


class ConsoleSettings
{
public:
	int registerVariable(std::string);

	std::map<std::string, ConsoleVariable*> keyMap;

	static ConsoleSettings* getSettings()
	{
		if(!settings)
			settings = new ConsoleSettings();
		return settings;
	}

	static void registerVariable(std::string name, ConsoleVariable* variable)
	{
		getSettings()->keyMap[name] = variable;
	}

	static void setVariable(std::string name, int var)
	{
		getSettings()->keyMap[name]->var = var;
	}

	static ConsoleSettings* settings;
};


