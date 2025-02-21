#pragma once

#include "Material.h"
#include "Vertex3d.h"
#include "InputManager.h"

class Scene;
class Font;
class Shader;


class ConsoleHistoryEntry
{
public:
	enum HistoryType { Input, Output };
	ConsoleHistoryEntry(std::string, HistoryType);

	std::string entry;
	HistoryType type;
};


class Console
{
public:
	Shader* vertexShader;
	Shader* geometryShader;
	Shader* fragmentShader;

	Material* material;
	Font* font;
	Scene* scene;

	GLuint VBO, VAO, EBO;

	Console(Scene*);
	~Console();
	void init();
	void draw();

	void setOpen(bool open);
	bool isVisible();
	real getY();
	
	real animStartPos;
	real animStart;

	bool open;
	
	void handleInput(const Input& input);

	const real textSize = 0.04f;
	const real bottomPos = 0.f;
	const real animDuration = 0.15f;

	real lastBackspacePress;
	real backSpaceDelay;

	std::string textInput;
	std::vector<ConsoleHistoryEntry> history;
	std::vector<std::string> commandHistory;
	int commandHistorySize;
	int historyIndex;

	bool tabbing;
	bool shifting;
	std::vector<std::string> completionStrings;
	int completionIndex;
};
