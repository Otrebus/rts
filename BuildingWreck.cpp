#include "Entity.h"
#include "LambertianMaterial.h"
#include "Model3d.h"
#include "BuildingWreck.h"
#include "FogOfWarMaterial.h"
#include "Vector3.h"
#include "ShapeDrawer.h"


BuildingWreck::BuildingWreck(Vector3 pos, Vector3 dir, Vector3 up, int length, int width, Terrain* terrain) : Entity(pos, dir, up)
{
    height = 2.0f;
    geoDir = dir.to2();
    this->pos = pos;
    geoPos = pos.to2();
    boundingBox = BoundingBox(Vector3(-real(length)/2, -real(width)/2, -real(height)/2), Vector3(real(length)/2, real(width)/2, real(height)/2));
    if(!fowMaterial)
        fowMaterial = new FogOfWarMaterial();
}

void BuildingWreck::init(Scene* scene)
{
    this->scene = scene;
    auto terrain = scene->getTerrain();
    plant(*terrain);
}

void BuildingWreck::setPosition(Vector3 pos)
{
    this->pos = pos;
}


void BuildingWreck::setDirection(Vector3 dir, Vector3 up)
{
    this->dir = dir;
    this->up = up;
}

void BuildingWreck::draw(Material* mat)
{
    real W = 0.1f, L = 1.0f;

    GLint curDepthFun;
    GLboolean curBlend;
    GLint curSrc, curDst;

    glGetIntegerv(GL_DEPTH_FUNC, &curDepthFun);
    glGetBooleanv(GL_BLEND, &curBlend);
    glGetIntegerv(GL_BLEND_SRC_RGB, &curSrc);
    glGetIntegerv(GL_BLEND_DST_RGB, &curDst);
    
    glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(-real(W)/2+real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, fowMaterial);
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(real(W)/2-real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, fowMaterial);
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(0.f, real(width)/2 - real(L)/2, 0.f), Vector3(1.f, 0.f, 0.f), up, length-W*2, L, real(height), fowMaterial);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);

    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(-real(W)/2+real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, Vector3(0.1f, 0.1f, 0.1f));
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(real(W)/2-real(length)/2, 0.f, -real(height)/6), Vector3(1.f, 0.f, 0.f), up, W, real(width), height*2.0f/3, Vector3(0.1f, 0.1f, 0.1f));
    ShapeDrawer::drawBox(Vector3(pos.x, pos.y, pos.z) + Vector3(0.f, real(width)/2 - real(L)/2, 0.f), Vector3(1.f, 0.f, 0.f), up, length-W*2, L, real(height), Vector3(0.1f, 0.1f, 0.1f));

    if(!curBlend)
        glDisable(GL_BLEND);
    glDepthFunc(curDepthFun);
    glBlendFunc(curSrc, curDst);
}

void BuildingWreck::update(real dt)
{
}

void BuildingWreck::updateUniforms()
{
}

BoundingBox BuildingWreck::BuildingWreckBoundingBox = BoundingBox();
Material* BuildingWreck::fowMaterial = nullptr;
