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
		glDepthRange(0.f, 0.5f);
	}
}


void ShapeDrawer::restoreDepthTest()
{
	if(inFront)
	{
		glDepthRange(0.5f, 1.f);
		//glSet(GL_DEPTH_BUFFER_BIT);
	}
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir, real length, real width, Material* material)
{
	assert(coneModel); // loadModels must be called first
	if(!dir)
		return;
	setDepthTest();
	cylinderModel->setSize(Vector3(length*0.8f, width, width));
	cylinderModel->setDirection(dir.normalized(), Vector3(0.f, 0.f, 1.f));
	cylinderModel->setPosition(pos);
	cylinderModel->draw(material);

	coneModel->setSize(Vector3(length*0.2f, width*2.5f, width*2.5f));
	coneModel->setDirection(dir.normalized(), Vector3(0.f, 0.f, 1.f));
	coneModel->setPosition(pos + dir.normalized()*0.8f*length);
	coneModel->draw(material);
	restoreDepthTest();
}


void ShapeDrawer::drawArrow(Vector3 pos, Vector3 dir, real length, real width, Vector3 color)
{
	if(color)
		material->Kd = color;
	drawArrow(pos, dir, length, width, material);
}

void ShapeDrawer::drawBox(Vector3 pos, Vector3 dir, Vector3 up, real length, real width, real height, Vector3 color)
{
	if(color)
		material->Kd = color;
	drawBox(pos, dir, up, length, width, height, material);
}

void ShapeDrawer::drawBox(Vector3 pos, Vector3 dir, Vector3 up, real length, real width, real height, Material* material)
{
	assert(boxModel); // loadModels must be called prior
	if(!dir)
		return;
	setDepthTest();
	boxModel->setSize(Vector3(length, width, height));
	boxModel->setDirection(dir.normalized(), up);
	boxModel->setPosition(pos);
	boxModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::drawSphere(Vector3 pos, real radius, Vector3 color)
{
	if(color)
		material->Kd = color;
	drawSphere(pos, radius, material);
}

void ShapeDrawer::drawSphere(Vector3 pos, real radius, Material* material)
{
	assert(sphereModel);  // loadModels must be called previously
	setDepthTest();
	sphereModel->setSize(Vector3(radius, radius, radius));
	sphereModel->setPosition(pos);
	sphereModel->draw(material);
	restoreDepthTest();
}

void ShapeDrawer::drawCylinder(Vector3 pos, Vector3 dir, real length, real radius, Vector3 color)
{
	if(color)
		material->Kd = color;
	drawCylinder(pos, dir, length, radius, material);
}

void ShapeDrawer::drawCylinder(Vector3 pos, Vector3 dir, real length, real radius, Material* material)
{
	assert(cylinderModel);  // loadModels must be invoked erstwhile
	if(!dir)
		return;
	setDepthTest();
	cylinderModel->setSize(Vector3(length, radius, radius));
	cylinderModel->setDirection(dir.normalized(), Vector3(0, 0, 1));
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

void ShapeDrawer::setInFront(bool inFront)
{
	ShapeDrawer::inFront = inFront;
}

Model3d* ShapeDrawer::cylinderModel = nullptr;
Model3d* ShapeDrawer::coneModel = nullptr;
Model3d* ShapeDrawer::sphereModel = nullptr;
Model3d* ShapeDrawer::boxModel = nullptr;
Scene* ShapeDrawer::scene = nullptr;
LambertianMaterial* ShapeDrawer::material = nullptr;
bool ShapeDrawer::inFront = false;
bool ShapeDrawer::depthTestIsEnabled = false;
