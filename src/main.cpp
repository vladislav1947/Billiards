#include "core/Window.hpp"
#include "core/Camera.hpp"
#include "render/Shader.hpp"
#include "render/Renderer.hpp"
#include "game/Physics.hpp"

int main() {
    Window window(1280, 720, "3D Billiards");
    if (!window.init()) {
        return -1;
    }

    Shader shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");

    // Инициализируем физику: размер стола 2x1 (условно), трение 0.1
    Physics physics(2.0f, 1.0f, 0.1f);

    Camera camera(
    glm::vec3(0.0f, 0.0f, 3.0f), // позиция камеры
    glm::vec3(0.0f, 1.0f, 0.0f), // вектор "вверх" (обычно Y)
    -90.0f,                      // yaw, обычно -90 чтобы смотреть по -Z
    0.0f                        // pitch (угол наклона)
    );

    Renderer renderer;

    // Пример инициализации шаров (позиция, радиус, масса)
    std::vector<Ball> balls = {
        Ball(glm::vec3(-0.5f, 0.05f, 0.0f), 0.05f, 1.0f),
        Ball(glm::vec3(0.5f, 0.05f, 0.0f), 0.05f, 1.0f)
    };

    // Задаем начальные скорости шаров для теста
    balls[0].velocity = glm::vec3(1.0f, 0.0f, 0.0f);
    balls[1].velocity = glm::vec3(-1.0f, 0.0f, 0.0f);

    while (!window.shouldClose()) {
        float dt = window.processInput(camera);

        physics.Update(balls, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("view", camera.getViewMatrix());
        shader.setMat4("projection", camera.getProjectionMatrix(window.getAspect()));

        renderer.renderBalls(balls, shader);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}