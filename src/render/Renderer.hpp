#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Shader.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Init();

    void DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                  const glm::mat4& view, const glm::mat4& projection);

    void DrawTable(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color,
                   const glm::mat4& view, const glm::mat4& projection);

    // Теперь с параметром толщины линии
    void DrawCue(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness,
                 const glm::mat4& view, const glm::mat4& projection);

    void DrawWall(const glm::vec3& position, const glm::vec2& size, float height, const glm::vec3& color,
        const glm::mat4& view, const glm::mat4& projection);

    void DrawPocket(const glm::vec3& position, float radius,
                const glm::mat4& view, const glm::mat4& projection);

    void SetCameraPos(const glm::vec3& pos) { cameraPos = pos; }

private:


    glm::vec3 cameraPos;
    Shader shader;

    GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
    GLuint quadVAO = 0, quadVBO = 0, quadEBO = 0;

    // Для кия теперь рисуем не линию, а прямоугольник (2 треугольника)
    GLuint cueVAO = 0, cueVBO = 0, cueEBO = 0;

    unsigned int indexCount = 0;

    void CreateSphere();
    void CreateQuad();
    void CreateCue();

    void Cleanup();
};

// Implementation

Renderer::Renderer() = default;

Renderer::~Renderer() {
    Cleanup();
}

bool Renderer::Init() {
    if (!shader.Init()) {
        std::cerr << "Failed to initialize shader\n";
        return false;
    }

    CreateSphere();
    CreateQuad();
    CreateCue();

    return true;
}

void Renderer::CreateSphere() {
    const unsigned int X_SEGMENTS = 16;
    const unsigned int Y_SEGMENTS = 16;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / X_SEGMENTS;
            float ySegment = (float)y / Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }

    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
            unsigned int first = y * (X_SEGMENTS + 1) + x;
            unsigned int second = first + X_SEGMENTS + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Renderer::CreateQuad() {
    float quadVertices[] = {
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f,
        -0.5f, 0.0f,  0.5f,
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

// Создаем VAO/VBO/EBO для кия — это будет прямоугольник с 4 вершинами и 6 индексами
void Renderer::CreateCue() {
    glGenVertexArrays(1, &cueVAO);
    glGenBuffers(1, &cueVBO);
    glGenBuffers(1, &cueEBO);

    glBindVertexArray(cueVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cueVBO);
    // 4 вершины по 3 float (x,y,z)
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cueEBO);
    // 2 треугольника = 6 индексов
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Renderer::DrawWall(const glm::vec3& position, const glm::vec2& size, float height, const glm::vec3& color,
                        const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(size.x, height, size.y));

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::DrawPocket(const glm::vec3& position, float radius,
                          const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(radius, 0.1f, radius)); // Немного приплюснутый

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", glm::vec3(0.0f, 0.0f, 0.0f)); // Черные лунки

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                        const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();
    
    // Нормаль для сферы (от центра к поверхности)
    glm::vec3 normal = glm::normalize(position - glm::vec3(0.0f, position.y, 0.0f));
    
    // Источник света прямо сверху (в середине стола)
    glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f); // 2 единицы над столом
    glm::vec3 lightDir = glm::normalize(lightPos - position);
    
    shader.SetVec3("uNormal", normal);
    shader.SetVec3("uLightDir", lightDir);
    
    // Остальной код без изменений...
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(radius));

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::DrawTable(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color,
                         const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();
    
    // Для стола нормаль всегда вверх
    shader.SetVec3("uNormal", glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Направление света сверху вниз (из центра)
    shader.SetVec3("uLightDir", glm::vec3(0.0f, -1.0f, 0.0f));
    
    // Остальной код без изменений...
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(size.x, 1.0f, size.y));

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    glBindVertexArray(quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Отрисовка кия как прямоугольника с толщиной
void Renderer::DrawCue(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness,
                       const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    // Вычисляем направление и вектор "вбок" для толщины
    glm::vec3 dir = glm::normalize(to - from);
    // Вектор "вверх" для построения боковой смещённой линии — можно взять мировой "вверх"
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(dir, up)) * (thickness * 0.5f);

    // 4 вершины прямоугольника кия
    // Вершины идут по порядку для индексов {0,1,2} и {2,3,0}
    glm::vec3 vertices[4] = {
        from - right,
        from + right,
        to + right,
        to - right
    };

    // Загружаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, cueVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    glBindVertexArray(cueVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::Cleanup() {
    if (sphereVAO) {
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);
    }
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &quadEBO);
    }
    if (cueVAO) {
        glDeleteVertexArrays(1, &cueVAO);
        glDeleteBuffers(1, &cueVBO);
        glDeleteBuffers(1, &cueEBO);
    }
}