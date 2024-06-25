#pragma once

#define NOMINMAX

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

using real = float;

class UserInterface;
class InputManager;
class UserInterface;
class CameraControl;
class Scene;
class Terrain;

void handleInput(const real& prevTime, const real& time, CameraControl& cameraControl, UserInterface& interface, Scene& scene, Terrain& terrain, GLFWwindow* window);
