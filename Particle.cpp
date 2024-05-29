#include "Particle.h"
#include "Input.h"
#include "Ray.h"
#include <random>


Particle::Particle(const Vector3& pos, const Vector3& color) : pos(pos), color(color), time(0)
{
}


void Particle::setPos(const Vector3& pos)
{
    this->pos = pos;
}


const Vector3& Particle::getPos() const
{
    return pos;
}


Vector3 Particle::getVelocity() const
{
    return velocity;
}


void Particle::setVelocity(const Vector3& velocity)
{
    this->velocity = velocity;
}


const Vector3& Particle::getColor() const
{
    return color;
}

void Particle::setColor(const Vector3& color)
{
    this->color = color;
}

real Particle::getStart()
{
    return start;
}

float Particle::getRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}


int Particle::getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}

GunFireParticle::GunFireParticle(Vector3 initialPos, Vector3 initialDir, Vector3 initialVelocity) : Particle(initialPos, initialDir) // TODO: this sets initialdir to color
{
    auto right = (initialDir%Vector3(0, 0, 1)).normalized();
    auto up = initialDir%right;
    auto theta = getRandomFloat(0, 2*pi);
    start = getRandomFloat(0, 0.15);
    this->initialVelocity = initialVelocity;

    smoke = getRandomFloat(0, 1.0) > 0.5;

    if(!smoke)
    {
        lifeTime = 0.15;
        velocity = initialVelocity + 5.f*(initialDir*getRandomFloat(0.8, 1.0f) + getRandomFloat(0, 0.1)*(right*std::sin(theta) + up*std::cos(theta)));
    }
    else
    {
        real darkness = getRandomFloat(0.37, 0.5);
        startColor = Vector4(darkness, darkness, darkness, 1);
        start = getRandomFloat(0.00, 0.02f);
        lifeTime = 1.0;
        velocity = initialVelocity + getRandomFloat(0.4, 1.38)*(initialDir*getRandomFloat(1.3, 2.5) + getRandomFloat(0.5, 0.7)*(right*std::sin(theta) + up*std::cos(theta)));
    }
    startVelocity = velocity;
}

void GunFireParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
    else
        pos += initialVelocity*dt;
    if(smoke && time > start)
        velocity = startVelocity*std::max(std::exp(start-time), 0.0f);
}

bool GunFireParticle::isAlive()
{
    return time < start + lifeTime;
}

bool GunFireParticle::isVisible()
{
    return time > start;
}

SerializedParticle GunFireParticle::serialize()
{
    if(!smoke)
        return SerializedParticle(pos, 0.03+time*0.2, Vector4(1.0-(time-start)*1.5, 1.0-(time-start)*1.5, 0.7-(time-start)*1.5, 1-(time-start)/lifeTime));
    else
    {
        auto color = startColor;
        color.w = 1-(time-start)/lifeTime;
        return SerializedParticle(pos, 0.02+time*0.35, color);
    }
}

GroundExplosionParticle::GroundExplosionParticle(Vector3 initialPos, Vector3 normal) : Particle(initialPos, normal)
{
    auto right = (normal%Vector3(0, 1, 0)).normalized();
    auto forward = normal%right;
    auto theta = getRandomFloat(0, 2*pi);

    auto t = getRandomFloat(0, 1.0);
    if(t < 0.33)
        type = Explosion;
    else if(t < 0.66)
        type = Dust;
    else
        type = Debris;

    if(type == Explosion)
    {
        lifeTime = 0.15;
        velocity = 3.f*(normal*getRandomFloat(0.8, 1.0f) + getRandomFloat(0.4, 0.8)*(right*std::sin(theta) + forward*std::cos(theta)));
        start = getRandomFloat(0, 0.02f);
    }
    else if(type == Dust)
    {
        real darkness = getRandomFloat(0.17, 0.3);
        startColor = Vector4(darkness, darkness/1.5, 0, 1);
        start = getRandomFloat(0.00, 0.02f);
        lifeTime = 0.7;
        velocity = getRandomFloat(0.8, 1.78)*(normal*getRandomFloat(1.3, 4.5) + getRandomFloat(0.5, 1.3)*(right*std::sin(theta) + forward*std::cos(theta)));
    }
    else if(type == Debris)
    {
        real darkness = getRandomFloat(0.10, 0.15);
        startColor = Vector4(darkness, darkness/1.5, 0, 1);
        start = getRandomFloat(0.00, 0.02f);
        lifeTime = 0.7;
        size = getRandomFloat(0.02, 0.05);
        velocity = getRandomFloat(0.8, 1.78)*(normal*getRandomFloat(1.3, 4.5) + getRandomFloat(1.5, 2.3)*(right*std::sin(theta) + forward*std::cos(theta)));
    }
    startVelocity = velocity;
}

void GroundExplosionParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
    if(type == Dust && time > start)
        velocity = startVelocity*std::max(std::exp((start-time)*3), 0.0f);
    else if(type == Debris && time > start)
    {
        auto v = startVelocity*(std::max(std::exp((start-time)*2), 0.0f));
        velocity = v + Vector3(0, 0, -5)*time;
    }
}

bool GroundExplosionParticle::isAlive()
{
    return time < start + lifeTime;
}

bool GroundExplosionParticle::isVisible()
{
    return time > start;
}

SerializedParticle GroundExplosionParticle::serialize()
{
    if(type == Explosion)
        return SerializedParticle(pos, 0.03+time*0.2, Vector4(1.0-(time-start)*1.5, 1.0-(time-start)*1.5, 0.7-(time-start)*1.5, 1-(time-start)/lifeTime));
    else if(type == Dust)
    {
        auto color = startColor;
        color.w = 1-(time-start)/lifeTime;
        return SerializedParticle(pos, 0.02+(time-start)*0.65, color);
    }
    else if(type == Debris)
    {
        auto color = startColor;
        return SerializedParticle(pos, size, color);
    }
}

UnitHitParticle::UnitHitParticle(Vector3 initialPos, Vector3 normal) : Particle(initialPos, normal)
{
    auto right = (normal%Vector3(0, 1, 0)).normalized();
    auto forward = normal%right;
    auto theta = getRandomFloat(0, 2*pi);
    start = getRandomFloat(0, 0.05);

    smoke = getRandomFloat(0, 1.0) > 0.5;

    lifeTime = 0.15;
    velocity = 3.f*(normal*getRandomFloat(0.8, 1.0f) + getRandomFloat(0.8, 1.0)*(right*std::sin(theta) + forward*std::cos(theta)));
}

void UnitHitParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
}

bool UnitHitParticle::isAlive()
{
    return time < start + lifeTime;
}

bool UnitHitParticle::isVisible()
{
    return time > start;
}

SerializedParticle UnitHitParticle::serialize()
{
    return SerializedParticle(pos, 0.01, Vector4(1.0-(time-start)*1.5, 1.0-(time-start)*1.5, 0.7-(time-start)*1.5, 1-(time-start)/lifeTime));
}
