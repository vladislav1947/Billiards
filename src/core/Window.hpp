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

    void update();
    void swapBuffers() const;
    bool shouldClose() const;

    float getDeltaTime() const;
    float getAspectRatio() const;

    GLFWwindow* getGLFWwindow() const;

    void attachCamera(const std::shared_ptr<Camera>& camera);
    void processInput();

    bool isKeyPressed(int key) const;

private:
    void setupCallbacks();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
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

inline Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title) {}

inline Window::~Window() {
    shutdown();
}

inline bool Window::init() {
    if (!glfwInit()) {
        std::cerr << "[GLFW] Failed to initialize" << std::endl;
        return false;
    }

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

    glViewport(0, 0, m_width, m_height);
    setupCallbacks();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

inline void Window::shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_window = nullptr;
    }
}

inline void Window::update() {
    float currentFrame = glfwGetTime();
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    glfwPollEvents();
}

inline void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

inline bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

inline float Window::getDeltaTime() const {
    return m_deltaTime;
}

inline float Window::getAspectRatio() const {
    return static_cast<float>(m_width) / m_height;
}

inline GLFWwindow* Window::getGLFWwindow() const {
    return m_window;
}

inline void Window::attachCamera(const std::shared_ptr<Camera>& camera) {
    s_camera = camera;
}

inline void Window::processInput() {
    if (!s_camera) return;
    float deltaTime = getDeltaTime();
    if (isKeyPressed(GLFW_KEY_W))
        s_camera->processKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_S))
        s_camera->processKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (isKeyPressed(GLFW_KEY_A))
        s_camera->processKeyboard(Camera_Movement::LEFT, deltaTime);
    if (isKeyPressed(GLFW_KEY_D))
        s_camera->processKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// Реализация нового метода проверки клавиши
inline bool Window::isKeyPressed(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

inline void Window::setupCallbacks() {
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
}

inline void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

inline void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!s_camera) return;

    if (s_firstMouse) {
        s_lastX = static_cast<float>(xpos);
        s_lastY = static_cast<float>(ypos);
        s_firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - s_lastX;
    float yoffset = s_lastY - static_cast<float>(ypos);

    s_lastX = static_cast<float>(xpos);
    s_lastY = static_cast<float>(ypos);

    s_camera->processMouseMovement(xoffset, yoffset);
}

inline void Window::scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    if (s_camera)
        s_camera->processMouseScroll(static_cast<float>(yoffset));
}