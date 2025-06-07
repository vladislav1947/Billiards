#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Ball {
    glm::vec3 position;
    glm::vec3 velocity;
    float radius;
    float mass;

    Ball(const glm::vec3& pos, float r, float m)
        : position(pos), velocity(0.0f), radius(r), mass(m) {}
};

class Physics {
public:
    Physics(float tableWidth, float tableHeight, float friction);

    // Обновление состояния всех шаров за один кадр (dt — дельта времени)
    void Update(std::vector<Ball>& balls, float dt);

private:
    float tableWidth;
    float tableHeight;
    float friction; // коэффициент трения, замедляющий шары

    void ApplyFriction(Ball& ball, float dt);
    void HandleWallCollisions(Ball& ball);
    void HandleBallCollisions(Ball& ballA, Ball& ballB);
};

#include <cmath>
#include <iostream>

Physics::Physics(float tableWidth, float tableHeight, float friction)
    : tableWidth(tableWidth), tableHeight(tableHeight), friction(friction) {}

void Physics::Update(std::vector<Ball>& balls, float dt) {
    // Обновляем позиции шаров по скорости
    for (auto& ball : balls) {
        ball.position += ball.velocity * dt;

        // Отражение от границ стола (простая прямоугольная таблица на XZ плоскости)
        HandleWallCollisions(ball);

        // Трение замедляет шары
        ApplyFriction(ball, dt);
    }

    // Обработка столкновений шаров (каждая пара один раз)
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i + 1; j < balls.size(); ++j) {
            HandleBallCollisions(balls[i], balls[j]);
        }
    }
}

void Physics::ApplyFriction(Ball& ball, float dt) {
    // Простое линейное замедление скорости
    if (glm::length(ball.velocity) > 0.0001f) {
        glm::vec3 frictionForce = -glm::normalize(ball.velocity) * friction;
        ball.velocity += frictionForce * dt;

        // Не дать скорости стать отрицательной
        if (glm::dot(ball.velocity, ball.velocity) < 0.00001f) {
            ball.velocity = glm::vec3(0.0f);
        }
    }
}

void Physics::HandleWallCollisions(Ball& ball) {
    // Стол ограничен по X и Z (Y — вверх, шар лежит на Y=radius)
    float left = -tableWidth / 2.0f + ball.radius;
    float right = tableWidth / 2.0f - ball.radius;
    float top = tableHeight / 2.0f - ball.radius;
    float bottom = -tableHeight / 2.0f + ball.radius;

    if (ball.position.x < left) {
        ball.position.x = left;
        ball.velocity.x = -ball.velocity.x;
    } else if (ball.position.x > right) {
        ball.position.x = right;
        ball.velocity.x = -ball.velocity.x;
    }

    if (ball.position.z < bottom) {
        ball.position.z = bottom;
        ball.velocity.z = -ball.velocity.z;
    } else if (ball.position.z > top) {
        ball.position.z = top;
        ball.velocity.z = -ball.velocity.z;
    }
}

void Physics::HandleBallCollisions(Ball& ballA, Ball& ballB) {
    glm::vec3 delta = ballB.position - ballA.position;
    float dist = glm::length(delta);
    float penetration = ballA.radius + ballB.radius - dist;

    if (penetration > 0.0f && dist > 0.0f) {
        // Раздвигаем шары, чтобы не пересекались
        glm::vec3 collisionNormal = delta / dist;

        float totalMass = ballA.mass + ballB.mass;
        float correction = penetration / 2.0f;

        ballA.position -= collisionNormal * correction * (ballB.mass / totalMass);
        ballB.position += collisionNormal * correction * (ballA.mass / totalMass);

        // Скорости по формулам упругого столкновения
        glm::vec3 relativeVelocity = ballB.velocity - ballA.velocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

        if (velocityAlongNormal > 0)
            return; // уже разлетаются

        // Коэффициент восстановления (1 - полностью упругое)
        const float restitution = 0.9f;

        float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal;
        impulseMagnitude /= (1.0f / ballA.mass + 1.0f / ballB.mass);

        glm::vec3 impulse = impulseMagnitude * collisionNormal;

        ballA.velocity -= impulse / ballA.mass;
        ballB.velocity += impulse / ballB.mass;
    }
}