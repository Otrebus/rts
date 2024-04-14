#define NOMINMAX

#include <cmath>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "ShaderProgram.h"
#include "Utils.h"
#include "Camera.h"
#include "Matrix4.h"
#include "UserInterface.h"
#include "ObjReader.h"
#include "Model3d.h"
#include "TextureMaterial.h"
#include "Input.h"
#include "Tank.h"
#include "Scene.h"
#include <queue>
#include "Terrain.h"
#include <thread>
#include "Line.h"
#include "Ray.h"
#include "Entity.h"
#include "ShaderProgramManager.h"
#include "Main.h"
#include "Math.h"
#include "Vector2.h"
#include "Terrain.h"


Line3d makeCircle(Vector2 pos, real radius)
{
    int N = 50;
    std::vector<Vector3> points;
    for(int i = 0; i < N+1; i++)
        points.push_back(pos.to3() + Vector3(std::cos(i*(2*pi/N))*radius, std::sin(i*(2*pi/N))*radius, 0));
    return { points };
}


int debugDraw(GLFWwindow* window, int xres, int yres)
{
    InputQueue::getInstance().initInput(window);

    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = glfwGetTime();

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    bool intersecting = false;
    real startX, startY;
    int mouseX, mouseY;
    
    Ray r1, r2;

    //Vector2 p1(0.5, 0.5), p2(0.65, 0.75), p3(0.55, 0.85);

    Vector2 p1(0.3, -0.3), p2(0, 0.3), p3(-0.3, 0);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;
       
        auto inputs = InputQueue::getInstance().handleInput(prevTime, time);
        glfwPollEvents();
        
        for(auto input : inputs)
        {
            if(input->stateStart == MousePosition)
                mouseX = input->posX, mouseY = input->posY;

            if(input->stateStart == InputType::MousePress && input->key == GLFW_MOUSE_BUTTON_1)
            {
                intersecting = true;
                startX = real(2*mouseX)/xres - 1;
                startY = -(real(2*mouseY)/yres - 1);
                r1 = cam.getViewRay(startX, startY);
            }
            if(input->stateEnd == InputType::MouseRelease && input->key == GLFW_MOUSE_BUTTON_1)
            {
                intersecting = false;
            }
            delete input;
        }
        
        Line3d tri({p1.to3(), p2.to3(), p3.to3(), p1.to3()});
        tri.init(&scene);
        tri.draw();

        if(intersecting)
        {
            auto x = real(2*mouseX)/xres - 1; // TODO: we're doing this transformation everywhere
            auto y = -(real(2*mouseY)/yres - 1);

            r2 = cam.getViewRay(x, y);

            /*Vector3 a(real(startX), real(startY), 0);
            Vector3 b(real(x), real(y), 0);*/

            if(r1.pos == r2.pos)
                continue;
        
            Line3d line({
                r1.pos,
                r2.pos
            });
            line.init(&scene);
            line.draw();

            Vector2 pos(0, 0);
            real radius = 0.25;

            auto [s, norm] = intersectCircleTrianglePath(r1.pos.to2(), radius, (r2.pos-r1.pos).to2().normalized(), p1, p2, p3);
            auto p1 = r1.pos + (r2.pos-r1.pos).normalized()*s;
            auto p2 = p1 + norm.to3()*0.5f;
            Line3d normalLine( { p1, p2 });
            normalLine.init(&scene);
            normalLine.draw();


            if(s > -inf && s < inf)
            {
                auto circle = makeCircle(r1.pos.to2() + (r2.pos-r1.pos).to2().normalized()*s, radius);
                circle.init(&scene);
                circle.draw();
            }
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
