#pragma once

#define NOMINMAX

#include "Vector3.h"
#include <iostream>
#include <random>
#include "Matrix4.h"

class Terrain;
class Input;
class Ray;



class Random {
public:
    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    std::default_random_engine& getGenerator() {
        return generator;
    }

private:
    Random() : generator(std::random_device()()) {}
    std::default_random_engine generator;

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
};


struct SerializedParticle
{
    Vector3 pos;
    real size;
    Vector3 color;
};


class Particle
{
public:
    Particle(real start, const Vector3& pos, const Vector3& color);

    void setPos(const Vector3& pos);
    const Vector3& getPos() const;

    Vector3 getVelocity() const;
    void setVelocity(const Vector3& velocity);

    const Vector3& getColor() const;
    void setColor(const Vector3&);

    real getStart();
    
    virtual void update(real time) = 0;
    virtual bool isAlive(real time) = 0;
    virtual bool isVisible(real time) = 0;
    virtual SerializedParticle serialize() = 0;

protected:
    float getRandomFloat(float min, float max);
    int getRandomInt(int min, int max);

    Vector3 velocity;
    Vector3 pos;
    Vector3 color;
    real start;
};


class GunFireParticle : public Particle
{
public:
    GunFireParticle(real time, Vector3 initialPos, Vector3 initialDir);

    void update(real time);
    bool isAlive(real time);
    bool isVisible(real time);
    SerializedParticle serialize();
};
