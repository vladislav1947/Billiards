#pragma once

#include <render/Renderer.hpp>
#include <glm/glm.hpp>
#include <vector>

class Scene {
public:
    Scene();
    void Render(Renderer& renderer,
                const glm::mat4& view,
                const glm::mat4& projection,
                const glm::vec3& cameraPos);

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
    float legWidth;   // Ширина ножки
    float legHeight;  // Высота ножки
    glm::vec3 legColor; // Цвет ножек

    void drawSkyBackground();
    void drawFloor(Renderer& renderer,
                   const glm::mat4& view,
                   const glm::mat4& projection);
    void drawTable(Renderer& renderer,
                   const glm::mat4& view,
                   const glm::mat4& projection);
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
      legWidth(0.08f),   // Ширина ножки (8 см)
      legHeight(0.6f),   // Высота ножки (60 см)
      legColor(0.3f, 0.15f, 0.05f) // Цвет ножек (коричневый)
{
    pocketPositions = {
        {-0.95f, 0.01f, -0.45f},
        {-0.95f, 0.01f,  0.45f},
        { 0.95f, 0.01f, -0.45f},
        { 0.95f, 0.01f,  0.45f},
        { 0.0f,  0.01f, -0.45f},
        { 0.0f,  0.01f,  0.45f}
    };
}

inline void Scene::Render(Renderer& renderer,
                          const glm::mat4& view,
                          const glm::mat4& projection,
                          const glm::vec3& cameraPos)
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
    for (const auto& pocket : pocketPositions) {
        renderer.DrawPocket(pocket, pocketRadius, view, projection);
    }
}

inline void Scene::drawSkyBackground() {
    // установка цвета фона
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    // очистка глубины и цвета
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

inline void Scene::drawFloor(Renderer& renderer,
                             const glm::mat4& view,
                             const glm::mat4& projection)
{
    glm::vec2 floorSize(2.4f, 1.25f);
    glm::vec3 floorPos(0.0f, -0.01f, 0.0f);
    renderer.DrawTable(floorPos, floorSize, floorColor, view, projection);
}

inline void Scene::drawTable(Renderer& renderer,
                             const glm::mat4& view,
                             const glm::mat4& projection) 
{
    // Игровая поверхность (как раньше)
    renderer.DrawTable(glm::vec3(0, 0, 0), tableSize, tableColor, view, projection);

    // Бортики (как раньше)
    glm::vec3 sizeX(tableSize.x + 2 * wallThickness, wallHeight, wallThickness);
    glm::vec3 sizeZ(wallThickness, wallHeight, tableSize.y);

    // Задний бортик
    renderer.DrawBox(
        glm::vec3(0.0f, wallHeight / 2.0f, tableSize.y / 2.0f + wallThickness / 2.0f),
        sizeX, wallColor, view, projection);

    // Передний бортик
    renderer.DrawBox(
        glm::vec3(0.0f, wallHeight / 2.0f, - (tableSize.y / 2.0f + wallThickness / 2.0f)),
        sizeX, wallColor, view, projection);

    // Левый бортик
    renderer.DrawBox(
        glm::vec3(- (tableSize.x / 2.0f + wallThickness / 2.0f), wallHeight / 2.0f, 0.0f),
        sizeZ, wallColor, view, projection);

    // Правый бортик
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2.0f + wallThickness / 2.0f, wallHeight / 2.0f, 0.0f),
        sizeZ, wallColor, view, projection);

    // Ножки стола (4 штуки по углам)
    glm::vec3 legSize(legWidth, legHeight, legWidth);

    // Координаты углов стола с учётом толщины бортиков
    float xCorner = tableSize.x / 2.0f + wallThickness - legWidth / 2.0f;
    float zCorner = tableSize.y / 2.0f + wallThickness - legWidth / 2.0f;

    // Левая передняя ножка
    renderer.DrawBox(
        glm::vec3(-xCorner, -legHeight / 2.0f, -zCorner),
        legSize, legColor, view, projection);

    // Левая задняя ножка
    renderer.DrawBox(
        glm::vec3(-xCorner, -legHeight / 2.0f, zCorner),
        legSize, legColor, view, projection);

    // Правая передняя ножка
    renderer.DrawBox(
        glm::vec3(xCorner, -legHeight / 2.0f, -zCorner),
        legSize, legColor, view, projection);

    // Правая задняя ножка
    renderer.DrawBox(
        glm::vec3(xCorner, -legHeight / 2.0f, zCorner),
        legSize, legColor, view, projection);
}