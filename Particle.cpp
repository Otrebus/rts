#include "Particle.h"
#include "Input.h"
#include "Ray.h"
#include <random>


Particle::Particle(real start, const Vector3& pos, const Vector3& color) : start(start), pos(pos), color(color)
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

GunFireParticle::GunFireParticle(real time, Vector3 initialPos, Vector3 initialDir) : Particle(time, initialPos, initialDir)
{
    velocity = initialDir + Vector3(getRandomFloat(-1.0f, 1.0f), getRandomFloat(-1.0f, 1.0f), getRandomFloat(-1.0f, 1.0f));
}

void GunFireParticle::update(real dt)
{
    // Example update method that uses the random generator
    pos += velocity * dt;
    // Introduce some random change in velocity
    velocity += Vector3(getRandomFloat(-0.1f, 0.1f), getRandomFloat(-0.1f, 0.1f), getRandomFloat(-0.1f, 0.1f));
}

bool GunFireParticle::isAlive(real time)
{
    return time - start < 1.0;
}

bool GunFireParticle::isVisible(real time)
{
    return true;
}

SerializedParticle GunFireParticle::serialize()
{
    return SerializedParticle(pos, 0.01, Vector3(1.0, 1.0, 0));
}

float Particle::getRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}


int Particle::getRandomInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}
