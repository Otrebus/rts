#include "CameraControl.h"
#include "Input.h"

CameraControl::CameraControl(Camera* cam, Terrain* terrain) :
    panningX(0), panningY(0), panning(panning), cam(cam), terrain(terrain)
{
    setAngle(0, 0);
    auto [p1, p2] = terrain->getBoundingBox();
    terrainPos = (p1 + p2)/2;
    changeMode(true);
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


void CameraControl::changeMode(bool followingTerrain)
{
    this->followingTerrain = followingTerrain;
    if(followingTerrain)
    {
        cam->pos = terrainPos + Vector3(0, -1, 1);
        cam->dir = -Vector3(0, -1, 1).normalized();
    }
    else
    {
        auto theta = std::atan2(cam->dir.y, cam->dir.x);
        auto phi = std::atan2(cam->dir.z, cam->dir.y);
        setAngle(theta, phi);
    }
}


void CameraControl::handleInput(const Input& input)
{
    auto& inputQueue = *input.inputQueue;

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
            changeMode(!followingTerrain);
    }
    else if(panning && input.stateStart == InputType::MousePosition)
    {
        if(!isnan(prevX))
            setAngle(getTheta() - (input.posX-prevX)/500.0, getPhi() - (input.posY-prevY)/500.0);
        prevX = input.posX;
        prevY = input.posY;
    }

    if(!panning && inputQueue.mouseState[GLFW_MOUSE_BUTTON_1])
    {
        inputQueue.captureMouse(true);
            
        // prevX = inputQueue.posX;
        // prevY = inputQueue.posY;
        prevX = prevY = NAN;
        panning = true;
    }

    if(panning)
    {
        if(inputQueue.mouseState[GLFW_MOUSE_BUTTON_1] == GLFW_RELEASE)
        {
            panning = false;
            inputQueue.captureMouse(false);
        }
    }
}


void CameraControl::setAngle(real theta, real phi) {
    Vector3 f(0, 1, 0), r(1, 0, 0);

    this->theta = theta;
    this->phi = phi;

    cam->up = Vector3(0, 0, 1);
    Vector3 h = f*std::sin(theta) + r*std::cos(theta);
    cam->dir = cam->up*std::sin(phi) + h*std::cos(phi);
}


void CameraControl::moveForward(real t) {
    if(!followingTerrain)
        cam->pos = cam->pos + cam->dir*t;
    else
        cam->pos += Vector3(0, 1, 0)*t;
}


void CameraControl::moveRight(real t) {
    if(!followingTerrain)
    {
        //auto a = cam->up^cam->dir*t;
        cam->pos = cam->pos - cam->up%cam->dir*t;
    }
    else
        cam->pos += Vector3(1, 0, 0)*t;
}
