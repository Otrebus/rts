#define NOMINMAX

#include "LineMesh3d.h"
#include "Camera.h"
#include "Harvester.h"
#include "DebugDraw.h"
#include "Rock.h"
#include "TankWreck.h"
#include "Entity.h"
#include "Truck.h"
#include "InputManager.h"
#include "Line.h"
#include "Logger.h"
#include "UserInterface.h"
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
#include "TextureMaterial.h"
#include "UserInterface.h"
#include "Utils.h"
#include <cmath>
#include <format>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <random>
#include <stdio.h>
#include <thread>
#include "Input.h"
#include "ConsoleSettings.h"
#include "Vehicle.h"
#include "Font.h"
#include "Building.h"


extern bool quitting = false;

extern int xres = 1980, yres = 1080;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    xres = width;
    yres = height;
    glViewport(0, 0, width, height);
}


int main()
{
    if(!glfwInit())
    {
        printf("failed to initialize GLFW.\n");
        return -1;
    }
    
    std::default_random_engine generator;
    std::uniform_real_distribution<real> coord(50, 70);

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

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    glEnable(GL_DEPTH_TEST);

    InputManager::getInstance().initInput(window);

    PerspectiveCamera cam({ 0, 0, 100 }, { 0, 1, -1 }, { 0, 0, 1 }, 59, real(xres)/float(yres));

    real time = real(glfwGetTime());

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

    Tank::loadModels();
    TankWreck::loadModels();
    Truck::loadModels();
    Harvester::loadModels();
    Vehicle::loadModels();
    Rock::loadModels();

    //scene.addUnit(new Tank({ 180.480316, 99.7414932, 15.0 }, { 1, 0, 0 }, { 0, 0, 1}, &terrain));

    auto harvester = new Harvester({ 155.5f, 35.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    scene.addEntity(harvester);

    auto vehicle = new Truck({ 150.5f, 35.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    scene.addEntity(vehicle);

    //for(int y = 0; y < 5; y++)
    //{
    //    for(int x = 0; x < 5; x++)
    //    {
    //        scene.addEntity(new Tank({ 165.5f-x, 95.15f-y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain));
    //    }
    //}

    
    /*auto tank = new Tank({ 170.5f, 65.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    tank->constructing = true;
    scene.addEntity(tank);*/

    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            auto enemy = new Tank({ 170.5f+x, 75.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
            enemy->setEnemy(true);
            scene.addEntity(enemy);
        }
    }

    for(int y = 0; y < 3; y++)
    {
        for(int x = 0; x < 3; x++)
        {
            auto tank = new Tank({ 170.5f+x, 55.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
            scene.addEntity(tank);
        }
    }

    auto wreck = new TankWreck({ 167.5f, 85.15f, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
    wreck->init(&scene);
    scene.addEntity(wreck);

    for(int i = 0; i < 1000; i++) {
        auto rock = new Rock({ coord(generator), coord(generator), 0 }, { 1, 0, 0 }, { 0, 0, 1 }, &terrain);
        rock->init(&scene);
        scene.addEntity(rock);
    }
    
    scene.updateUnitList();
    for(auto& e : scene.getUnits())
        e->init(&scene);

    auto entities = scene.getEntities();
    for(auto& entity : entities)
        entity->plant(*scene.getTerrain());

    checkError();

    std::vector<Vector3> vs = { { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 1} };
    std::vector<std::pair<int, int>> vv = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

    UserInterface interface(window, &scene, &cameraControl);

    int frames = 0;
    auto prevFogOfWar = Terrain::fogOfWarEnabled.varInt();

    ShapeDrawer::setScene(&scene);
    ShapeDrawer::loadModels();

	glDepthRange(0.5, 1);
    
    for(int y = 0; y < terrain.getHeight(); y++)
    {
        for(int x = 0; x < terrain.getWidth(); x++)
        {
            terrain.setFog(x, y, 1);
        }
    }

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        auto prevTime = time;
        time = real(glfwGetTime());
        auto dt = time - prevTime;

        handleInput(prevTime, time, cameraControl, interface, scene, terrain, window);

        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        if(prevFogOfWar != Terrain::fogOfWarEnabled.varInt())
        {
            for(auto y = 0; y < terrain.getHeight(); y++)
            {
                for(auto x = 0; x < terrain.getWidth(); x++)
                {
                    terrain.setFog(x, y, Terrain::fogOfWarEnabled.varInt());
                }
            }
        }

        std::unordered_map<int, Unit*> fovUpdate;
        const auto fogR = 8;

        for(auto& unit : scene.getUnits())
        {
            if(unit->isEnemy())
                continue;
            auto pos = unit->getPosition();

            if(Terrain::fogOfWarEnabled.varInt())
            {
                for(auto x = std::max(0, int(pos.x) - fogR); x <= std::min(int(pos.x) + fogR, terrain.getWidth()-1); x++)
                {
                    for(auto y = std::max(0, int(pos.y) - fogR); y <= std::min(int(pos.y) + fogR, terrain.getHeight()-1); y++)
                    {
                        fovUpdate[y*terrain.getWidth() + x] = unit;
                    }
                }
            }
        }

        checkError();

        // TODO: what does setDead do again
        for(auto entity : scene.getEntities())
        {
            if(entity->dead)
            {
                scene.removeEntity(entity);
            }
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
            real t = real(glfwGetTime()) - light->getStart();
            light->setColor(Vector3(0.3f, 0.3f, 0.2f)*std::exp(-20.f*t));
            light->setPos(light->getPos() + light->getVelocity()*dt);
            if(light->getColor().length() < 1e-3f)
                scene.removeLight(light);
        }

        if(Terrain::fogOfWarEnabled.varInt())
        {
            for(auto p : fovUpdate)
            {
                auto check = [&] (Unit* unit, int x, int y)
                {
                    int dx = int(unit->getPosition().x) - x;
                    int dy = int(unit->getPosition().y) - y;
                    if(dx*dx + dy*dy < fogR*fogR && !unit->isEnemy())
                    {
                        terrain.setFog(x, y, false);
                        return true;
                    }
                    return false;
                };

                auto pos = p.first;
                int y = pos/terrain.getWidth();
                int x = pos%terrain.getWidth();

                terrain.setFog(x, y, 1);

                if(check(p.second, x, y))
                    continue;

                for(auto& unit : scene.getUnits())
                {
                    if(check(unit, x, y))
                        break;
                }
            }
        }

        terrain.updateAdmissiblePoints();

        terrain.draw();

        for(auto& entity : scene.getEntities())
        {
            entity->updateUniforms();
            entity->draw();
        }

        interface.setResolution(xres, yres);
        cameraControl.setResolution(xres, yres);

        std::vector<SerializedParticle> P;
        for(auto particle : scene.getParticles())
        {
            particle->update(dt);
            int x = int(particle->getPos().x), y = int(particle->getPos().y);
            if(particle->isAlive() && (!Terrain::fogOfWarEnabled.varInt() || !terrain.getFog(x, y)))
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
        glDrawArrays(GL_POINTS, 0, (GLsizei)P.size());
        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        interface.update(dt);
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
            glUniform1i(glGetUniformLocation(program->getId(), std::format("nLights", i).c_str()), int(scene.getLights().size()));
        }
        prevFogOfWar = Terrain::fogOfWarEnabled.varInt();
    
        checkError();
    }

    quitting = true;
    t.join();

    glfwTerminate();
    return 0;
}
