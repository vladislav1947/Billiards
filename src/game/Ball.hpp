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

inline Ball::Ball(const glm::vec3& position, float radius, float mass)
    : position(position), velocity(0.0f), radius(radius), mass(mass) {}

inline void Ball::update(float deltaTime) {
    position += velocity * deltaTime;
}

inline void Ball::applyImpulse(const glm::vec3& impulse) {
    velocity += impulse / mass;
}

inline void Ball::applyFriction(float frictionCoeff, float deltaTime) {
    if (glm::length2(velocity) > 0.0f) {
        glm::vec3 friction = -glm::normalize(velocity) * frictionCoeff * deltaTime;
        if (glm::length2(friction) > glm::length2(velocity)) {
            velocity = glm::vec3(0.0f);
        } else {
            velocity += friction;
        }
    }
}

inline bool Ball::isMoving(float threshold) const {
    return glm::length2(velocity) > threshold * threshold;
}

inline const glm::vec3& Ball::getPosition() const {
    return position;
}

inline const glm::vec3& Ball::getVelocity() const {
    return velocity;
}

inline float Ball::getRadius() const {
    return radius;
}

inline float Ball::getMass() const {
    return mass;
}

inline void Ball::setPosition(const glm::vec3& pos) {
    position = pos;
}

inline void Ball::setVelocity(const glm::vec3& vel) {
    velocity = vel;
}