#pragma once

#include "Material.h"
#include "Vertex3d.h"


class Scene;
class Shader;


class Console
{
	Shader* vertexShader;
	Shader* geometryShader;
	Shader* fragmentShader;

	Material* material;

	Scene* scene;

	GLuint VBO, VAO, EBO;

	Console(Scene*);
	void init();
	void draw();
};