#define NOMINMAX

#include <cmath>
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <random>
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
#include <format>


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

    if (!glfwInit())
    {
        printf("failed to initialize GLFW.\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
     
    auto window = glfwCreateWindow(xres, yres, "PolRts", nullptr, nullptr);
    if (!window)
        return -1;
    glfwSetWindowAspectRatio(window, xres, yres);
    glfwMakeContextCurrent(window);
    if (gl3wInit())
        return -1;

    glfwSetFramebufferSizeCallback(window, sizeCallback);

    //debugDraw(window, xres, yres);
    //return 0;

    /*glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );*/

    int success;
    char infoLog[512];

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    InputQueue::getInstance().initInput(window);

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

    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            scene.addUnit(new Tank({ 155.5f+x, 85.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain));
        }
    }

    for(int y = 0; y < 5; y++)
    {
        for(int x = 0; x < 5; x++)
        {
            auto enemy = new Tank({ 170.5f+x, 85.15f+y, 3.07f }, { 1, 0, 0 }, { 0, 0, 1 }, 1, &terrain);
            enemy->setEnemy(true);
            scene.addUnit(enemy);
        }
    }

    /*for(int i = 0; i < 50; i++)
        scene.addParticle(new Particle(glfwGetTime(), Vector3(170.5, 85.15, 3.07f+i*0.1f), Vector3(dist(generator), dist(generator), dist(generator)) ));*/

    //PointLight* p = new PointLight();
    //p->setPos({ 170.5f, 85.15f, scene.getTerrain()->getElevation(170.5, 85.15) + 1.0f });
    //p->setColor({ 1, 0, 0 });
    //scene.addLight(p);

    for(auto& e : scene.getUnits())
        e->init(&scene);

    auto moveSlow = false;

    int mouseX, mouseY;

    checkError();

    UserInterface interface(window, &scene, &cameraControl);

    int frames = 0;
    real frameTime = 0;
    real avgFps = 0;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        terrain.draw();

        for(auto& unit : scene.getUnits())
        {
            unit->updateUniforms();
            unit->draw();
        }

        for(auto& entity : scene.getEntities())
        {
            entity->updateUniforms();
            entity->draw();
        }

        int i = 0;
        for(auto& unit : scene.getUnits())
        {
            glPolygonOffset(-1.0, -1.0*++i);
            unit->drawSelectionDecal(0);
        }

        glPolygonOffset(-1.0, -1.0);
        for(auto& unit : scene.getUnits())
        {
            unit->drawSelectionDecal(1);
        }

        interface.setResolution(xres, yres);
        cameraControl.setResolution(xres, yres);
        interface.draw();

        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;

        auto inputs = InputQueue::getInstance().handleInput(prevTime, time);
        glfwPollEvents();
        
        for(auto& unit : scene.getUnits())
            unit->update(dt);

        for(auto& entity : scene.getEntities())
            entity->update(dt);

        // TODO: this should be done in some update function or something
        for(auto& light : scene.getLights())
        {
            real t = glfwGetTime() - light->getStart();
            light->setColor(Vector3(0.2, 0.2, 0.12)*std::exp(-20.f*t));
            light->setPos(light->getPos() + light->getVelocity()*dt);
            if(t > 0.6)
                scene.removeLight(light);
        }

        scene.updateEntities();

        for(auto result = popPathFindingResult(); result; result = popPathFindingResult())
        {
            for(auto& unit : scene.getUnits())
            {
                if(unit->getCurrentPathfindingRequest() == result)
                {
                    unit->setCurrentPathfindingRequest(nullptr);
                    unit->setPath(result->path);
                }
            }
            delete result;
        }

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
            interface.handleInput(*input, scene.getUnits());

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

        std::vector<SerializedParticle> P;
        for(auto particle : scene.getParticles())
        {
            particle->update(dt);
            if(particle->isAlive())
                P.push_back(particle->serialize());
        }
        std::sort(P.begin(), P.end(), [&scene] (const auto p1, const auto& p2) { return (p1.pos-scene.getCamera()->getPos()).length2() > (p2.pos-scene.getCamera()->getPos()).length2(); });

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

        glDepthMask (GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glDrawArrays(GL_POINTS, 0, P.size());
        glDisable(GL_BLEND);
        glBindVertexArray(0);
        glDepthMask (GL_TRUE);

        avgFps = ((9*avgFps + 1/dt))/10;
        //std::cout << avgFps << std::endl;
        cameraControl.update(dt);
        glfwSwapBuffers(window);
        frames++;

        scene.clearUnits();
        scene.updateLights();
        for(auto program : scene.getShaderProgramManager()->getPrograms())
        {
            scene.setShaderProgram(program);
            program->use();

            int i = 0;
            for(auto light : scene.getLights())
            {
                auto loc = glGetUniformLocation(program->getId(), std::format("pointLights[{}].color", i).c_str());
                glUniform3fv(loc, 1, (GLfloat*) &(light->getColor()));
                loc = glGetUniformLocation(program->getId(), std::format("pointLights[{}].position", i).c_str());
                glUniform3fv(loc, 1, (GLfloat*) &(light->getPos()));
                i++;
            }
            glUniform1i(glGetUniformLocation(program->getId(), std::format("nLights", i).c_str()), scene.getLights().size());
        }

    }

    quitting = true;
    t.join();

    //for(auto& u : scene.getUnits())
    //    delete u;

    glfwTerminate();
    return 0;
}
