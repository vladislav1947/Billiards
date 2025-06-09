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

    void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color,
              const glm::mat4& view, const glm::mat4& projection);

    void DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                  const glm::mat4& view, const glm::mat4& projection);

    void DrawTable(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color,
                   const glm::mat4& view, const glm::mat4& projection);

    void DrawCue(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness,
                 const glm::mat4& view, const glm::mat4& projection);

    void DrawWall(const glm::vec3& position, const glm::vec2& size, float height, const glm::vec3& color,
        const glm::mat4& view, const glm::mat4& projection);

    void DrawPocket(const glm::vec3& position, float radius,
                const glm::mat4& view, const glm::mat4& projection);

    void DrawBox(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color,
                 const glm::mat4& view, const glm::mat4& projection);

    void SetCameraPos(const glm::vec3& pos) { cameraPos = pos; }

    void InitCube();

private:
    glm::vec3 cameraPos;
    Shader shader;

    GLuint sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
    GLuint quadVAO = 0, quadVBO = 0, quadEBO = 0;
    GLuint cubeVAO = 0, cubeVBO = 0, cubeEBO = 0;

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
    InitCube();  // Добавьте эту строку

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color,
                        const glm::mat4& view, const glm::mat4& projection) {

    shader.Use(); // Заменяем simpleShader на shader
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = projection * view * model;
    shader.SetMat4("uModel", model); // Используем shader вместо simpleShader
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    float vertices[] = {
        start.x, start.y, start.z,
        end.x, end.y, end.z
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, 2);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
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

void Renderer::CreateCue() {
    glGenVertexArrays(1, &cueVAO);
    glGenBuffers(1, &cueVBO);
    glGenBuffers(1, &cueEBO);

    glBindVertexArray(cueVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cueVBO);
    float emptyCue[12] = {}; // 4 вершины * 3 координаты
    glBufferData(GL_ARRAY_BUFFER, sizeof(emptyCue), emptyCue, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cueEBO);
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

void Renderer::DrawBox(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color,
                       const glm::mat4& view, const glm::mat4& projection)
{
    // Построить матрицу модели для параллелепипеда
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);

    // Установить шейдер, передать uniform-ы (модель, вид, проекция, цвет)
    shader.Use();
    shader.SetMat4("uModel", model);       // Используем единообразные имена
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", color);

    // Отрисовать куб (используя заранее подготовленный VAO куба)
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Renderer::InitCube() {
    // Вершины куба (8 вершин)
    float vertices[] = {
        // Передняя грань
        -0.5f, -0.5f,  0.5f,  // 0
         0.5f, -0.5f,  0.5f,  // 1
         0.5f,  0.5f,  0.5f,  // 2
        -0.5f,  0.5f,  0.5f,  // 3
        
        // Задняя грань
        -0.5f, -0.5f, -0.5f,  // 4
         0.5f, -0.5f, -0.5f,  // 5
         0.5f,  0.5f, -0.5f,  // 6
        -0.5f,  0.5f, -0.5f   // 7
    };

    // Индексы для отрисовки (12 треугольников = 36 индексов)
    unsigned int indices[] = {
        // Передняя грань
        0, 1, 2, 2, 3, 0,
        // Правая грань
        1, 5, 6, 6, 2, 1,
        // Задняя грань
        7, 6, 5, 5, 4, 7,
        // Левая грань
        4, 0, 3, 3, 7, 4,
        // Нижняя грань
        4, 5, 1, 1, 0, 4,
        // Верхняя грань
        3, 2, 6, 6, 7, 3
    };

    // Создаем и настраиваем VAO, VBO и EBO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    // Заполняем VBO вершинами
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Заполняем EBO индексами
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Указываем атрибуты вершин
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void Renderer::DrawPocket(const glm::vec3& position, float radius,
                          const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    const int segments = 64;
    std::vector<glm::vec3> vertices;

    vertices.push_back(position);

    for (int i = 0; i <= segments; ++i) {
        float angle = glm::two_pi<float>() * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        vertices.emplace_back(position.x + x, position.y, position.z + z);
    }

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProjection", projection);
    shader.SetVec3("uColor", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.SetVec3("uNormal", glm::vec3(0.0f, 1.0f, 0.0f));
    shader.SetVec3("uLightDir", glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)));

    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size()));

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Renderer::DrawBall(const glm::vec3& position, float radius, const glm::vec3& color,
                        const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    glm::vec3 normal = glm::normalize(position - glm::vec3(0.0f, position.y, 0.0f));

    glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 lightDir = glm::normalize(lightPos - position);

    shader.SetVec3("uNormal", normal);
    shader.SetVec3("uLightDir", lightDir);

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

    shader.SetVec3("uNormal", glm::vec3(0.0f, 1.0f, 0.0f));

    shader.SetVec3("uLightDir", glm::vec3(0.0f, -1.0f, 0.0f));

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

void Renderer::DrawCue(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, float thickness,
                       const glm::mat4& view, const glm::mat4& projection) {
    shader.Use();

    glm::vec3 dir = glm::normalize(to - from);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(dir, up)) * (thickness * 0.5f);

    glm::vec3 vertices[4] = {
        from - right,
        from + right,
        to + right,
        to - right
    };

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