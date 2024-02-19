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

void checkError() {
    GLenum error;
    error = glGetError();

    if (error != GL_NO_ERROR)
        std::cout << "Not working" << std::endl;
    if (error == GL_INVALID_OPERATION)
        std::cout << "Invalid operation" << std::endl;
    if (error == GL_NO_ERROR)
        std::cout << "No error" << std::endl;
    if (error == GL_INVALID_VALUE)
        std::cout << "No value" << std::endl;
}


int xres = 1000, yres = 600;

void sizeCallback(GLFWwindow* window, int width, int height)
{
    xres = width;
    yres = height;
    glViewport(0, 0, width, height);
}

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
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

    auto model = Model3d("CornellBox-Original.obj");

    TextureMaterial texture("wall.bmp");
    TextureMaterial texture2("grass.bmp");

    int success;
    char infoLog[512];

    glEnable(GL_DEPTH_TEST);

    auto startTime = glfwGetTime();

    //glEnable              ( GL_DEBUG_OUTPUT );
    //glDebugMessageCallback( MessageCallback, 0 );

    initInput(window);

    Camera cam({ 0, 0, 4 }, { 0, 1, -1 }, { 0, 0, 1 }, 59, real(xres)/float(yres));
    // Camera cam({ 0, -1, 0 }, { 0, 1, 0 },  { 0, 0, 1 }, 59, real(xres)/float(yres));

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

    for(int x = 0; x < 10; x++)
    {
        for(int y = 0; y < 10; y++)
        {
            for(int z = 0; z < 10; z++)
            {
                entities.push_back(new Entity({ 0.5f+x*0.1f, 0.5f+y*0.1f, 3.07f+z*0.1f }, { 1, 0, 0 }, { 0, 0, 1 }));
            }
        }
    }
    for(auto& e : entities)
    {
        e->setUp(&scene);
    }

    model.setUp(&scene);
    mesh.setUp(&scene);
    mesh2.setUp(&scene);
    line.setUp(&scene);
    //entity.setUp(&scene);

    Line3d line1({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line2({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line3({ { 0, 0, 0 }, { 0, 1, 1 } });
    Line3d line4({ { 0, 0, 0 }, { 0, 1, 1 } });
    line1.setUp(&scene);
    line2.setUp(&scene);
    line3.setUp(&scene);
    line4.setUp(&scene);

    Terrain terrain("Heightmap.bmp", &scene);
    CameraControl cameraControl(&cam, &terrain, true);

    auto moveSlow = false;

    int mouseX, mouseY;

    checkError();

    UserInterface interface(&scene);

    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
        //entity.updateUniforms();
        //entity.drawBoundingBox();
        for(auto& entity : entities)
        {
            entity->updateUniforms();
            entity->drawBoundingBox();
        }

        /*mesh.draw();

        mesh2.draw();*/

        model.draw();

        terrain.draw();
        
        // checkError();
        interface.setResolution(xres, yres);
        interface.draw();

        line.draw();

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

        auto inputs = handleInput(window, prevTime, time, cameraControl, terrain);
        glfwPollEvents();

        //entity.pos += Vector3(0.1, 0, 0)*dt;
        auto a = dt*1;

        //entity.dir *= Matrix4(
        //    std::cos(a), -std::sin(a), 0, 0,
        //    std::sin(a), std::cos(a), 0, 0,
        //    0, 0, 0, 0,
        //    0, 0, 0, 0
        //);


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
            {
                cameraControl.handleInput(*input);
            }
            interface.handleInput(*input, entities);

            if(input->stateStart == MousePosition)
            {
                mouseX = input->posX;
                mouseY = input->posY;
            }

            //if(input->stateStart == InputType::MousePress && input->key == GLFW_MOUSE_BUTTON_3)
            //{
            //    Vector3 dir = getViewRay(cam, resToScreenX(mouseX, xres), resToScreenY(mouseY, yres));
            //    dir.normalize();
            //    line = Line3d({ cam.pos, cam.pos+dir });
            //    line.setUp(&scene);

            //    terrain.intersect(Ray(cam.pos, dir));
            //    //entity.intersectBoundingBox(Ray(cam.pos, dir));
            //}

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
        glfwSwapBuffers(window);
    }

    for(auto& e : entities)
    {
        delete e;
    }

    model.tearDown(&scene);
    mesh.tearDown(&scene);
    mesh2.tearDown(&scene);

    glfwTerminate();
    return 0;
}
