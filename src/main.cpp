#include <core/Window.hpp>
#include <core/Camera.hpp>
#include <render/Shader.hpp>
#include <render/Renderer.hpp>
#include <game/Physics.hpp>
#include <game/Ball.hpp>
#include <game/Cue.hpp>
#include <game/Scene.hpp>
#include <iostream>

int main() {
    Window window(1280, 720, "3D Billiards");
    if (!window.init()) return -1;

    auto camera = std::make_shared<Camera>(
        glm::vec3(0.0f, 2.0f, 3.0f),  // позиция камеры
        glm::vec3(0.0f, 1.0f, 0.0f),  // вектор вверх
        -90.0f, -30.0f                // yaw, pitch
    );
    window.attachCamera(camera);

    Renderer renderer;
    if (!renderer.Init()) return -1;

    // Создаем объект сцены
    Scene scene;

    Physics physics(2.0f, 1.0f, 0.1f);

    float ballRadius = 0.05f;
    float ballMass = 1.0f;
    std::vector<Ball> balls;

    // Черный шар - кий-бол (используем конструктор нового класса)
    balls.emplace_back(glm::vec3(-0.8f, ballRadius, 0.0f), ballRadius, ballMass);

    // Треугольная расстановка белых шаров
    int rows = 5;
    float spacing = ballRadius * 2.05f; // чуть больше диаметра, чтобы не перекрывались
    glm::vec3 startPos(0.6f, ballRadius, 0.0f);  // ближе к центру стола, справа

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col <= row; ++col) {
            float x = startPos.x + row * spacing * 0.866f; // cos(30°)
            float z = startPos.z - row * spacing * 0.5f + col * spacing;
            balls.emplace_back(glm::vec3(x, ballRadius, z), ballRadius, ballMass);
        }
    }

    // Создаем объект кия
    Cue cue;

    glEnable(GL_DEPTH_TEST);

    // Лунки теперь управляются классом Scene, но нам нужны координаты для физики
    const float pocketRadius = 0.08f;
    std::vector<glm::vec3> pockets = {
        glm::vec3(-0.95f, 0.01f, -0.45f),  // левый нижний угол
        glm::vec3(-0.95f, 0.01f, 0.45f),   // левый верхний угол
        glm::vec3(0.95f, 0.01f, -0.45f),   // правый нижний угол
        glm::vec3(0.95f, 0.01f, 0.45f),    // правый верхний угол
        glm::vec3(0.0f, 0.01f, -0.45f),    // середина нижней стороны
        glm::vec3(0.0f, 0.01f, 0.45f)      // середина верхней стороны
    };

    // Отладочная информация
    bool showDebugInfo = true;
    float debugTimer = 0.0f;

    while (!window.shouldClose()) {
        window.update();
        window.processInput();

        float dt = window.getDeltaTime();
        debugTimer += dt;

        // Управление кием: поворот влево/вправо
        if (window.isKeyPressed(GLFW_KEY_LEFT)) {
            cue.rotate(90.0f * dt);
        }
        if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
            cue.rotate(-90.0f * dt);
        }

        // Управление смещением точки удара
        if (window.isKeyPressed(GLFW_KEY_Q)) {
            cue.adjustOffset(glm::vec2(-dt, 0.0f));
        }
        if (window.isKeyPressed(GLFW_KEY_E)) {
            cue.adjustOffset(glm::vec2(dt, 0.0f));
        }
        if (window.isKeyPressed(GLFW_KEY_R)) {
            cue.adjustOffset(glm::vec2(0.0f, dt));
        }
        if (window.isKeyPressed(GLFW_KEY_F)) {
            cue.adjustOffset(glm::vec2(0.0f, -dt));
        }

        // Зарядка силы удара при зажатом пробеле
        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            cue.charge(dt);
        } else {
            // Отпуск пробела — наносим удар, если сила > 0
            if (cue.getPower() > 0.01f && !balls[0].isMoving()) {
                glm::vec3 impulse = cue.release();
                balls[0].applyImpulse(impulse);
            }
        }

        // Переключение отладочной информации
        if (window.isKeyPressed(GLFW_KEY_TAB) && debugTimer > 0.5f) {
            showDebugInfo = !showDebugInfo;
            debugTimer = 0.0f;
        }

        // Обновление физики
        physics.Update(balls, dt);

        // Проверка попадания в лунки
        for (auto& ball : balls) {
            for (const auto& pocket : pockets) {
                if (physics.CheckPocketCollision(ball, pocket, pocketRadius)) {
                    ball.setPosition(glm::vec3(-100.0f, -100.0f, -100.0f));
                    ball.setVelocity(glm::vec3(0.0f));
                    break;
                }
            }
        }

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix(window.getAspectRatio());

        renderer.SetCameraPos(camera->getPosition());

        // Рендеринг сцены
        scene.Render(renderer, view, projection, camera->getPosition());

        // Отрисовка шаров
        for (size_t i = 0; i < balls.size(); ++i) {
            glm::vec3 color = (i == 0) ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.7f, 0.7f, 0.7f);
            renderer.DrawBall(balls[i].getPosition(), balls[i].getRadius(), color, view, projection);
        }

        // Отрисовка кия
        const Ball& cueBall = balls[0];
        
        // Проверяем, движутся ли шары
        bool ballsMoving = false;
        for (const auto& ball : balls) {
            if (ball.isMoving()) {
                ballsMoving = true;
                break;
            }
        }
        
        if (!ballsMoving) {
            // Получаем точки кия
            glm::vec3 hitPoint = cue.getHitPoint(cueBall.getPosition(), cueBall.getRadius());
            glm::vec3 cueStart = cue.getCueStart(hitPoint);
            glm::vec3 cueEnd = cue.getCueEnd(hitPoint);
            
            glm::vec3 cueColor = glm::mix(
                glm::vec3(0.6f, 0.4f, 0.2f),  // коричневый (без силы)
                glm::vec3(1.0f, 0.2f, 0.2f),  // красный (максимальная сила)
                cue.getPower()
            );
            
            // Рисуем кий
            renderer.DrawCue(cueStart, cueEnd, cueColor, cue.getWidth(), view, projection);

            //Вектор удара
            std::vector<glm::vec3> positions;
            for (const auto& b : balls) {
                positions.push_back(b.getPosition());
            }

            glm::vec3 impact = cue.computeImpactPoint(hitPoint, positions, cueBall.getRadius(), tableWidth, tableHeight);
            renderer.DrawLine(hitPoint, impact, {1.0f, 1.0f, 1.0f}, view, projection); // белая линия
        }

        window.swapBuffers();
    }

    return 0;
}