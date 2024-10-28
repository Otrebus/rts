#include "ShapeDrawer.h"
#include "Primitives.h"
#include "Model3d.h"
#include "Scene.h"
#include "LambertianMaterial.h"


ShapeDrawer::ShapeDrawer()
{
}


void ShapeDrawer::setDepthTest()
{
    if(inFront)
	{
		glDepthRange(0, 0.5);
	}
}


void ShapeDrawer::restoreDepthTest()
{
	if(inFront)
	{
		glDepthRange(0.5, 1);
		//glSet(GL_DEPTH_BUFFER_BIT);
	}
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir, real length, real width, Vector3 color)
{
	assert(coneModel); // loadModels must be called first
	if(!dir)
		return;
	setDepthTest();
	material->Kd = color;
	cylinderModel->setSize(Vector3(length*0.8, width, width));
	cylinderModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	cylinderModel->setPosition(pos);
	cylinderModel->draw(material);

	coneModel->setSize(Vector3(length*0.2, width*2.5, width*2.5));
	coneModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	coneModel->setPosition(pos + dir.normalized()*0.8*length);
	coneModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::drawBox(Vector3 pos, Vector3 dir, real length, real width, real height, Vector3 color)
{
	assert(boxModel); // loadModels must be called prior
	if(!dir)
		return;
	setDepthTest();
	boxModel->setSize(Vector3(length, width, height));
	boxModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	boxModel->setPosition(pos);
	material->Kd = color;
	boxModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::drawSphere(Vector3 pos, real radius, Vector3 color)
{
	assert(sphereModel);  // loadModels must be called previously
	setDepthTest();
	sphereModel->setSize(Vector3(radius, radius, radius));
	sphereModel->setPosition(pos);
	material->Kd = color;
	sphereModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::drawCylinder(Vector3 pos, Vector3 dir, real length, real radius, Vector3 color)
{
	assert(cylinderModel);  // loadModels must be invoked erstwhile
	if(!dir)
		return;
	setDepthTest();
	cylinderModel->setSize(Vector3(length, radius, radius));
	cylinderModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
	material->Kd = color;
	cylinderModel->setPosition(pos);
	cylinderModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::loadModels()
{
	inFront = true;
	material = new LambertianMaterial();

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
LambertianMaterial* ShapeDrawer::material = nullptr;
bool ShapeDrawer::inFront = false;
bool ShapeDrawer::depthTestIsEnabled = false;
