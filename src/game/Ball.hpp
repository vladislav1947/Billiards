#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

class Ball
{
public:
    Ball(const glm::vec3 &position, float radius, float mass);

    void update(float deltaTime);
    void applyImpulse(const glm::vec3 &impulse);
    void applyFriction(float frictionCoeff, float deltaTime);

    bool isMoving(float threshold = 0.01f) const;

    const glm::vec3 &getPosition() const;
    const glm::vec3 &getVelocity() const;

    float getRadius() const;
    float getMass() const;

    void setPosition(const glm::vec3 &pos);
    void setVelocity(const glm::vec3 &vel);

    void applyAngularImpulse(const glm::vec3 &point, const glm::vec3 &impulse);

    glm::mat4 getRotationMatrix() const;

    const glm::quat &getRotation() const { return rotation; };

private:
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float mass;

    glm::quat rotation;        // Кватернион для вращения
    glm::vec3 angularVelocity; // Угловая скорость
};

Ball::Ball(const glm::vec3 &position, float radius, float mass)
    : position(position), velocity(0.0f), radius(radius), mass(mass),
      rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), // Инициализация единичным кватернионом
      angularVelocity(0.0f)
{
}

void Ball::update(float deltaTime)
{
    position += velocity * deltaTime;

    // Обновление вращения на основе угловой скорости
    if (glm::length(angularVelocity) > 0.01f)
    {
        float angle = glm::length(angularVelocity) * deltaTime;
        glm::vec3 axis = glm::normalize(angularVelocity);
        glm::quat deltaRot = glm::angleAxis(angle, axis);
        rotation = deltaRot * rotation;
    }

    // Также добавляем вращение от качения (если шар движется)
    if (glm::length(velocity) > 0.01f)
    {
        glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), velocity));
        float angle = glm::length(velocity) * deltaTime / radius;
        glm::quat deltaRot = glm::angleAxis(angle, axis);
        rotation = deltaRot * rotation;
    }

    // Замедление угловой скорости из-за трения
    angularVelocity *= (1.0f - 0.1f * deltaTime);
}

void Ball::applyImpulse(const glm::vec3 &impulse)
{
    velocity += impulse / mass;
}

void Ball::applyAngularImpulse(const glm::vec3 &point, const glm::vec3 &impulse)
{
    glm::vec3 r = point - position;
    glm::vec3 torque = glm::cross(r, impulse);
    angularVelocity += torque / (0.4f * mass * radius * radius); // Момент инерции для шара
}

void Ball::applyFriction(float frictionCoeff, float deltaTime)
{
    const float speedSquared = glm::length2(velocity);
    if (speedSquared <= 0.0f)
        return;

    const glm::vec3 frictionDir = -glm::normalize(velocity);
    const glm::vec3 friction = frictionDir * frictionCoeff * deltaTime;
    const float frictionSquared = glm::length2(friction);

    velocity = (frictionSquared > speedSquared)
                   ? glm::vec3(0.0f)
                   : velocity + friction;
}

bool Ball::isMoving(float threshold) const
{
    return glm::length2(velocity) > threshold * threshold;
}

glm::mat4 Ball::getRotationMatrix() const
{
    return static_cast<glm::mat4>(glm::mat4_cast(rotation));
}

const glm::vec3 &Ball::getPosition() const
{
    return position;
}

const glm::vec3 &Ball::getVelocity() const
{
    return velocity;
}

float Ball::getRadius() const
{
    return radius;
}

float Ball::getMass() const
{
    return mass;
}

void Ball::setPosition(const glm::vec3 &pos)
{
    position = pos;
}

void Ball::setVelocity(const glm::vec3 &vel)
{
    velocity = vel;
}