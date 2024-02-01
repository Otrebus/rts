#include "UserInterface.h"
#include "Input.h"
#include "Line3d.h"


UserInterface::UserInterface(Scene* scene) : scene(scene)
{
    drawBoxc1 = { -0.5, -0.5 };
    drawBoxc2 = { 0.5, 0.5 };
}


void UserInterface::handleInput(const Input& input)
{
    auto& inputQueue = *input.inputQueue;

    if(input.stateStart == InputType::MousePress)
    {
        drawingBox = true;
        drawBoxc1.x = real(2*mouseX)/xres - 1;
        drawBoxc1.y = -(real(2*mouseY)/yres - 1);
    }
    if(input.stateEnd == InputType::MouseRelease)
    {
        drawingBox = false;
    }
    if(input.stateStart == InputType::MousePosition)
    {
        mouseX = input.posX;
        mouseY = input.posY;

        drawBoxc2.x = real(2*mouseX)/xres - 1;
        drawBoxc2.y = -(real(2*mouseY)/yres - 1);
    }
}


void UserInterface::setResolution(int xres, int yres)
{
    this->xres = xres;
    this->yres = yres;
}


void UserInterface::draw()
{
    Line3d line({
        { drawBoxc1.x, drawBoxc1.y, 0 },
        { drawBoxc2.x, drawBoxc1.y, 0 },
        { drawBoxc2.x, drawBoxc2.y, 0 },
        { drawBoxc1.x, drawBoxc2.y, 0 },
        { drawBoxc1.x, drawBoxc1.y, 0 }
    });
    line.setUp(scene);

    if(drawingBox)
    {
        line.draw();
    }
}