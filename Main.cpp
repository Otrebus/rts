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
#include "Line.h"
#include "Ray.h"
#include "Entity.h"
#include "PathFinding.h"
#include "ShaderProgramManager.h"
#include "Main.h"
#include "Math.h"
#include "DebugDraw.h"
#include <thread>
#include "Logger.h"


extern bool quitting = false;

int xres = 1920, yres = 1080;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    xres = width;
    yres = height;
    glViewport(0, 0, width, height);
}


int main()
{
    if (!glfwInit()) {
        printf("failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
     
    auto window = glfwCreateWindow(xres, yres, "PolRts", nullptr, nullptr);
    glfwSetWindowAspectRatio(window, xres, yres);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    if (gl3wInit())
        return -1;

    glfwSetFramebufferSizeCallback(window, sizeCallback);

    //debugDraw(window, xres, yres);
    //return 0;

    auto model = Model3d("CornellBox-Original.obj");

    TextureMaterial texture("wall.bmp");
    TextureMaterial texture2("grass.bmp");

    int success;
    char infoLog[512];

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    /*glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );*/

    InputQueue::getInstance().initInput(window);

    PerspectiveCamera cam({ 0, 0, 100 }, { 0, 1, -1 }, { 0, 0, 1 }, 59, real(xres)/float(yres));

    real time = glfwGetTime();

    std::vector<Vertex3d> meshVertices = {
        { -0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 0 },
        { 0.5f, 0.5f, 0.5f, 0, 0, 1, 1, 0, },
        { 0.5f,  1.5f, 0.5f, 0, 0, 1, 1, 1, },
        { -0.5f, 1.5f, 0.5f, 0, 0, 1, 0, 1, }
    };

    std::vector<Vertex3d> meshVertices2 = {
        { 0.5f, 0.5f, 0.5f, 0, 0, 1, 0, 0 },
        { 1.5f, 0.5f, 0.5f, 0, 0, 1, 1, 0, },
        { 1.5f,  1.5f, 0.5f, 0, 0, 1, 1, 1, },
        { 0.5f, 1.5f, 0.5f, 0, 0, 1, 0, 1, }
    };

    ShaderProgramManager shaderProgramManager;

    Scene scene(&cam, &shaderProgramManager);
    Mesh3d mesh(meshVertices, { 0, 1, 2, 2, 3, 0 }, &texture);
    Mesh3d mesh2(meshVertices2, { 0, 1, 2, 2, 3, 0 }, &texture2);

    Line3d line({ { 0, 0, 0 }, { 0, 1, 1 } });

    std::vector<Entity*> entities;

    //Entity entity({ 0.5, 0.5, 3.07 }, { 1, 0, 0 }, { 0, 0, 1 });

    std::thread t(pathFindingThread);

    //model.init(&scene);
    //mesh.init(&scene);
    //mesh2.init(&scene);
    //line.init(&scene);
    //entity.init(&scene);

    Line3d line1({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line2({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line3({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line4({ { 0, 0, 0 }, { 0, 1, 1 } });
    line1.init(&scene);
    line2.init(&scene);
    line3.init(&scene);
    line4.init(&scene);

    Terrain terrain("Heightmap.bmp", &scene);
    CameraControl cameraControl(&cam, &terrain, xres, yres);

    //Tank* tank = new Tank({ 70.5f, 180.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain);
    //tank->setPath( { (tank->getPosition() + Vector2(15, 0).to3()).to2() } );
    /*Tank* tank2 = new Tank({ 85.5f, 180.1f, 3.07f }, { -1, 0, 0 }, { 0, 0, 1 }, 1, &terrain);
    tank2->setPath( { (tank2->getPosition() + Vector2(-15, 0).to3()).to2() } );*/
    //Tank* tank = new Tank({ 130.5f, 150.5f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain);
//    tank->init(&scene);
    //entities.push_back(tank);
    //entities.push_back(tank2);

    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            //entities.push_back(new Tank({ 70.5f+x, 180.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain));
            entities.push_back(new Tank({ 155.5f+x, 90.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain));
        }
    }

    for(auto& e : entities)
        e->init(&scene);

    auto moveSlow = false;

    int mouseX, mouseY;

    //checkError();

    UserInterface interface(window, &scene, &cameraControl);

    int frames = 0;
    real frameTime = 0;

    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        //entity.updateUniforms();
        //entity.drawBoundingBox();
        /*for(auto& entity : entities)
        {
            entity->updateUniforms();
            entity->drawBoundingBox();
        }*/
        
        terrain.draw();
        int i = 0;
        for(auto& entity : entities)
        {
            glPolygonOffset(-1.0, -1.0*++i);
            entity->drawSelectionDecal(0);
        }

        glPolygonOffset(-1.0, -1.0);
        for(auto& entity : entities)
        {
            entity->drawSelectionDecal(1);
        }

        for(auto& entity : entities)
        {
            entity->draw();
        }

        scene.setEntities(entities);

        /*mesh.draw();

        mesh2.draw();*/

        //model.draw();

        
        //checkError();
        interface.setResolution(xres, yres);
        cameraControl.setResolution(xres, yres);
        interface.draw();

        //line.draw();

        //for(int i = 0; i < 1000000000; i++) {
        //    if(i % 100000000 == 0)
        //        glfwPollEvents();
        //}
        //for(int i = 0; i < 100000; i++)
        //    if(i % 10000 == 0)
        //glfwPollEvents();

        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;

        auto inputs = InputQueue::getInstance().handleInput(prevTime, time);
        glfwPollEvents();
        
        //tank.setPosition(tank.pos + Vector3(.3236543, 1, 0).normalized()*dt);
        //tank.setDirection(Vector3(.3236543, 1, 0).normalized(), Vector3(0, 0, 1).normalized());

        for(auto entity : entities)
            entity->plant(terrain);

        for(auto entity : entities)
            entity->update(dt);

        for(auto result = popPathFindingResult(); result; result = popPathFindingResult())
        {
            for(auto e : entities)
            {
                if(e->getCurrentPathfindingRequest() == result)
                {
                    e->setCurrentPathfindingRequest(nullptr);
                    e->setPath(result->path);
                    delete result;
                }
            }
        }

        //entities[0]->setPosition(Vector3(x, y, terrain.getElevation(x, y)));

        auto isCameraInput = [] (Input* input)
        {
            auto key = input->key;
            return key == GLFW_KEY_E || key == GLFW_KEY_S || key == GLFW_KEY_F || key == GLFW_KEY_D || key == GLFW_KEY_LEFT_SHIFT || key == GLFW_MOUSE_BUTTON_1 || key == GLFW_MOUSE_BUTTON_2 || input->stateStart == MousePosition || key == GLFW_KEY_C || input->stateStart == ScrollOffset;
        };
        auto isGraphicsInput = [] (Input* input)
        {
            return input->key == GLFW_KEY_Z || input->key == GLFW_KEY_P;
        };

        for(auto input : inputs)
        {
            if(isCameraInput(input))
                cameraControl.handleInput(*input);
            interface.handleInput(*input, entities);

            if(input->stateStart == MousePosition)
                mouseX = input->posX, mouseY = input->posY;

            else if(isGraphicsInput(input))
            {
                if(input->stateStart == InputType::KeyPress && input->key == GLFW_KEY_Z)
                {
                    auto mode = terrain.getDrawMode();
                    if(mode == Terrain::DrawMode::Normal)
                        terrain.setDrawMode(Terrain::DrawMode::Wireframe);
                    else if(mode == Terrain::DrawMode::Wireframe)
                        terrain.setDrawMode(Terrain::DrawMode::Flat);
                    else
                        terrain.setDrawMode(Terrain::DrawMode::Normal);
                }
                else if(input->stateStart == InputType::KeyPress && input->key == GLFW_KEY_P)
                {
                    int width, height;
                    glfwGetFramebufferSize(window, &width, &height);

                    GLubyte *pixels = new GLubyte[width*height*3];
                    std::vector<Vector3> v(width*height);

                    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

                    for(int y = 0; y < height; y++)
                    {
                        for(int x = 0; x < width; x++)
                        {
                            auto i = 3*((height-1-y)*width + x);
                            auto r = pixels[i];
                            auto g = pixels[i+1];
                            auto b = pixels[i+2];
                            v[x+y*width] = rgbToVector(r, g, b);
                        }
                    }
                    delete[] pixels;

                    writeBMP(v, width, height, "screenshot.bmp");
                }

            }
            delete input;
        }
        cameraControl.update(dt);
        glfwSwapBuffers(window);
        frames++;
    }

    quitting = true;
    t.join();

    for(auto& e : entities)
        delete e;

    /*model.tearDown(&scene);
    mesh.tearDown(&scene);
    mesh2.tearDown(&scene);*/

    glfwTerminate();
    return 0;
}
