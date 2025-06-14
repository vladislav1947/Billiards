#pragma once

#include <render/Renderer.hpp>
#include <glm/glm.hpp>
#include <vector>

class Scene
{
public:
    Scene();
    void Render(Renderer &renderer,
                const glm::mat4 &view,
                const glm::mat4 &projection,
                const glm::vec3 &cameraPos);

private:
    glm::vec2 tableSize;
    glm::vec3 tableColor;
    glm::vec3 floorColor;
    glm::vec3 skyColor;
    glm::vec3 wallColor;
    float wallHeight;
    float wallThickness;
    float pocketRadius;
    std::vector<glm::vec3> pocketPositions;
    float legWidth;     // Ширина ножки
    float legHeight;    // Высота ножки
    glm::vec3 legColor; // Цвет ножек

    void drawSkyBackground();
    void drawFloor(Renderer &renderer,
                   const glm::mat4 &view,
                   const glm::mat4 &projection);
    void drawTable(Renderer &renderer,
                   const glm::mat4 &view,
                   const glm::mat4 &projection);
};

inline Scene::Scene()
    : tableSize(2.0f, 1.0f),
      tableColor(0.0f, 0.3f, 0.0f),
      floorColor(0.2f, 0.15f, 0.1f),
      skyColor(0.5f, 0.7f, 1.0f),
      wallColor(0.3f, 0.2f, 0.1f),
      wallHeight(0.1f),
      wallThickness(0.05f),
      pocketRadius(0.08f),
      legWidth(0.08f),             // Ширина ножки (8 см)
      legHeight(0.6f),             // Высота ножки (60 см)
      legColor(0.3f, 0.15f, 0.05f) // Цвет ножек (коричневый)
{
    pocketPositions = {
        {-0.95f, 0.01f, -0.45f},
        {-0.95f, 0.01f, 0.45f},
        {0.95f, 0.01f, -0.45f},
        {0.95f, 0.01f, 0.45f},
        {0.0f, 0.01f, -0.45f},
        {0.0f, 0.01f, 0.45f}};
}

inline void Scene::Render(Renderer &renderer,
                          const glm::mat4 &view,
                          const glm::mat4 &projection,
                          const glm::vec3 &cameraPos)
{
    // Передаём позицию камеры в renderer (если нужно для расчётов света и т.п.)
    renderer.SetCameraPos(cameraPos);

    // Рисуем фон (небо)
    drawSkyBackground();

    // Рисуем пол
    drawFloor(renderer, view, projection);

    // Рисуем сам стол с бортиками и лунками
    drawTable(renderer, view, projection);

    // Затем лунки (после стола, чтобы они были сверху)
    for (const auto &pocket : pocketPositions)
    {
        renderer.DrawPocket(pocket, pocketRadius, view, projection);
    }
}

