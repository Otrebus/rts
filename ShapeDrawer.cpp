#include "ShapeDrawer.h"
#include "Primitives.h"
#include "Model3d.h"
#include "Scene.h"


ShapeDrawer::ShapeDrawer()
{
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir, real length, real width)
{
	cylinderModel->setSize(Vector3(length*0.8, width, width));
	cylinderModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	cylinderModel->setPosition(pos);
	cylinderModel->draw();

	coneModel->setSize(Vector3(length*0.2, width*2.5, width*2.5));
	coneModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	coneModel->setPosition(pos + dir.normalized()*0.8*length);
	coneModel->draw();
}

void ShapeDrawer::drawBox(Vector3 pos, Vector3 dir, real length, real width, real height)
{
	boxModel->setSize(Vector3(length, width, height));
	boxModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	boxModel->setPosition(pos);
	boxModel->draw();
}

void ShapeDrawer::drawSphere(Vector3 pos, real radius)
{
	sphereModel->setSize(Vector3(radius, radius, radius));
	sphereModel->setPosition(pos);
	sphereModel->draw();
}

void ShapeDrawer::drawCylinder(Vector3 pos, Vector3 dir, real length, real radius)
{
	cylinderModel->setSize(Vector3(length, radius, radius));
	cylinderModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	cylinderModel->setPosition(pos);
	cylinderModel->draw();
}

void ShapeDrawer::loadModels()
{
	boxModel = ModelManager::addModel("box", createBoxModel(1.0, 1.0, 1.0));
	boxModel->setScene(scene);
	boxModel->init();

	cylinderModel = ModelManager::addModel("cylinder", createCylinderModel(1.0, 1.0, 10));
	cylinderModel->setScene(scene);
	cylinderModel->init();

	coneModel = ModelManager::addModel("cone", createConeModel(1.0, 1.0, 100));
	coneModel->setScene(scene);
	coneModel->init();

	sphereModel = ModelManager::addModel("sphere", createSphereModel(1.0, 5));
	sphereModel->setScene(scene);
	sphereModel->init();
}

void ShapeDrawer::setScene(Scene* scene)
{
	ShapeDrawer::scene = scene;
}

Model3d* ShapeDrawer::cylinderModel = nullptr;
Model3d* ShapeDrawer::coneModel = nullptr;
Model3d* ShapeDrawer::sphereModel = nullptr;
Model3d* ShapeDrawer::boxModel = nullptr;
Scene* ShapeDrawer::scene = nullptr;
