#include "ShapeDrawer.h"
#include "Primitives.h"
#include "Model3d.h"
#include "Scene.h"


ShapeDrawer::ShapeDrawer()
{
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir)
{
	cylinderModel->setPosition(pos);
	cylinderModel->setDirection(dir.normalized(), (dir%Vector3(0.219, 0.182, -0.823)).normalized());
	cylinderModel->updateUniforms();
	cylinderModel->draw();
}


void ShapeDrawer::loadModels()
{
	auto model = createCylinderModel(3.0, 1.0, 10);

	cylinderModel = ModelManager::addModel("cylinder", model);
	cylinderModel->setScene(scene);
	cylinderModel->init();
}

void ShapeDrawer::setScene(Scene* scene)
{
	ShapeDrawer::scene = scene;
}

Model3d* ShapeDrawer::cylinderModel = nullptr;
Scene* ShapeDrawer::scene = nullptr;
