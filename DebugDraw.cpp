#define NOMINMAX

#include "Main.h"
#include "Camera.h"
#include <cmath>
#include "Entity.h"
#include "GeometryUtils.h"
#include "InputManager.h"
#include "LambertianMaterial.h"
#include "Bytestream.h"
#include "Line.h"
#include "Logger.h"
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
#include <set>
#include <ft2build.h>
#include <filesystem>
#include "Font.h"
#include "Buffer.h"
#include FT_FREETYPE_H


void drawLine(Scene& scene, Vector2 a, Vector2 b)
{
    Line2d line({ a, b });
    line.init(&scene);
    line.draw();
}

void drawArrow(Scene& scene, Vector2 pos, Vector2 dir, real len)
{
    auto a = pos, b = pos+dir.normalized()*len;
    auto c = a + (b-a)*0.85f + (a-b).perp()*0.15f;
    auto d = a + (b-a)*0.85f - (a-b).perp()*0.15f;

    Line2d line({ a, b, c });
    line.init(&scene);
    line.draw();

    Line2d line2({ d, b });
    line2.init(&scene);
    line2.draw();
}

void drawCircle(Scene& scene, Vector2 pos, real radius)
{
    int N = 50;
    std::vector<Vector2> points;
    for(int i = 0; i < N+1; i++)
        points.push_back(pos + Vector2(std::cos(i*(2*pi/N))*radius, std::sin(i*(2*pi/N))*radius));

    Line2d line(points);
    line.init(&scene);
    line.draw();
}

void drawArc(Scene& scene, Vector2 c, Vector2 p, real angle)
{
    int N = 50;
    auto v = p-c;
    real radius = v.length();

    real a1 = std::atan2(v.y, v.x);
    std::vector<Vector2> points;
    for(int i = 0; i < N+1; i++)
        points.push_back(c + Vector2(std::cos(a1 + i*(angle/N))*radius, std::sin(a1 + i*(angle/N))*radius));

    Line2d line(points);
    line.init(&scene);
    line.draw();
}

