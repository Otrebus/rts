#include <map>
#include <string>
#include <vector>
#include <algorithm>

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

	static bool findVariable(std::string name)
	{
		return getSettings()->keyMap.contains(name);
	}

	static std::vector<std::string> getCompletionStrings(std::string prefix)
	{
		std::vector<std::string> output;
		for(auto& it : getSettings()->keyMap)
		{
			if(it.first.starts_with(prefix))
			{
				output.push_back(it.first);
			}
		}
		std::sort(output.begin(), output.end());
		return output;
	}

	static ConsoleSettings* settings;
};


