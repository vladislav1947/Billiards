#pragma once

#define _USE_MATH_DEFINES
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Инициализация рендерера
    bool Init();

    // Рисуем примитив (например, мяч)
    void DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                  const glm::mat4& view, const glm::mat4& projection);

    // Рисуем стол (плоскость)
    void DrawTable(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color,
                   const glm::mat4& view, const glm::mat4& projection);

private:
    Shader shader;

    GLuint sphereVAO = 0;
    GLuint sphereVBO = 0;
    GLuint sphereEBO = 0;
    unsigned int indexCount = 0;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    // Создание геометрии сферы
    void CreateSphere();

    // Создание геометрии квадрата (для стола)
    void CreateQuad();

    void Cleanup();
};

#include <vector>
#include <cmath>
#include <iostream>

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

    return true;
}

void Renderer::CreateSphere() {
    // Генерация сферы — минималистично, low-poly UV сфера
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

    bool oddRow = false;
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

    // Позиция: layout (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Renderer::CreateQuad() {
    float quadVertices[] = {
        // позиции
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
    GLuint quadEBO;
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

void Renderer::DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                        const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

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

void Renderer::Cleanup() {
    if (sphereVAO) {
        glDeleteVertexArrays(1, &sphereVAO);
        glDeleteBuffers(1, &sphereVBO);
        glDeleteBuffers(1, &sphereEBO);
    }
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}