int drawCircleTriangle(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = real(glfwGetTime());

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    bool intersecting = false;
    int mouseX = 0, mouseY = 0;

    Ray r1, r2;

    //Vector2 p1(0.5, 0.5), p2(0.65, 0.75), p3(0.55, 0.85);

    Vector2 p1(0.3f, -0.3f), p2(0.f, 0.3f), p3(-0.3f, 0.f);

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto prevTime = time;
        time = real(glfwGetTime());

        auto inputs = InputManager::getInstance().handleInput(prevTime, time);
        glfwPollEvents();

        for(auto input : inputs)
        {
            if(input->stateStart == MousePosition)
                mouseX = int(input->posX), mouseY = int(input->posY);

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
                drawCircle(scene, r1.pos.to2() + (r2.pos-r1.pos).to2().normalized()*s, radius);
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
        {{-0.15f, -0.15f, 0.0f}, {0.0f, 0.0f, 1.0f}, {-0.5f, -0.5f}},
        {{ 0.15f, -0.15f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, -0.5f}},
        {{ 0.15f,  0.15f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}},
        {{-0.15f,  0.15f, 0.0f}, {0.0f, 0.0f, 1.0f}, {-0.5f, 0.5f}}
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

    real time = real(glfwGetTime());

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

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


        real x = ((rand()%2000) - 1000.0f)/10000.0f;
        real y = ((rand()%2000) - 1000.0f)/10000.0f;

        position[i] = { x, y, 0 };

        real vx = ((rand()%2000) - 1000.0f)/10000.0f;
        real vy = ((rand()%2000) - 1000.0f)/10000.0f;

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

        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.updateUniforms();
        mesh.draw();

        auto prevTime = time;
        time = real(glfwGetTime());
        auto dt = time - prevTime;

        checkError();

        SelectionDecalMaterial::radiusA = 0.15f;
        int i = 1;

        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ONE, GL_ZERO);

        for(auto& decal : decals)
        {
            glPolygonOffset(-1.0f, -1.0f*++i);
            decal->updateUniforms();
            decal->draw();
        }

        glEnable(GL_SAMPLE_SHADING);
        glMinSampleShading(1.0f);

        glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
        i = 0;

        SelectionDecalMaterial::radiusA = 0.17f;
        for(auto& decal : decals)
        {
            glPolygonOffset(-1.0f, -1.0f);
            decal->updateUniforms();
            decal->draw();
        }

        for(int i = 0; i < 10; i++)
        {
            if(position[i].y > 0.2)
            {
                position[i].y = 0.2f;
                velocity[i].y = -velocity[i].y;
            }
            if(position[i].y < -0.2f)
            {
                position[i].y = -0.2f;
                velocity[i].y = -velocity[i].y;
            }
            if(position[i].x > 0.2f)
            {
                position[i].x = 0.2f;
                velocity[i].x = -velocity[i].x;
            }
            if(position[i].x < -0.2f)
            {
                position[i].x = -0.2f;
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
                mouseX = int(input->posX), mouseY = int(input->posY);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


int drawSigned(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));

    real time = real(glfwGetTime());

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    Font font(scene, "OpenSans-Regular.ttf");

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        auto prevTime = time;
        time = real(glfwGetTime());
        
        glClearColor(0.5f, 0.5f, 0.5f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
        real size = 0.2f;
        real y = 1;
        for(int i = 0; i < 15; i++)
        {
            font.draw(scene, "This, is me. Typing something (I wonder if this will look good :)).", { -1, y }, size);
            y -= size;
            size -= 0.2f/15;
        }


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


int drawVehiclePaths(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));
    
    real time = real(glfwGetTime());

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    int mouseX, mouseY;

    Ray r1, r2;
    mouseX = mouseY = 0;

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto prevTime = time;
        time = real(glfwGetTime());

        auto inputs = InputManager::getInstance().handleInput(prevTime, time);
        glfwPollEvents();

        for(auto input : inputs)
        {
            if(input->stateStart == MousePosition)
            {
                mouseX = int(input->posX);
                mouseY = int(input->posY);
            }
            delete input;
        }

        // TODO: make a mouseCoordtoWorldCoord as well
        auto mousePos = mouseCoordToScreenCoord(::xres, ::yres, mouseX, mouseY);
        auto [orig, ray] = scene.getCamera()->getViewRay(mousePos.x, mousePos.y);

        Vector2 dest = orig.to2();

        Vector2 pos(0.25f, 0.25f);
        auto dir = Vector2(0.5f, 0.5f).normalized();

        real R = 0.10f;

        auto c_l = pos + dir.perp()*R;
        auto c_r = pos - dir.perp()*R;

        real reverseCost = 1.5;

        auto leftHand = [&] (bool draw = false) {
            // Case 1: left hand turn

            auto [a, b] = getTangents(c_l, R, dest);

            Vector2 v_t = (c_l - dest) % (a - dest) > 0 ? a : b;

            auto A = (pos - c_l).normalized(), B = (v_t - c_l).normalized();

            auto angle = std::acos(A*B);
            if(A%B < 0)
                angle = 2*pi - angle;
         
            if(draw) {
                drawArc(scene, c_l, pos, angle);
                drawLine(scene, v_t, dest);
            }
            return angle*R + (v_t - dest).length();
        };

        auto rightHand = [&] (bool draw = false) {
            // Case 1: right hand turn
            auto p = getTangents(c_r, R, dest);
            auto a = p.first;
            auto b = p.second;

            auto v_t = (c_r - dest) % (a - dest) > 0 ? b : a;

            auto A = (pos - c_r).normalized();
            auto B = (v_t - c_r).normalized();

            auto angle = std::acos(A*B);
            if(A%B > 0)
                angle = 2*pi - angle;

            if(draw) {
                drawArc(scene, c_r, pos, -angle);
                drawLine(scene, v_t, dest);
            }

            return angle*R + (v_t - dest).length();
        };

        ////////// Case 2: right two point turn
        auto rightTwoPoint = [&] (bool draw = false) {
            auto v = (dest - c_l).normalized();
            auto P = c_l + v*R;

            auto P_r = P + v*R;

            auto p = getTangents(P_r, R, dest);
            auto a = p.first;
            auto b = p.second;

            auto v_t = (P_r - dest) % (a - dest) > 0 ? b : a;

            auto A = (pos-c_l).normalized(), B = (P-c_l).normalized();
            auto angle = std::acos(A*B);
            if(A%B > 0)
                angle = 2*pi - angle;

            real ret = 0;
            if(draw)
                drawArc(scene, c_l, pos, -angle);
            ret += reverseCost*angle*R;

            A = (v_t-P_r).normalized(), B = (P-P_r).normalized();
            angle = std::acos(A*B);
            if(A%B < 0)
                angle = 2*pi - angle;

            if(draw) {
                drawArc(scene, P_r, v_t, angle);
                drawLine(scene, v_t, dest);
            }

            return ret + angle*R + (v_t - dest).length();
        };

        //////////////////// Case 2: left two point turn
        auto leftTwoPoint = [&] (bool draw = false) {
            auto v = (dest - c_r).normalized();
            auto P = c_r + v*R;

            auto P_l = P + v*R;

            auto p = getTangents(P_l, R, dest);
            auto a = p.first;
            auto b = p.second;

            auto v_t = (P_l - dest) % (a - dest) > 0 ? a : b;

            auto A = (pos-c_r).normalized(), B = (P-c_r).normalized();
            auto angle = std::acos(A*B);
            if(A%B < 0)
                angle = 2*pi - angle;

            if(draw)
                drawArc(scene, c_r, pos, angle);

            real ret = reverseCost*angle*R;

            A = (v_t-P_l).normalized(), B = (P-P_l).normalized();
            angle = std::acos(A*B);
            if(A%B > 0)
                angle = 2*pi - angle;

            if(draw) {
                drawArc(scene, P_l, v_t, -angle);
                drawLine(scene, v_t, dest);
            }

            return ret + angle*R + (v_t - dest).length();
        };

        /////////////////////// Case 3: reverse left turn
        auto leftReverse = [&] (bool draw = false) {
            auto p = getTangents(c_l, R, dest);
            auto a = p.first;
            auto b = p.second;

            auto v_t = (c_l - dest) % (a - dest) < 0 ? a : b;

            auto A = (pos - c_l).normalized();
            auto B = (v_t - c_l).normalized();

            auto angle = std::acos(A*B);
            if(A%B > 0)
                angle = 2*pi - angle;

            auto D = angle*R + (v_t - dest).length();

            if(draw) {
                drawArc(scene, c_l, v_t, angle);
                drawLine(scene, v_t, dest);
            }
            return reverseCost*D;
        };

        ///////////////////// Case 3: reverse right turn
        auto rightReverse = [&] (bool draw = false) {
            auto p = getTangents(c_r, R, dest);
            auto a = p.first;
            auto b = p.second;

            auto v_t = (c_r - dest) % (a - dest) < 0 ? b : a;

            auto A = (pos - c_r).normalized();
            auto B = (v_t - c_r).normalized();

            auto angle = std::acos(A*B);
            if(A%B < 0)
                angle = 2*pi - angle;

            auto D_2 = angle*R + (v_t - dest).length();

            if(draw) {
                drawArc(scene, c_r, v_t, -angle);
                drawLine(scene, v_t, dest);
            }
            return reverseCost*D_2;
        };

        auto lh = leftHand();
        auto rh = rightHand();
        auto rtp = rightTwoPoint();
        auto ltp = leftTwoPoint();
        auto lr = leftReverse();
        auto rr = rightReverse();
        real m = min(lh, rh, rtp, ltp, lr, rr);

        if(m == lh)
            leftHand(true);
        if(m == rh)
            rightHand(true);
        if(m == rtp)
            rightTwoPoint(true);
        if(m == ltp)
            leftTwoPoint(true);
        if(m == lr)
            leftReverse(true);
        if(m == rr)
            rightReverse(true);

        //(pos - c_l) % (

        /*drawCircle(scene, c_l, R);

        auto [a, b] = getTangents(c_l, R, dest);
        drawCircle(scene, a, 0.02);
        drawCircle(scene, b, 0.02);*/

        //drawArrow(scene, Vector2(0.2, 0.2), Vector2(0.2, 0.2), 0.15);

        /*Vector2 pos(0.25, 0.25);

        auto dest = Vector2(x, y);

        auto line = makeCircle(b, r2);
        line.init(&scene);
        line.draw();

        auto line2d = makeCircle2d(b, r2);
        line2d.init(&scene);
        line2d.draw();

        auto v = (a - b);
        auto d = v.length();

        auto L = std::sqrt(d*d - r2*r2);

        auto [p1, p2] = intersectCircleCircle(a, L, b, r2);

        auto lineA = Line3d( { p1.to3(), a.to3() } );
        auto lineB = Line3d( { p2.to3(), a.to3() } );

        lineA.init(&scene);
        lineB.init(&scene);

        lineA.draw();
        lineB.draw();

        drawArc(scene, Vector2(-0.25, 0.25), Vector2(-0.35, 0.30), -3.1415);
       */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

real getTimeToMove(real v, real a_f, real a_r, real maxV, real d)
{
    real ret = 0;
    if(v < 0)
    {
        auto t = -v/a_r;
        ret += t;
        d += -a_r*t*t/2 - v*t;
        v = 0;
    }

    auto tm = (maxV - v)/a_f;
    auto t = -v/a_f + std::sqrt((v*v)/(a_f*a_f) + 2*d/a_f);

    if(t < tm)
    {
        return ret + t;
    }
    else
    {
        d -= a_f*tm*tm/2 + v*tm;
        ret += tm;
        v = maxV;
        return ret + t + d/maxV;
    }
}

int drawAccelerationPath(GLFWwindow* window, int xres, int yres)
{
    InputManager::getInstance().initInput(window);
    OrthogonalCamera cam({ 0, 0, 1 }, { 0, 0, -1 }, { 0, 1, 0 }, real(xres)/float(yres));
    
    real time = real(glfwGetTime());

    ShaderProgramManager shaderProgramManager;
    Scene scene(&cam, &shaderProgramManager);

    int mouseX, mouseY;

    Ray r1, r2;
    mouseX = mouseY = 0;

    while(!glfwWindowShouldClose(window))
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto prevTime = time;
        time = real(glfwGetTime());

        auto inputs = InputManager::getInstance().handleInput(prevTime, time);
        glfwPollEvents();

        for(auto input : inputs)
        {
            if(input->stateStart == MousePosition)
            {
                mouseX = int(input->posX);
                mouseY = int(input->posY);
            }
            delete input;
        }

        auto mousePos = mouseCoordToScreenCoord(::xres, ::yres, mouseX, mouseY);
        auto [orig, ray] = scene.getCamera()->getViewRay(mousePos.x, mousePos.y);

        Vector2 dest = Vector2(0, 1.0f);

        real R = 0.25f;

        Vector2 pos = orig.to2().normalized()*R;

        real v = 0.5f;
        real maxV = 3.0;

        real a_f = 2.0;
        real a_r = 2.0;
        real a_b = 13.0;

        auto dir = pos.perp().normalized();

        std::cout << orig << std::endl;

        drawArrow(scene, pos, dir, 0.03);

        auto c_l = pos + dir.perp()*R;
        
        auto C = dir%dest > 0 ? pos + dir.perp()*R : pos -dir.perp()*R;

        auto pf = C - dest.perp().normalized()*R;
        auto pb = C + dest.perp().normalized()*R;
        
        auto th_f = std::acos((pf-C).normalized()*(pos-C).normalized());
        auto th_b = -(pi - std::acos((pf-C).normalized()*(pos-C).normalized()));

        if(dir%dest < 0)
        {
            std::swap(th_f, th_b);
        }

        auto d1 = std::abs(th_f*R);
        auto d2 = std::abs(th_b*R);

        auto t1 = getTimeToMove(v, a_f, a_b, maxV, d1);
        auto t2 = getTimeToMove(-v, a_r, a_b, maxV, d2);

        if(t1 < t2)
            drawArc(scene, C, pos, th_f);
        else
            drawArc(scene, C, pos, th_b);
        std::cout << (dir%dest > 0 ? "left" : "right") << std::endl;
        
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

int debugDraw(GLFWwindow* window, int xres, int yres)
{
    // return drawCircleTriangle(window, xres, yres);
    // return drawDecals(window, xres, yres);
    // return drawSigned(window, xres, yres);
    //return drawVehiclePaths(window, xres, yres);

    return drawAccelerationPath(window, xres, yres);
}
