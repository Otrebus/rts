#define NOMINMAX

#include "Camera.h"
#include "Entity.h"
#include "GeometryUtils.h"
#include "InputManager.h"
#include "LambertianMaterial.h"
#include "Line.h"
#include "Logger.h"
#include "Main.h"
#include "Math.h"
#include "Matrix4.h"
#include "Model3d.h"
#include "Parser.h"
#include "Ray.h"
#include "Scene.h"
#include "SelectionDecalMaterial.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ShaderProgramManager.h"
#include "Tank.h"
#include "Terrain.h"
#include "Text.h"
#include "TextureMaterial.h"
#include "UserInterface.h"
#include "Utils.h"
#include "Vector2.h"
#include <cmath>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <thread>

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
    InputManager::getInstance().initInput(window);
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

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;

        auto inputs = InputManager::getInstance().handleInput(prevTime, time);
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

        Line3d tri({ p1.to3(), p2.to3(), p3.to3(), p1.to3() });
        tri.init(&scene);
        tri.draw();

        if(intersecting)
        {
            auto [x, y] = mouseCoordToScreenCoord(xres, yres, mouseX, mouseY);
            r2 = cam.getViewRay(x, y);

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
            Line3d normalLine({ p1, p2 });
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

std::vector<Vertex3> createQuadVertices()
{
    std::vector<Vertex3> vs = {
        {{-0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {-0.5, -0.5}},
        {{ 0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {0.5, -0.5}},
        {{ 0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {0.5, 0.5}},
        {{-0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {-0.5, 0.5}}
    };

    return vs;
}

std::vector<int> quadIndices()
{
    return { 0, 1, 2, 2, 3, 0 };
}

int drawDecals(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = glfwGetTime();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

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

    for(int i = 0; i < 10; ++i)
    {
        auto mesh = new Mesh3d(createQuadVertices(), quadIndices(), decalMaterial);
        Model3d* decal = new Model3d(*mesh);
        decal->setScene(&scene);
        decals.push_back(decal);


        real x = ((rand()%2000) - 1000.0)/10000.0;
        real y = ((rand()%2000) - 1000.0)/10000.0;

        position[i] = { x, y, 0 };

        real vx = ((rand()%2000) - 1000.0)/10000.0;
        real vy = ((rand()%2000) - 1000.0)/10000.0;

        velocity[i] = { vx, vy, 0 };
    }

    std::vector<Vertex3> meshVertices = {
        { -0.5f, -0.5f, 0.0, 0, 0, -1, 0, 0 },
        { 0.5f, -0.5f, 0.0, 0, 0, -1, 1, 0, },
        { 0.5f,  0.5f, 0.0, 0, 0, -1, 1, 1, },
        { -0.5f, 0.5f, 0.0, 0, 0, -1, 0, 1, }
    };
    TextureMaterial texture("grass.bmp");
    Mesh3d mesh(meshVertices, { 0, 1, 2, 2, 3, 0 }, &texture);
    mesh.setScene(&scene);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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

        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(1.0);

        glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
        i = 0;

        SelectionDecalMaterial::radius = 0.17;
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

        auto inputs = InputManager::getInstance().handleInput(prevTime, time);
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
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
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


std::vector<bool> calcSigned(std::vector<unsigned char> v, int xres, int yres)
{
    std::vector<bool> data(xres*yres);
    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres*3; x += 3)
        {
            data[y*xres+x] = v[y*xres*3+x];
        }
    }

    // timus 1976 calculates this exactly and faster
    for(int y = 0; y < yres; y++)
    {
        for(int x = 0; x < xres; x++)
        {
            
        }
    }

    return data;
}


int drawSigned(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = glfwGetTime();

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    GLuint VAO, VBO, EBO;

    GLuint texture;

    glGenTextures(1, &texture);
    auto [data, width, height] = readBMP("signsource.bmp");

    std::vector<Vertex3> vs = {
        {{-0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {0, 0}},
        {{ 0.15, -0.15, 0.0}, {0.0, 0.0, 1.0}, {1, 0}},
        {{ 0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {1, 1}},
        {{-0.15,  0.15, 0.0}, {0.0, 0.0, 1.0}, {0, 1}}
    };

    std::vector<int> indices = { 0, 1, 2, 2, 3, 0 };

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3)*vs.size(), vs.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);

    Shader* geometryShader = nullptr, *vertexShader = nullptr, *fragmentShader;

    geometryShader = new Shader("geometryShader.geom", GL_GEOMETRY_SHADER);
    vertexShader = new Shader("vertexShader.vert", GL_VERTEX_SHADER);
    fragmentShader = new Shader("texture.frag", GL_FRAGMENT_SHADER);

    auto s = scene.getShaderProgramManager();
    auto program = s->getProgram(fragmentShader, geometryShader, vertexShader);
    scene.setShaderProgram(program);

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        auto prevTime = time;
        time = glfwGetTime();
        auto dt = time - prevTime;
        
        glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
        (new LambertianMaterial)->updateUniforms(&scene);

        glUniformMatrix4fv(glGetUniformLocation(program->getId(), "modelViewMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
        glUniformMatrix4fv(glGetUniformLocation(program->getId(), "projectionMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);
        glUniformMatrix4fv(glGetUniformLocation(program->getId(), "normalMatrix"), 1, GL_TRUE, (float*)identityMatrix.m_val);

        program->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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
    // return drawTexts(window, xres, yres);
    return drawSigned(window, xres, yres);
}