inline void Scene::drawSkyBackground()
{
    // установка цвета фона
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    // очистка глубины и цвета
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

inline void Scene::drawFloor(Renderer &renderer,
                             const glm::mat4 &view,
                             const glm::mat4 &projection)
{
    // 1. Рассчитываем позицию самой нижней точки ножек
    float lowestLegY = -legHeight / 2.0f; // Центр ножки
    lowestLegY -= legHeight / 2.0f;       // Нижняя точка ножки

    // 2. Определяем границы пола по ножкам
    float xCorner = tableSize.x / 2.0f + wallThickness - legWidth / 2.0f;
    float zCorner = tableSize.y / 2.0f + wallThickness - legWidth / 2.0f;

    // 3. Размер пола (на 20% больше области ножек)
    glm::vec2 floorSize(
        (xCorner * 2.0f) * 5.0f, // Ширина (X)
        (zCorner * 2.0f) * 5.0f  // Глубина (Z)
    );

    // 4. Позиция пола
    glm::vec3 floorPos(0.0f, lowestLegY, 0.0f);

    // 5. Отрисовка
    renderer.DrawTable(floorPos, floorSize, floorColor, view, projection);
}

inline void Scene::drawTable(Renderer &renderer, const glm::mat4 &view, const glm::mat4 &projection)
{
    // Игровая поверхность
    renderer.DrawTable(glm::vec3(0, 0, 0), tableSize, tableColor, view, projection);

    // Размеры бортиков
    glm::vec3 sizeX(tableSize.x + 2 * wallThickness, wallHeight, wallThickness);
    glm::vec3 sizeZ(wallThickness, wallHeight, tableSize.y);

    // Цвета
    glm::vec3 topColor(0.3f, 0.15f, 0.05f); // Темно-коричневый для верха
    glm::vec3 sideColor(0.5f, 0.35f, 0.2f); // Светло-коричневый для боковин

    // Столешница
    renderer.DrawTable(glm::vec3(0, -0.01f, 0), tableSize, sideColor, view, projection);

    // Высота верхней части (1/4 от общей высоты)
    float topHeight = wallHeight / 4;
    float baseHeight = wallHeight - topHeight;

    // Задний бортик (основание)
    renderer.DrawBox(
        glm::vec3(0.0f, baseHeight / 2, tableSize.y / 2 + wallThickness / 2),
        glm::vec3(sizeX.x, baseHeight, sizeX.z),
        sideColor, view, projection);

    // Задний бортик (верх)
    renderer.DrawBox(
        glm::vec3(0.0f, baseHeight + topHeight / 2, tableSize.y / 2 + wallThickness / 2),
        glm::vec3(sizeX.x, topHeight, sizeX.z),
        topColor, view, projection);

    // Передний бортик (основание)
    renderer.DrawBox(
        glm::vec3(0.0f, baseHeight / 2, -(tableSize.y / 2 + wallThickness / 2)),
        glm::vec3(sizeX.x, baseHeight, sizeX.z),
        sideColor, view, projection);

    // Передний бортик (верх)
    renderer.DrawBox(
        glm::vec3(0.0f, baseHeight + topHeight / 2, -(tableSize.y / 2 + wallThickness / 2)),
        glm::vec3(sizeX.x, topHeight, sizeX.z),
        topColor, view, projection);

    // Левый бортик (основание)
    renderer.DrawBox(
        glm::vec3(-(tableSize.x / 2 + wallThickness / 2), baseHeight / 2, 0.0f),
        glm::vec3(sizeZ.x, baseHeight, sizeZ.z),
        sideColor, view, projection);

    // Левый бортик (верх)
    renderer.DrawBox(
        glm::vec3(-(tableSize.x / 2 + wallThickness / 2), baseHeight + topHeight / 2, 0.0f),
        glm::vec3(sizeZ.x, topHeight, sizeZ.z),
        topColor, view, projection);

    // Правый бортик (основание)
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2 + wallThickness / 2, baseHeight / 2, 0.0f),
        glm::vec3(sizeZ.x, baseHeight, sizeZ.z),
        sideColor, view, projection);

    // Правый бортик (верх)
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2 + wallThickness / 2, baseHeight + topHeight / 2, 0.0f),
        glm::vec3(sizeZ.x, topHeight, sizeZ.z),
        topColor, view, projection);

    // Ножки стола (остаются без изменений)
    glm::vec3 legSize(legWidth, legHeight, legWidth);
    float xCorner = tableSize.x / 2 + wallThickness - legWidth / 2;
    float zCorner = tableSize.y / 2 + wallThickness - legWidth / 2;

    renderer.DrawBox(glm::vec3(-xCorner, -legHeight / 2, -zCorner), legSize, legColor, view, projection);
    renderer.DrawBox(glm::vec3(-xCorner, -legHeight / 2, zCorner), legSize, legColor, view, projection);
    renderer.DrawBox(glm::vec3(xCorner, -legHeight / 2, -zCorner), legSize, legColor, view, projection);
    renderer.DrawBox(glm::vec3(xCorner, -legHeight / 2, zCorner), legSize, legColor, view, projection);
}