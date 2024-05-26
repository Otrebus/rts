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

GunFireParticle::GunFireParticle(Vector3 initialPos, Vector3 initialDir, Vector3 initialVelocity) : Particle(initialPos, initialDir)
{
    auto right = (initialDir%Vector3(0, 0, 1)).normalized();
    auto up = initialDir%right;
    auto theta = getRandomFloat(0, 2*pi);
    start = getRandomFloat(0, 0.1);
    this->initialVelocity = initialVelocity;
    velocity = initialVelocity + 5.f*(initialDir*getRandomFloat(0.8, 1.0f) + getRandomFloat(0, 0.1)*(right*std::cos(theta) + up*std::cos(theta)));
}

void GunFireParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
    else
        pos += initialVelocity*dt;
}

bool GunFireParticle::isAlive()
{
    return time < 0.2;
}

bool GunFireParticle::isVisible()
{
    return time > start;
}

SerializedParticle GunFireParticle::serialize()
{
    return SerializedParticle(pos, 0.01+time*0.15, Vector4(1.0-(time-start)*1.5, 1.0-(time-start)*1.5, (time-start)*1.5, 1-(time-start)*5));
}

float Particle::getRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}


int Particle::getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}
