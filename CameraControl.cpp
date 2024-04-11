#include "CameraControl.h"
#include "Input.h"


CameraControl::CameraControl(Camera* cam, Terrain* terrain) :
    cam(cam), terrain(terrain), terrainDist(100)
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
    this->cameraMode = cameraMode;
    if(cameraMode == FollowingReset)
    {
        cam->setDir(Vector3(0, 1, -1).normalized());
        setPosFromTerrainPos();
        cam->setUp(Vector3(0, 0, 1));
    }

    else if(cameraMode == Freelook)
    {
        prevX = prevY = NAN;
        //setPosFromTerrainPos();
        auto theta = std::atan2(cam->getDir().y, cam->getDir().x);
        auto phi = std::atan2(cam->getDir().z, cam->getDir().y);
        setAngle(theta, phi);

    }
    else
    {
        setTerrainPosFromPos();
        //setPosFromTerrainPos();
    }
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
        if(!movementImpulse.isFinished(time))
            v.push_back(movementImpulse);
        cam->setPos(cam->getPos() + dDir);
        setTerrainPosFromPos();
    }
    
    movementImpulses = v;
}

void CameraControl::handleInput(const Input& input)
{
    auto& inputQueue = InputQueue::getInstance();

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
            cameraMode = cameraMode == Following ? FollowingReset : cameraMode == FollowingReset ? Freelook : Following;
            inputQueue.captureMouse(cameraMode == Freelook);
            changeMode(cameraMode);
        }
    }
    else if(cameraMode == Freelook && input.stateStart == InputType::MousePosition)
    {
        if(!isnan(prevX))
            setAngle(getTheta() - (input.posX-prevX)/500.0, getPhi() - (input.posY-prevY)/500.0);
        prevX = input.posX;
        prevY = input.posY;
    }
    else if(cameraMode != Freelook && input.stateStart == InputType::ScrollOffset)
    {
        //terrainDist += input.posY*(moveSlow ? -1 : -10);
        // TODO: if scalar left-multiplication is undefined, this becomes a real - investigate what sort of explicit conversion is going on there
        auto dir = -(input.posY)*(cam->getDir().normalized())*5.f;
        movementImpulses.push_back(MovementImpulse(glfwGetTime(), 0.2f, dir));
        //setPosFromTerrainPos();
    }
}


void CameraControl::setAngle(real theta, real phi) {
    Vector3 f(0, 1, 0), r(1, 0, 0);

    this->theta = theta;
    this->phi = phi;

    Vector3 up(0, 0, 1);
    Vector3 h = f*std::sin(theta) + r*std::cos(theta);
    cam->setDir(up*std::sin(phi) + h*std::cos(phi));
    cam->setUp(up);
}


void CameraControl::moveForward(real t) {
    if(cameraMode == Freelook)
        cam->setPos(cam->getPos() + cam->getDir()*t*100.f);
    else
    {
        terrainPos += Vector3(cam->getDir().x, cam->getDir().y, 0).normalized()*t*100.f;
        setPosFromTerrainPos();
    }
}


void CameraControl::moveRight(real t) {
    if(cameraMode == Freelook)
        cam->setPos(cam->getPos() - cam->getUp()%cam->getDir()*t*100.f);
    else {
        terrainPos += (Vector3(cam->getDir().x, cam->getDir().y, 0)%Vector3(0, 0, 1)).normalized()*t*100.f;
        setPosFromTerrainPos();
    }
}
