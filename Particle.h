#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include <iostream>
#include <random>

class Terrain;
class Input;
class Ray;

class Random
{
public:
    static Random& getInstance()
    {
        static Random instance;
        return instance;
    }

    std::default_random_engine& getGenerator()
    {
        return generator;
    }

private:
    Random() : generator(std::random_device()()) {}
    std::default_random_engine generator;

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
};


#pragma pack(push, 1)
struct SerializedParticle
{
    Vector3 pos;
    real size;
    Vector4 color;
};
#pragma pack(pop)


class Particle
{
public:
    Particle(const Vector3& pos, const Vector3& color);
    ~Particle();

    void setPos(const Vector3& pos);
    const Vector3& getPos() const;

    Vector3 getVelocity() const;
    void setVelocity(const Vector3& velocity);

    const Vector3& getColor() const;
    void setColor(const Vector3&);

    real getStart();

    virtual void update(real time) = 0;
    virtual bool isAlive() = 0;
    virtual bool isVisible() = 0;
    virtual SerializedParticle serialize() = 0;

protected:
    float getRandomFloat(float min, float max);
    int getRandomInt(int min, int max);

    Vector3 initialVelocity;
    Vector3 velocity;
    Vector3 startVelocity;
    Vector3 pos;
    Vector3 color;
    Vector4 startColor;
    real start;
    real time;
    real lifeTime;
};


class GunFireParticle : public Particle
{
public:
    GunFireParticle(Vector3 initialPos, Vector3 initialDir, Vector3 initialVelocity);

    void update(real time);
    bool isAlive();
    bool isVisible();
    SerializedParticle serialize();

private:
    bool smoke;
};


class GroundExplosionParticle : public Particle
{
public:
    GroundExplosionParticle(Vector3 initialPos, Vector3 normal);

    void update(real time);
    bool isAlive();
    bool isVisible();
    SerializedParticle serialize();

private:
    Vector3 normal;
    real size;
    enum Type { Explosion, Dust, Debris } type;
};


class UnitHitParticle : public Particle
{
public:
    UnitHitParticle(Vector3 initialPos, Vector3 normal);

    void update(real time);
    bool isAlive();
    bool isVisible();
    SerializedParticle serialize();

private:
    Vector3 normal;
    bool smoke;
};
