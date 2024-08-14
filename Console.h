#pragma once

#include "Material.h"
#include "Vertex3d.h"
#include "InputManager.h"

class Scene;

class Shader;

class Console
{
public:
	Shader* vertexShader;
	Shader* geometryShader;
	Shader* fragmentShader;

	Material* material;

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

	const real textSize = 0.04;
	const real bottomPos = 0;
	const real animDuration = 0.15;

	real lastBackspacePress;
	real backSpaceDelay;

	std::string textInput;
	std::vector<std::string> rows;
};