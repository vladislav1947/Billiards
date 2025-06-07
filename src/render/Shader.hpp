#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();

    // Инициализация шейдера: компиляция и линковка
    bool Init();

    // Активировать шейдер
    void Use() const;

    // Установить uniform-переменные
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint programID = 0;

    // Компиляция одного шейдера
    GLuint CompileShader(GLenum type, const char* source);

    // Встроенные минималистичные шейдеры
    static constexpr const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;
        void main() {
            gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
        }
    )";

    static constexpr const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 uColor;
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )";
};

Shader::Shader() = default;

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool Shader::Init() {
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

    // Шейдеры больше не нужны после линковки
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

void Shader::Use() const {
    glUseProgram(programID);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

GLuint Shader::CompileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED of type " 
                  << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")
                  << "\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}