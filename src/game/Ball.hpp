#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

class Ball {
public:
    Ball(const glm::vec3& position, float radius, float mass);

    void update(float deltaTime);
    void applyImpulse(const glm::vec3& impulse);
    void applyFriction(float frictionCoeff, float deltaTime);

    bool isMoving(float threshold = 0.01f) const;

    const glm::vec3& getPosition() const;
    const glm::vec3& getVelocity() const;
    float getRadius() const;
    float getMass() const;

    void setPosition(const glm::vec3& pos);
    void setVelocity(const glm::vec3& vel);

private:

    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float mass;
};

///////////////////////////////////////////////////////////////////////////////////////////////

Ball::Ball(const glm::vec3& position, float radius, float mass)
    : position(position),
      velocity(0.0f),
      radius(radius),
      mass(mass) {}

void Ball::update(float deltaTime) {
    position += velocity * deltaTime;
}

void Ball::applyImpulse(const glm::vec3& impulse) {
    velocity += impulse / mass;
}

void Ball::applyFriction(float frictionCoeff, float deltaTime) {
    const float speedSquared = glm::length2(velocity);
    if (speedSquared <= 0.0f) return;

    const glm::vec3 frictionDir = -glm::normalize(velocity);
    const glm::vec3 friction = frictionDir * frictionCoeff * deltaTime;
    const float frictionSquared = glm::length2(friction);

    velocity = (frictionSquared > speedSquared) 
        ? glm::vec3(0.0f) 
        : velocity + friction;
}

bool Ball::isMoving(float threshold) const {
    return glm::length2(velocity) > threshold * threshold;
}

const glm::vec3& Ball::getPosition() const { return position; }
const glm::vec3& Ball::getVelocity() const { return velocity; }
float Ball::getRadius() const { return radius; }
float Ball::getMass() const { return mass; }

void Ball::setPosition(const glm::vec3& pos) { position = pos; }
void Ball::setVelocity(const glm::vec3& vel) { velocity = vel; }