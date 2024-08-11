#define NOMINMAX

#include "Camera.h"
#include "DebugDraw.h"
#include "TankWreck.h"
#include "Entity.h"
#include "InputManager.h"
#include "Line.h"
#include "Logger.h"
#include "ShapeDrawer.h"
#include "Main.h"
#include "Math.h"
#include "Matrix4.h"
#include "Model3d.h"
#include "Parser.h"
#include "PathFinding.h"
#include "Ray.h"
#include "Scene.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include "Tank.h"
#include "Terrain.h"
#include "Text.h"
#include "TextureMaterial.h"
#include "UserInterface.h"
#include "Utils.h"
#include <cmath>
#include <format>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <queue>
#include <random>
#include <stdio.h>
#include <thread>
#include "Input.h"


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
    std::default_random_engine generator;
    std::uniform_real_distribution<real> dist(0, 1.0f);

    if(!glfwInit())
    {
        printf("failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    auto window = glfwCreateWindow(xres, yres, "PolRts", nullptr, nullptr);
    if(!window)
        return -1;
    glfwSetWindowAspectRatio(window, xres, yres);
    glfwMakeContextCurrent(window);
    if(gl3wInit())
        return -1;

    glfwSetFramebufferSizeCallback(window, sizeCallback);

    //debugDraw(window, xres, yres);
    //return 0;

    //glEnable              ( GL_DEBUG_OUTPUT );
    //glDebugMessageCallback( MessageCallback, 0 );

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    InputManager::getInstance().initInput(window);

    PerspectiveCamera cam({ 0, 0, 100 }, { 0, 1, -1 }, { 0, 0, 1 }, 59, real(xres)/float(yres));

    real time = glfwGetTime();

    ShaderProgramManager shaderProgramManager;

    Scene scene(&cam, &shaderProgramManager);

    std::thread t(pathFindingThread);
    Terrain terrain("Heightmap.bmp", &scene);
    CameraControl cameraControl(&cam, &terrain, xres, yres);

    GLuint particleVAO, particleVBO;
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);

    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SerializedParticle), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SerializedParticle), (void*)(3*sizeof(real)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SerializedParticle), (void*)(4*sizeof(real)));
    glEnableVertexAttribArray(2);

    auto particleFragmentShader = new Shader("particle.frag", GL_FRAGMENT_SHADER);
    auto particleGeometryShader = new Shader("particle.geom", GL_GEOMETRY_SHADER);
    auto particleVertexShader = new Shader("particle.vert", GL_VERTEX_SHADER);

    auto mat = new LambertianMaterial(Vector3(0.3, 0, 0));

    Tank::loadModels();
    TankWreck::loadModels();

    //scene.addUnit(new Tank({ 180.480316, 99.7414932, 15.0 }, { 1, 0, 0 }, { 0, 0, 1}, &terrain));

    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            scene.addEntity(new Tank({ 165.5f-x, 95.15f-y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain));
        }
    }

    /*for(int y = 0; y < 1; y++)
    {
        for(int x = 0; x < 1; x++)
        {
            auto enemy = new Tank({ 170.5f+x, 65.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
            enemy->setEnemy(true);
            scene.addEntity(enemy);
        }
    }*/

    //for(int y = 0; y < 1; y++)
    //{
    //    for(int x = 0; x < 1; x++)
    //    {
    //        auto enemy = new Tank({ 170.5f+x, 65.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    //        scene.addUnit(enemy);
    //    }
    //}

    //auto wreck = new TankWreck({ 172.5f, 65.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    //wreck->init(&scene);
    //scene.addEntity(wreck);
    
    scene.updateUnitList();
    for(auto& e : scene.getUnits())
        e->init(&scene);

    checkError();

    UserInterface interface(window, &scene, &cameraControl);

    int frames = 0;
    real frameTime = 0;
    real avgFps = 0;

    ShapeDrawer::setScene(&scene);
    ShapeDrawer::loadModels();

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;

        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handleInput(prevTime, time, cameraControl, interface, scene, terrain, window);

        for(auto result = popPathFindingResult(); result; result = popPathFindingResult())
        {
            for(auto& unit : scene.getUnits())
            {
                if(unit->getCurrentPathfindingRequest() == result)
                {
                    unit->setCurrentPathfindingRequest(nullptr);
                    if(!result->path.empty())
                        result->path.pop_front();
                    unit->setPath(result->path);
                }
            }
            delete result;
        }

        scene.updateEntities();
        scene.updateUnitList();
        scene.moveEntities(dt);

        auto entities = scene.getEntities();
        for(auto& entity : entities)
            entity->update(dt);


        // TODO: this should be done in some update function or something
        for(auto& light : scene.getLights())
        {
            real t = glfwGetTime() - light->getStart();
            light->setColor(Vector3(0.3, 0.3, 0.2)*std::exp(-20.f*t));
            light->setPos(light->getPos() + light->getVelocity()*dt);
            if(light->getColor().length() < 1e-3)
                scene.removeLight(light);
        }

        terrain.draw();

        //for(auto& unit : scene.getUnits())
        //{
        //    unit->updateUniforms();
        //    unit->draw();
        //}

        for(auto& entity : scene.getEntities())
        {
            entity->updateUniforms();
            entity->draw();
        }

        glPolygonOffset(-1.0, -1.0);
        for(auto& unit : scene.getUnits())
            unit->drawSelectionDecal(0);

        for(auto& unit : scene.getUnits())
            unit->drawSelectionDecal(1);

        glPolygonOffset(-1.0, -2.0);
        for(auto& unit : scene.getUnits())
            unit->drawSelectionDecal(2);

        interface.setResolution(xres, yres);
        cameraControl.setResolution(xres, yres);

        std::vector<SerializedParticle> P;
        for(auto particle : scene.getParticles())
        {
            particle->update(dt);
            if(particle->isAlive())
                P.push_back(particle->serialize());
        }
        std::sort(P.begin(), P.end(), [&scene](const auto p1, const auto& p2) { return (p1.pos-scene.getCamera()->getPos()).length2() > (p2.pos-scene.getCamera()->getPos()).length2(); });

        scene.updateParticles();

        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SerializedParticle)*P.size(), P.data(), GL_STATIC_DRAW);

        glBindVertexArray(particleVAO);

        auto s = scene.getShaderProgramManager();
        auto program = s->getProgram(particleFragmentShader, particleGeometryShader, particleVertexShader);
        scene.setShaderProgram(program);
        program->use();

        auto perspM = scene.getCamera()->getMatrix();

        glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)perspM.m_val);
        glUniform3fv(glGetUniformLocation(program->getId(), "camPos"), 1, (float*)(&scene.getCamera()->getPos()));
        glUniform3fv(glGetUniformLocation(program->getId(), "camUp"), 1, (float*)(&scene.getCamera()->getUp()));

        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDrawArrays(GL_POINTS, 0, P.size());
        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        avgFps = ((9*avgFps + 1/dt))/10;

        drawText(realToString(avgFps, 3), { 0.80, 0.90 }, 0.03, { 0, 0.8, 0 });
        interface.draw();
        cameraControl.update(dt);
        glfwSwapBuffers(window);
        frames++;

        scene.updateLights();

        for(auto program : scene.getShaderProgramManager()->getPrograms())
        {
            scene.setShaderProgram(program);
            program->use();

            int i = 0;
            for(auto light : scene.getLights())
            {
                auto loc = glGetUniformLocation(program->getId(), std::format("pointLights[{}].color", i).c_str());
                glUniform3fv(loc, 1, (GLfloat*)&(light->getColor()));
                loc = glGetUniformLocation(program->getId(), std::format("pointLights[{}].position", i).c_str());
                glUniform3fv(loc, 1, (GLfloat*)&(light->getPos()));
                i++;
            }
            glUniform1i(glGetUniformLocation(program->getId(), std::format("nLights", i).c_str()), scene.getLights().size());
        }
    }

    quitting = true;
    t.join();

    glfwTerminate();
    return 0;
}
