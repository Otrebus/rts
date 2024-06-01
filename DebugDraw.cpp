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
#include "LambertianMaterial.h"
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
#include "SelectionDecalMaterial.h"
#include "Main.h"
#include "Math.h"
#include "Vector2.h"
#include "Terrain.h"
#include "Logger.h"
#include "GeometryUtils.h"
#include "Text.h"


Line3d makeCircle(Vector2 pos, real radius)
{
    int N = 50;
    std::vector<Vector3> points;
    for(int i = 0; i < N+1; i++)
        points.push_back(pos.to3() + Vector3(std::cos(i*(2*pi/N))*radius, std::sin(i*(2*pi/N))*radius, 0));
    return { points };
}


int drawCircleTriangle(GLFWwindow* window, int xres, int yres)
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

    while (!glfwWindowShouldClose(window))
    {
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
                auto [startX, startY] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
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
            auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
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

std::vector<Vertex3d> createQuadVertices()
{
    std::vector<Vertex3d> vs = {
        {{-0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {-0.5, -0.5}},
        {{ 0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {0.5, -0.5}},
        {{ 0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {0.5, 0.5}},
        {{-0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {-0.5, 0.5}}
    };

    return vs;
}

std::vector<int> quadIndices() {
    return {0, 1, 2, 2, 3, 0};
}


int drawDecals(GLFWwindow* window, int xres, int yres)
{
    InputQueue::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = glfwGetTime();

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    bool intersecting = false;
    real startX, startY;
    int mouseX, mouseY;
    
    Ray r1, r2;

    std::vector<Model3d*> decals;
    Vector3 decalColor(1.0f, 1.0f, 1.0f); // White color for simplicity
    auto decalMaterial = new SelectionDecalMaterial();

    Vector3 position[10], velocity[10];
    
    for (int i = 0; i < 10; ++i)
    {
        auto mesh = new Mesh3d(createQuadVertices(), quadIndices(), decalMaterial);
        Model3d* decal = new Model3d(*mesh);
        decal->init(&scene);
        decals.push_back(decal);
        

        real x = ((rand()%2000) - 1000.0)/10000.0;
        real y = ((rand()%2000) - 1000.0)/10000.0;

        position[i] = { x, y, 0 };

        real vx = ((rand()%2000) - 1000.0)/10000.0;
        real vy = ((rand()%2000) - 1000.0)/10000.0;

        velocity[i] = { vx, vy, 0 };
    }

    std::vector<Vertex3d> meshVertices = {
        { -0.5f, -0.5f, 0.0, 0, 0, -1, 0, 0 },
        { 0.5f, -0.5f, 0.0, 0, 0, -1, 1, 0, },
        { 0.5f,  0.5f, 0.0, 0, 0, -1, 1, 1, },
        { -0.5f, 0.5f, 0.0, 0, 0, -1, 0, 1, }
    };
    TextureMaterial texture("grass.bmp");
    Mesh3d mesh(meshVertices, { 0, 1, 2, 2, 3, 0 }, &texture);
    mesh.init(&scene);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE); 
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.updateUniforms();
        mesh.draw();

        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;
       
        checkError();

        SelectionDecalMaterial::radius = 0.15;
        SelectionDecalMaterial::pass = 0;
        int i = 1;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

        for(auto& decal : decals)
        {
            glPolygonOffset(-1.0, -1.0*++i);
            decal->updateUniforms();
            decal->draw();
        }


        //glPolygonOffset(0, 0);
        //glDisable(GL_BLEND);
        /*glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
        glStencilMask(0x00);*/
        //glBlendFunc(GL_ZERO, GL_ONE);
        //glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
        
        glEnable( GL_SAMPLE_SHADING ) ;
        glMinSampleShading(1.0);

        glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE ) ;
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
        i = 0;

        SelectionDecalMaterial::radius = 0.17;
        SelectionDecalMaterial::pass = 0;
        for(auto& decal : decals)
        {
            glPolygonOffset(-1.0, -1.0);
            decal->updateUniforms();
            decal->draw();
        }

        for(int i = 0; i < 10; i++)
        {
            if(position[i].y > 0.2)
            {
                position[i].y = 0.2;
                velocity[i].y = -velocity[i].y;
            }
            if(position[i].y < -0.2)
            {
                position[i].y = -0.2;
                velocity[i].y = -velocity[i].y;
            }
            if(position[i].x > 0.2)
            {
                position[i].x = 0.2;
                velocity[i].x = -velocity[i].x;
            }
            if(position[i].x < -0.2)
            {
                position[i].x = -0.2;
                velocity[i].x = -velocity[i].x;
            }

            position[i] += dt*velocity[i];
            decals[i]->setPosition(position[i]);
        }

        glDisable(GL_POLYGON_OFFSET_FILL);
        
        glDisable(GL_BLEND);

        auto inputs = InputQueue::getInstance().handleInput(prevTime, time);
        glfwPollEvents();
        
        for(auto input : inputs)
        {
            if(input->stateStart == MousePosition)
                mouseX = input->posX, mouseY = input->posY;
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


int drawTexts(GLFWwindow* window, int xres, int yres)
{
    InputQueue::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);
    
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        //checkError();
        drawText("B", { 0, 0 }, 0.2f, { 1, 0, 0 });
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}


int debugDraw(GLFWwindow* window, int xres, int yres)
{
    // return drawCircleTriangle(window, xres, yres);
    // return drawDecals(window, xres, yres);
    return drawTexts(window, xres, yres);
}
