#include "Particle.h"
#include "Math.h"
#include "BoundingBox.h"
#include "Entity.h"


Particle::Particle(const Vector3& pos, const Vector3& color) : pos(pos), color(color), time(0)
{
}

Particle::~Particle()
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

float Particle::getRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}

int Particle::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(Random::getInstance().getGenerator());
}

GunFireParticle::GunFireParticle(Vector3 initialPos, Vector3 initialDir, Vector3 initialVelocity) : Particle(initialPos, initialDir) // TODO: this sets initialdir to color
{
    auto right = (initialDir%Vector3(0.f, 0.f, 1.f)).normalized();
    auto up = initialDir%right;
    auto theta = getRandomFloat(0, 2*pi);
    start = getRandomFloat(0.f, 0.15f);
    this->initialVelocity = initialVelocity;

    smoke = getRandomFloat(0.f, 1.0f) > 0.5f;

    if(!smoke)
    {
        lifeTime = 0.15f;
        velocity = initialVelocity + 5.f*(initialDir*getRandomFloat(0.8f, 1.0f) + getRandomFloat(0.f, 0.1f)*(right*std::sin(theta) + up*std::cos(theta)));
    }
    else
    {
        real darkness = getRandomFloat(0.37f, 0.5f);
        startColor = Vector4(darkness, darkness, darkness, 1);
        start = getRandomFloat(0.00f, 0.05f);
        lifeTime = 1.0f;
        velocity = initialVelocity + getRandomFloat(0.4f, 1.38f)*(initialDir*getRandomFloat(1.3f, 2.5f) + getRandomFloat(0.5f, 0.7f)*(right*std::sin(theta) + up*std::cos(theta)));
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
        velocity = startVelocity*std::max(std::exp(start-time), 0.f);
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
        return SerializedParticle(pos, 0.03f+time*0.2f, Vector4(1.0f-(time-start)*1.5f, 1.0f-(time-start)*1.5f, 0.7f-(time-start)*1.5f, 1.f-(time-start)/lifeTime));
    else
    {
        auto color = startColor;
        color.w = 1.f-(time-start)/lifeTime;
        return SerializedParticle(pos, 0.02f+time*0.35f, color);
    }
}

GroundExplosionParticle::GroundExplosionParticle(Vector3 initialPos, Vector3 normal) : Particle(initialPos, normal)
{
    auto right = (normal%Vector3(0.f, 1.f, 0.f)).normalized();
    auto forward = normal%right;
    auto theta = getRandomFloat(0.f, 2*pi);

    auto t = getRandomFloat(0, 1.0f);
    if(t < 0.33f)
        type = Explosion;
    else if(t < 0.66f)
        type = Dust;
    else
        type = Debris;

    if(type == Explosion)
    {
        lifeTime = 0.15f;
        velocity = 3.f*(normal*getRandomFloat(0.8f, 1.0f) + getRandomFloat(0.4f, 0.8f)*(right*std::sin(theta) + forward*std::cos(theta)));
        start = getRandomFloat(0.f, 0.02f);
    }
    else if(type == Dust)
    {
        real darkness = getRandomFloat(0.17f, 0.3f);
        startColor = Vector4(darkness, darkness/1.5f, 0.f, 1.f);
        start = getRandomFloat(0.f, 0.02f);
        lifeTime = 0.7f;
        velocity = getRandomFloat(0.8f, 1.78f)*(normal*getRandomFloat(1.3f, 4.5f) + getRandomFloat(0.5f, 1.3f)*(right*std::sin(theta) + forward*std::cos(theta)));
    }
    else if(type == Debris)
    {
        real darkness = getRandomFloat(0.10f, 0.15f);
        startColor = Vector4(darkness, darkness/1.5f, 0.f, 1.f);
        start = getRandomFloat(0.00f, 0.02f);
        lifeTime = 0.7f;
        size = getRandomFloat(0.02f, 0.05f);
        velocity = getRandomFloat(0.8f, 1.78f)*(normal*getRandomFloat(1.3f, 4.5f) + getRandomFloat(1.5f, 2.3f)*(right*std::sin(theta) + forward*std::cos(theta)));
    }
    startVelocity = velocity;
}

void GroundExplosionParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
    if(type == Dust && time > start)
        velocity = startVelocity*std::max(std::exp((start-time)*3), 0.f);
    else if(type == Debris && time > start)
    {
        auto v = startVelocity*(std::max(std::exp((start-time)*2), 0.f));
        velocity = v + Vector3(0.f, 0.f, -5.f)*time;
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
        return SerializedParticle(pos, 0.03f+time*0.2f, Vector4(1.0f-(time-start)*1.5f, 1.0f-(time-start)*1.5f, 0.7f-(time-start)*1.5f, 1.f-(time-start)/lifeTime));
    else if(type == Dust)
    {
        auto color = startColor;
        color.w = 1-(time-start)/lifeTime;
        return SerializedParticle(pos, 0.02f+(time-start)*0.65f, color);
    }
    else if(type == Debris)
    {
        auto color = startColor;
        return SerializedParticle(pos, size, color);
    }

    assert(false);
    return SerializedParticle(pos, size, startColor);
}

UnitHitParticle::UnitHitParticle(Vector3 initialPos, Vector3 normal) : Particle(initialPos, normal)
{
    auto right = (normal%Vector3(0.f, 1.f, 0.f));
    if(!right)
        right = (normal%Vector3(1.f, 1.f, 0.f));
    right.normalize();
    auto forward = normal%right;
    auto theta = getRandomFloat(0, 2*pi);
    start = getRandomFloat(0, 0.05f);

    smoke = getRandomFloat(0.f, 1.0f) > 0.5f;

    lifeTime = 0.15f;
    velocity = 3.f*(normal*getRandomFloat(0.8f, 1.0f) + getRandomFloat(0.8f, 1.0f)*(right*std::sin(theta) + forward*std::cos(theta)));
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
    return SerializedParticle(pos, 0.01f, Vector4(1.0f-(time-start)*1.5f, 1.0f-(time-start)*1.5f, 0.7f-(time-start)*1.5f, 1.f-(time-start)/lifeTime));
}

ConstructionParticle::ConstructionParticle(Vector3 nozzlePos, const Entity& target) : Particle(Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f))
{
    start = 0.f;
    auto boundingBox = target.boundingBox;
    auto targetUp = target.up;
    auto targetDir = target.dir;
    auto targetRight = target.dir % target.up;
    auto targetPos = target.pos;

    auto a = targetDir*(boundingBox.c2.x - boundingBox.c1.x)*getRandomFloat(0.f, 1.f);
    auto b = targetRight*(boundingBox.c2.y - boundingBox.c1.y)*getRandomFloat(0.f, 1.f);
    auto c = targetUp*(boundingBox.c2.z - boundingBox.c1.z)*getRandomFloat(0.f, 1.f);

    pos = targetPos + a + b + c;

    lifeTime = 0.3f;
    velocity = (nozzlePos - pos)/0.3f;
}

void ConstructionParticle::update(real dt)
{
    time += dt;
    if(time > start)
        pos += velocity*dt;
}

bool ConstructionParticle::isAlive()
{
    return time < start + lifeTime;
}

bool ConstructionParticle::isVisible()
{
    return time > start;
}

SerializedParticle ConstructionParticle::serialize()
{
    return SerializedParticle(pos, 0.02, Vector4(0, 0.8, 0, 1.0f));
}