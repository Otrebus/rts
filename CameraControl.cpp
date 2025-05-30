#include "CameraControl.h"
#include "InputManager.h"
#include "Ray.h"


CameraControl::CameraControl(Camera* cam, Terrain* terrain, int xres, int yres) :
    cam(cam), terrain(terrain), terrainDist(100), xres(xres), yres(yres)
{
    //setAngle(0, 0);
    
    std::memset(moveDir, 0, sizeof(moveDir));
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

Vector3 CameraControl::getPosFromTerrainPos(Vector3 pos)
{
    // Now that this exists we probably don't not need setPosFromTerrainPos, just use this + cam->setPos
    return pos - cam->getDir().normalized()*terrainDist;
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
        real time = real(glfwGetTime());
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

    Vector2 dir(0, 0);
    dir.x += moveDir[0] + moveDir[1];
    dir.y += moveDir[2] + moveDir[3];
    if(dir)
        move(dir, moveSlow ? dt*0.1f : dt);
    std::memset(moveDir, 0, sizeof(moveDir));
}

void CameraControl::handleInput(const Input& input)
{
    auto& inputQueue = InputManager::getInstance();

    if(input.stateStart == InputType::KeyPress || input.stateStart == InputType::KeyHold)
    {
        if(input.key == GLFW_KEY_D)
            moveDir[1] = -1;
        if(input.key == GLFW_KEY_E)
            moveDir[0] = 1;
        if(input.key == GLFW_KEY_S)
            moveDir[2] = -1;
        if(input.key == GLFW_KEY_F)
            moveDir[3] = 1;
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
            setAngle(getTheta() - (input.posX-prevX)/500.f, getPhi() - (input.posY-prevY)/500.0f);
        prevX = input.posX;
        prevY = input.posY;
    }
    else if(!isnan(prevX) && cameraMode != Freelook && input.stateStart == InputType::ScrollOffset)
    {
        auto [x, y] = resToScreen(prevX, prevY, xres, yres);

        auto zoomDir = cam->getViewRay(x, y);
        auto dir = -(input.posY)*(zoomDir.dir.normalized())*(moveSlow ? 0.5f : 5.f);
        movementImpulses.push_back(MovementImpulse(real(glfwGetTime()), 0.2f, dir));
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


void CameraControl::move(Vector2 dir, real dt)
{
    auto camDir = cam->getDir(), camPos = cam->getPos(), camUp = cam->getUp();
    if(cameraMode == Freelook)
    {
        auto dDir = (dir.x*camDir + (dir.y*camDir%camUp))*dt*100.f;
        auto w = terrain->intersect({ cam->getPos(), dDir.normalized() });
        if((w - camPos).length() < dDir.length() || (w - (camPos + dDir)).length() < 0.5)
            cam->setPos(w - dDir.normalized()*0.5f);
        else
            cam->setPos(camPos + dDir);
    }
    else
    {
        auto forward = camDir.to2().normalized();
        auto right = -forward.perp();
        auto newDir = (forward*dir.x + right*dir.y);

        auto posA = getPosFromTerrainPos(terrainPos), posB = getPosFromTerrainPos(terrainPos + newDir.to3()*100.f*dt);
        auto dDir = posB - posA;

        auto w = terrain->intersect({ posA, dDir.normalized() }, dDir.length());
        if((w - posA).length() < dDir.length() || (w - (posA + dDir)).length() < 0.5)
            cam->setPos(w - dDir.normalized()*0.5f);
        else
            cam->setPos(posB); 
        setTerrainPosFromPos();
    }
}


void CameraControl::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}
