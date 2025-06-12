#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <iostream>
#include "Camera.hpp"

class Window {
public:
    
    Window(int width, int height, const std::string& title);
    ~Window();

    bool init();
    void shutdown();

    // Основные операции
    void update();
    void swapBuffers() const;
    bool shouldClose() const;

    // Ввод и управление
    void processInput();
    bool isKeyPressed(int key) const;
    void attachCamera(const std::shared_ptr<Camera>& camera);

    // Получение состояния
    float getDeltaTime() const;
    float getAspectRatio() const;
    GLFWwindow* getGLFWwindow() const;

private:
    
    void setupCallbacks();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* m_window = nullptr;
    int m_width;
    int m_height;
    std::string m_title;

    float m_lastFrame = 0.0f;
    float m_deltaTime = 0.0f;

    static inline float s_lastX = 400.0f;
    static inline float s_lastY = 300.0f;
    static inline bool s_firstMouse = true;

    static inline std::shared_ptr<Camera> s_camera = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title) {}

Window::~Window() {
    shutdown();
}

bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "[GLFW] Failed to initialize" << std::endl;
        return false;
    }

    // Настройка OpenGL контекста
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "[GLFW] Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[GLAD] Failed to initialize OpenGL context" << std::endl;
        return false;
    }

    // Начальные настройки OpenGL
    glViewport(0, 0, m_width, m_height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    setupCallbacks();

    return true;
}

void Window::shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

void Window::update() {
    // Обновление тайминга
    float currentFrame = glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    glfwPollEvents();
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::processInput() {
    if (!s_camera) return;
    
    const float deltaTime = getDeltaTime();
    if (isKeyPressed(GLFW_KEY_W)) s_camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_S)) s_camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_A)) s_camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    if (isKeyPressed(GLFW_KEY_D)) s_camera->processKeyboard(CameraMovement::RIGHT, deltaTime);

    glEnable(GL_DEPTH_TEST); // Всегда включаем тест глубины
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

float Window::getDeltaTime() const {
    return m_deltaTime;
}

float Window::getAspectRatio() const {
    return static_cast<float>(m_width) / m_height;
}

GLFWwindow* Window::getGLFWwindow() const {
    return m_window;
}

void Window::attachCamera(const std::shared_ptr<Camera>& camera) {
    s_camera = camera;
}

void Window::setupCallbacks() {
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!s_camera) return;

    if (s_firstMouse) {
        s_lastX = static_cast<float>(xpos);
        s_lastY = static_cast<float>(ypos);
        s_firstMouse = false;
    }

    const float xoffset = static_cast<float>(xpos) - s_lastX;
    const float yoffset = s_lastY - static_cast<float>(ypos);

    s_lastX = static_cast<float>(xpos);
    s_lastY = static_cast<float>(ypos);

    s_camera->processMouseMovement(xoffset, yoffset);
}

void Window::scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    if (s_camera) {
        s_camera->processMouseScroll(static_cast<float>(yoffset));
    }
}