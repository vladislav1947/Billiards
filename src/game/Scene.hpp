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
      pocketRadius(0.08f)
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
    glm::vec2 floorSize(100.0f, 100.0f);
    glm::vec3 floorPos(0.0f, -0.01f, 0.0f);
    renderer.DrawTable(floorPos, floorSize, floorColor, view, projection);
}

inline void Scene::drawTable(Renderer& renderer,
                             const glm::mat4& view,
                             const glm::mat4& projection)
{
    // Игровая поверхность
    renderer.DrawTable(glm::vec3(0, 0, 0), tableSize, tableColor, view, projection);

    // Параллелепипеды - бортики стола (объемные)
    // Ширина, высота, глубина бортиков
    float wallHeight = 0.1f;
    float wallThickness = 0.05f;

    // Бортики по X (длинные и тонкие)
    glm::vec3 sizeX(tableSize.x + 2 * wallThickness, wallHeight, wallThickness);
    // Бортики по Z (тонкие и длинные)
    glm::vec3 sizeZ(wallThickness, wallHeight, tableSize.y);

    // Бортик - задний (по Z положительный)
    renderer.DrawBox(
        glm::vec3(0.0f, wallHeight / 2.0f, tableSize.y / 2.0f + wallThickness / 2.0f),
        sizeX, wallColor, view, projection);

    // Бортик - передний (по Z отрицательный)
    renderer.DrawBox(
        glm::vec3(0.0f, wallHeight / 2.0f, - (tableSize.y / 2.0f + wallThickness / 2.0f)),
        sizeX, wallColor, view, projection);

    // Бортик - левый (по X отрицательный)
    renderer.DrawBox(
        glm::vec3(- (tableSize.x / 2.0f + wallThickness / 2.0f), wallHeight / 2.0f, 0.0f),
        sizeZ, wallColor, view, projection);

    // Бортик - правый (по X положительный)
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2.0f + wallThickness / 2.0f, wallHeight / 2.0f, 0.0f),
        sizeZ, wallColor, view, projection);

    // Параллелепипеды - ножки стола (толстые и высокие)
    float legWidth = 0.1f;
    float legHeight = 0.7f;
    glm::vec3 legColor(0.3f, 0.15f, 0.05f);

    glm::vec3 legSize(legWidth, legHeight, legWidth);

    // Левая передняя ножка
    renderer.DrawBox(
        glm::vec3(- (tableSize.x / 2.0f + wallThickness), -legHeight / 2.0f, - (tableSize.y / 2.0f + wallThickness)),
        legSize, legColor, view, projection);

    // Левая задняя ножка
    renderer.DrawBox(
        glm::vec3(- (tableSize.x / 2.0f + wallThickness), -legHeight / 2.0f, tableSize.y / 2.0f + wallThickness),
        legSize, legColor, view, projection);

    // Правая передняя ножка
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2.0f + wallThickness, -legHeight / 2.0f, - (tableSize.y / 2.0f + wallThickness)),
        legSize, legColor, view, projection);

    // Правая задняя ножка
    renderer.DrawBox(
        glm::vec3(tableSize.x / 2.0f + wallThickness, -legHeight / 2.0f, tableSize.y / 2.0f + wallThickness),
        legSize, legColor, view, projection);
}
