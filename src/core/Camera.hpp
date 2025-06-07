#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    float getZoom() const;
    glm::vec3 getPosition() const;

private:
    void updateCameraVectors();

private:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;

    float m_movementSpeed = 2.5f;
    float m_mouseSensitivity = 0.1f;
    float m_zoom = 45.0f;
};

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position),
      m_worldUp(up),
      m_yaw(yaw),
      m_pitch(pitch)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 100.0f);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD)
        m_position += m_front * velocity;
    if (direction == Camera_Movement::BACKWARD)
        m_position -= m_front * velocity;
    if (direction == Camera_Movement::LEFT)
        m_position -= m_right * velocity;
    if (direction == Camera_Movement::RIGHT)
        m_position += m_right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw   += xoffset;
    m_pitch += yoffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f)  m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    m_zoom -= yoffset;
    if (m_zoom < 1.0f)   m_zoom = 1.0f;
    if (m_zoom > 45.0f)  m_zoom = 45.0f;
}

float Camera::getZoom() const {
    return m_zoom;
}

glm::vec3 Camera::getPosition() const {
    return m_position;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}