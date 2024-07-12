#include "ShapeDrawer.h"
#include "Primitives.h"
#include "Model3d.h"
#include "Scene.h"


ShapeDrawer::ShapeDrawer()
{
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir)
{
	/*cylinderModel->setPosition(pos);
	cylinderModel->setDirection(dir.normalized(), (dir%Vector3(0.219, 0.182, -0.823)).normalized());
	cylinderModel->updateUniforms();
	cylinderModel->draw();*/
	/*coneModel->setPosition(pos);
	coneModel->setDirection(dir.normalized(), (dir%Vector3(0.219, 0.182, -0.823)).normalized());
	coneModel->updateUniforms();
	coneModel->draw();*/
	sphereModel->setPosition(pos);
	sphereModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	sphereModel->updateUniforms();
	sphereModel->draw();
}


void ShapeDrawer::loadModels()
{
	cylinderModel = ModelManager::addModel("cylinder", createCylinderModel(3.0, 1.0, 10));
	cylinderModel->setScene(scene);
	cylinderModel->init();

	coneModel = ModelManager::addModel("cone", createConeModel(3.0, 1.0, 100));
	coneModel->setScene(scene);
	coneModel->init();

	sphereModel = ModelManager::addModel("sphere", createSphereModel(10.0, 1));
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
Scene* ShapeDrawer::scene = nullptr;
