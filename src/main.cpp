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

    auto camera = std::make_shared<Camera>(
        glm::vec3(0.0f, 2.0f, 3.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f, -30.0f
    );
    
    window.attachCamera(camera);  // Подключить камеру

    Renderer renderer;
    if (!renderer.Init()) {      // Инициализировать рендерер
        return -1;
    }

    Physics physics(2.0f, 1.0f, 0.1f);
    
    std::vector<Ball> balls = {
        Ball(glm::vec3(-0.5f, 0.05f, 0.0f), 0.05f, 1.0f),
        Ball(glm::vec3(0.5f, 0.05f, 0.0f), 0.05f, 1.0f)
    };

    balls[0].velocity = glm::vec3(1.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);     // Включить тест глубины

    while (!window.shouldClose()) {
        window.update();         // Правильный метод
        window.processInput();   // Обработка ввода

        physics.Update(balls, window.getDeltaTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Получить матрицы
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix(window.getAspectRatio());

        // Рисовать стол
        renderer.DrawTable(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(2.0f, 1.0f), 
                          glm::vec3(0.0f, 0.8f, 0.0f), view, projection);

        // Рисовать шары
        for (const auto& ball : balls) {
            renderer.DrawBall(ball.position, ball.radius, glm::vec3(1.0f, 0.0f, 0.0f), 
                             view, projection);
        }

        window.swapBuffers();
    }

    return 0;
}