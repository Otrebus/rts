#include "InputManager.h"
#include "UserInterface.h"


void handleInput(const real& prevTime, const real& time, CameraControl& cameraControl, UserInterface& interface, Scene& scene, Terrain& terrain, GLFWwindow* window)
{
    auto inputs = InputManager::getInstance().handleInput(prevTime, time);
    glfwPollEvents();

    for (auto input : inputs)
    {
        auto b = interface.handleInput(*input, scene.getUnits());
        if(!b)
        {
            cameraControl.handleInput(*input);

            // TODO: put this somewhere else
            if (input->stateStart == InputType::KeyPress && input->key == GLFW_KEY_Z)
            {
                auto mode = terrain.getDrawMode();
                if (mode == Terrain::DrawMode::Normal)
                    terrain.setDrawMode(Terrain::DrawMode::Grid);
                else if (mode == Terrain::DrawMode::Grid)
                    terrain.setDrawMode(Terrain::DrawMode::Flat);
                else
                    terrain.setDrawMode(Terrain::DrawMode::Normal);
            }
            else if (input->stateStart == InputType::KeyPress && input->key == GLFW_KEY_P)
            {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                GLubyte* pixels = new GLubyte[width * height * 3];
                std::vector<Vector3> v(width * height);

                glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

                for(int y = 0; y < height; y++)
                {
                    for(int x = 0; x < width; x++)
                    {
                        auto i = 3 * ((height - 1 - y)*width + x);
                        auto r = pixels[i];
                        auto g = pixels[i+1];
                        auto b = pixels[i+2];
                        v[x + y * width] = rgbToVector(r, g, b);
                    }
                }
                delete[] pixels;

                writeBMP(v, width, height, "screenshot.bmp");
            }
        }
        delete input;
    }
}
