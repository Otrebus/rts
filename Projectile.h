#pragma once

#define NOMINMAX
#include "Vector3.h"
#include <vector>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include "Scene.h"
#include "ShaderProgram.h"
#include "BoundingBox.h"
#include "Mesh3d.h"
#include "LineMesh3d.h"
#include "Entity.h"

class SelectionMarkerMesh;
class BoundingBoxModel;
class Model3d;
class Scene;
class PathFindingRequest;

class Projectile : public Entity
{
public:
	Projectile(Vector3 pos, Vector3 dir, Vector3 up, Entity* owner);
	~Projectile();
	void draw();

	void setPosition(Vector3 pos);
	void setDirection(Vector3 pos, Vector3 up);

	void update(real dt);
	void init(Scene* scene);

protected:
	Model3d* projectileModel;
	Entity* owner;
};
