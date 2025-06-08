#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <algorithm>

class Cue {
public:
    Cue();

    void rotate(float angleDeg);                     // Поворот кия
    void charge(float dt);                           // Зарядка удара
    glm::vec3 release();                             // Выпуск удара
    void adjustOffset(glm::vec2 delta);              // Смещение точки удара

    float getWidth() const;

    glm::vec3 getHitPoint(const glm::vec3& ballPosition, float ballRadius) const;
    glm::vec3 getCueStart(const glm::vec3& hitPoint) const;
    glm::vec3 getCueEnd(const glm::vec3& hitPoint) const;

    glm::vec3 getDirection() const;
    float getPower() const;
    glm::vec2 getOffset() const;
    glm::vec3 computeImpactPoint(const glm::vec3& origin, const std::vector<glm::vec3>& ballPositions, float ballRadius, float tableWidth, float tableHeight) const;

private:
    glm::vec3 direction;
    float power;
    glm::vec2 offset;

    const float maxPower = 1.0f;
    const float chargeRate = 0.5f;
    const float maxForce = 5.0f;
    const float baseLength = 1.0f;
    const float cueWidth = 0.03f;
};

inline Cue::Cue()
    : direction(glm::vec3(1.0f, 0.0f, 0.0f)), power(0.0f), offset(0.0f) {}

inline void Cue::rotate(float angleDeg) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0, 1, 0));
    direction = glm::normalize(glm::vec3(rot * glm::vec4(direction, 0.0f)));
}

inline void Cue::charge(float dt) {
    power += dt * chargeRate;
    power = std::min(power, maxPower);
}

inline glm::vec3 Cue::release() {
    glm::vec3 velocity = direction * (power * maxForce);
    power = 0.0f;
    return velocity;
}

inline void Cue::adjustOffset(glm::vec2 delta) {
    offset += delta;
    float limit = 0.8f;
    offset.x = std::clamp(offset.x, -limit, limit);
    offset.y = std::clamp(offset.y, -limit, limit);
}

float Cue::getWidth() const {
    return cueWidth;
}

inline glm::vec3 Cue::getHitPoint(const glm::vec3& ballPosition, float ballRadius) const {
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    glm::vec3 vertical = up;
    return ballPosition + offset.x * right * ballRadius + offset.y * vertical * ballRadius;
}

inline glm::vec3 Cue::getCueStart(const glm::vec3& hitPoint) const {
    // Начальная точка кия (дальняя от шара)
    // Увеличиваем расстояние в зависимости от силы удара для визуального эффекта
    float dynamicLength = baseLength + power * 1.0f; // При полной силе кий отодвигается на 1 единицу дальше
    return hitPoint - direction * dynamicLength;
}

inline glm::vec3 Cue::getCueEnd(const glm::vec3& hitPoint) const {
    // Конечная точка кия (ближе к шару)
    // Оставляем небольшой зазор между кием и шаром
    float gap = 0.2f; // 20см зазор
    return hitPoint - direction * gap;
}

inline glm::vec3 Cue::getDirection() const {
    return direction;
}

inline float Cue::getPower() const {
    return power;
}

inline glm::vec2 Cue::getOffset() const {
    return offset;
}

inline glm::vec3 Cue::computeImpactPoint(
    const glm::vec3& origin,
    const std::vector<glm::vec3>& ballPositions,
    float ballRadius,
    float tableWidth,
    float tableHeight) const
{
    glm::vec3 dir = glm::normalize(direction);
    float maxDistance = 100.0f; // дальность трассировки
    glm::vec3 hit = origin + dir * maxDistance;

    // Проверка столкновения с шарами
    for (const auto& pos : ballPositions) {
        if (glm::distance(pos, origin) < 1e-4f) continue; // Пропускаем биток

        glm::vec3 toBall = pos - origin;
        float proj = glm::dot(toBall, dir);
        if (proj > 0.0f && proj < maxDistance) {
            glm::vec3 closestPoint = origin + dir * proj;
            float dist = glm::length(pos - closestPoint);
            if (dist < ballRadius) {
                hit = closestPoint;
                maxDistance = proj;
            }
        }
    }

    // Проверка столкновений со стенками (простая рамка)
    float xMax = tableWidth / 2.0f - ballRadius;
    float zMax = tableHeight / 2.0f - ballRadius;

    // Расчёт пересечения с четырьмя плоскостями стола (X и Z)
    std::vector<float> tVals;

    if (dir.x != 0.0f) {
        float tx1 = (xMax - origin.x) / dir.x;
        float tx2 = (-xMax - origin.x) / dir.x;
        if (tx1 > 0) tVals.push_back(tx1);
        if (tx2 > 0) tVals.push_back(tx2);
    }
    if (dir.z != 0.0f) {
        float tz1 = (zMax - origin.z) / dir.z;
        float tz2 = (-zMax - origin.z) / dir.z;
        if (tz1 > 0) tVals.push_back(tz1);
        if (tz2 > 0) tVals.push_back(tz2);
    }

    for (float t : tVals) {
        glm::vec3 p = origin + dir * t;
        if (abs(p.x) <= xMax + 0.01f && abs(p.z) <= zMax + 0.01f) {
            if (t < maxDistance) {
                hit = p;
                maxDistance = t;
            }
        }
    }

    return hit;
}