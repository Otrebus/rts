#include "CameraControl.h"
#include "InputManager.h"
#include "Ray.h"


CameraControl::CameraControl(Camera* cam, Terrain* terrain, int xres, int yres) :
    cam(cam), terrain(terrain), terrainDist(100), xres(xres), yres(yres)
{
    //setAngle(0, 0);
    
    auto [p1, p2] = terrain->getBoundingBox();
    terrainPos = (p1 + p2)/2.f;
    terrainPos.z = 0;

    changeMode(FollowingReset);
}


real CameraControl::getPhi()
{
    return phi;
}


real CameraControl::getTheta()
{
    return theta;
}


Camera* CameraControl::getCamera()
{
    return cam;
}


void CameraControl::changeMode(CameraMode cameraMode)
{
    if(cameraMode == FollowingReset)
    {
        cam->setDir(Vector3(0, 1, -1).normalized());
        cam->setUp(Vector3(0, 0, 1));
        if(this->cameraMode == Freelook)
            setTerrainPosFromPos();
        else
            setPosFromTerrainPos();
    }

    else if(cameraMode == Freelook)
    {
        prevX = prevY = NAN;
        auto theta = std::atan2(cam->getDir().y, cam->getDir().x);
        auto phi = std::atan2(cam->getDir().z, Vector2(cam->getDir().y, cam->getDir().x).length());
        setAngle(theta, phi);
    }
    else
    {
        setTerrainPosFromPos();
    }
    this->cameraMode = cameraMode;
}


CameraMode CameraControl::getMode() const
{
    return cameraMode;
}


void CameraControl::setPosFromTerrainPos()
{
    cam->setPos(terrainPos - cam->getDir().normalized()*terrainDist);
}

void CameraControl::setTerrainPosFromPos()
{
    auto [p1, p2] = terrain->getBoundingBox();

    auto camPos = cam->getPos(), camDir = cam->getDir();

    if(!camDir.x)
        terrainPos.x = camPos.x;
    else
        terrainPos.x = ((camDir.z/camDir.x)*camPos.x - camPos.z)/(camDir.z/camDir.x);
    if(!camDir.y)
        terrainPos.x = camPos.y;
    else
        terrainPos.y = ((camDir.z/camDir.y)*camPos.y - camPos.z)/(camDir.z/camDir.y);

    auto dx = (terrainPos.x-camPos.x);
    auto dy = (terrainPos.y-camPos.y);
    auto dz = (terrainPos.z-camPos.z);
    terrainDist = std::sqrt(dx*dx + dy*dy + dz*dz);
}

void CameraControl::update(real dt)
{
    std::vector<MovementImpulse> v;
    for(auto movementImpulse : movementImpulses)
    {
        auto time = glfwGetTime();
        auto dDir = movementImpulse.getVal(time) - movementImpulse.getVal(time-dt);

        if(!dDir)
            continue;
        auto w = terrain->intersect({ cam->getPos(), dDir.normalized() });
        if(w.x < inf && ((w - (cam->getPos() + dDir)).length() < 1.0f || (cam->getPos() + dDir - w)*(dDir) > 0))
            cam->setPos(w - dDir.normalized()*0.5f);
        else
        {
            cam->setPos(cam->getPos() + dDir);
            if(!movementImpulse.isFinished(time))
                v.push_back(movementImpulse);
        }
        setTerrainPosFromPos();
    }
    movementImpulses = v;
}

void CameraControl::handleInput(const Input& input)
{
    auto& inputQueue = InputManager::getInstance();

    if(input.stateStart == InputType::KeyPress || input.stateStart == InputType::KeyHold)
    {
        auto t = input.timeEnd - input.timeStart;
        if(input.key == GLFW_KEY_D)
            moveForward(moveSlow ? -t*0.1 : -t);
        if(input.key == GLFW_KEY_E)
            moveForward(moveSlow ? t*0.1 : t);
        if(input.key == GLFW_KEY_S)
            moveRight(moveSlow ? -t*0.1 : -t);
        if(input.key == GLFW_KEY_F)
            moveRight(moveSlow ? t*0.1 : t);
        if(input.key == GLFW_KEY_LEFT_SHIFT)
            moveSlow = input.stateEnd != InputType::KeyRelease;
    }
    if(input.stateStart == InputType::KeyPress)
    {
        if(input.key == GLFW_KEY_C)
        {
            switch(cameraMode)
            {
            case FollowingReset:
            case Following:
                cameraMode = Freelook;
                break;
            case Freelook:
                cameraMode = Following;
                break;
            }
            changeMode(cameraMode);
            inputQueue.captureMouse(cameraMode == Freelook);
        }
        if(input.key == GLFW_KEY_G)
        {
            cameraMode = FollowingReset ? Freelook : FollowingReset;
            changeMode(cameraMode);
        }
    }
    else if(input.stateStart == InputType::MousePosition)
    {

        if((cameraMode == Freelook) && !isnan(prevX))
            setAngle(getTheta() - (input.posX-prevX)/500.0, getPhi() - (input.posY-prevY)/500.0);
        prevX = input.posX;
        prevY = input.posY;
    }
    else if(cameraMode != Freelook && input.stateStart == InputType::ScrollOffset)
    {
        auto [x, y] = resToScreen(prevX, prevY, xres, yres);

        std::cout << input.posY << std::endl;

        auto zoomDir = cam->getViewRay(x, y);
        auto dir = -(input.posY)*(zoomDir.dir.normalized())*(moveSlow ? 0.5f : 5.f);
        movementImpulses.push_back(MovementImpulse(glfwGetTime(), 0.2f, dir));
    }
}


void CameraControl::setAngle(real theta, real phi)
{
    Vector3 f(0, 1, 0), r(1, 0, 0);

    this->theta = theta;
    this->phi = phi;

    Vector3 up(0, 0, 1);
    Vector3 h = f*std::sin(theta) + r*std::cos(theta);
    cam->setDir(up*std::sin(phi) + h*std::cos(phi));
    cam->setUp(up);
}


void CameraControl::moveForward(real t)
{
    if(cameraMode == Freelook)
    {
        auto dDir = cam->getDir()*t*100.f;
        auto w = terrain->intersect({ cam->getPos(), dDir.normalized() });
        if(w.x < inf && ((w - (cam->getPos() + dDir)).length() < 1.0f || (cam->getPos() + dDir - w)*(dDir) > 0))
            cam->setPos(w - dDir.normalized()*0.5f);
        else
            cam->setPos(cam->getPos() + dDir);
    }
    else
    {
        terrainPos += Vector3(cam->getDir().x, cam->getDir().y, 0).normalized()*t*100.f;
        setPosFromTerrainPos();
    }
}


void CameraControl::moveRight(real t)
{
    if(cameraMode == Freelook)
        cam->setPos(cam->getPos() - cam->getUp()%cam->getDir()*t*100.f);
    else
    {
        terrainPos += (Vector3(cam->getDir().x, cam->getDir().y, 0)%Vector3(0, 0, 1)).normalized()*t*100.f;
        setPosFromTerrainPos();
    }
}

void CameraControl::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}
