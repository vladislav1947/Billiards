#include "core/Window.hpp"
#include "core/Camera.hpp"
#include "render/Shader.hpp"
#include "render/Renderer.hpp"
#include "game/Physics.hpp"
#include "game/Ball.hpp"  // Подключаем новый класс Ball

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

    glm::vec3 cueDirection(1.0f, 0.0f, 0.0f);
    float cuePower = 0.0f;
    const float cueThickness = 0.03f; // Толщина кия

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f); // голубой фон

    // Перед главным циклом
    const float pocketRadius = 0.08f;
    
    std::vector<glm::vec3> pockets = {
    glm::vec3(-0.95f, 0.01f, -0.45f),  // левый нижний угол
    glm::vec3(-0.95f, 0.01f, 0.45f),   // левый верхний угол
    glm::vec3(0.95f, 0.01f, -0.45f),   // правый нижний угол
    glm::vec3(0.95f, 0.01f, 0.45f),    // правый верхний угол
    glm::vec3(0.0f, 0.01f, -0.45f),    // середина нижней стороны
    glm::vec3(0.0f, 0.01f, 0.45f)      // середина верхней стороны
    };

    while (!window.shouldClose()) {
        window.update();
        window.processInput();

        float dt = window.getDeltaTime();

        // Управление кием: поворот влево/вправо
        if (window.isKeyPressed(GLFW_KEY_LEFT)) {
            float angle = 90.0f * dt; // градусов в секунду
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            cueDirection = glm::normalize(glm::vec3(rot * glm::vec4(cueDirection, 0)));
        }
        if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
            float angle = -90.0f * dt;
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
            cueDirection = glm::normalize(glm::vec3(rot * glm::vec4(cueDirection, 0)));
        }

        // Зарядка силы удара при зажатом пробеле
        if (window.isKeyPressed(GLFW_KEY_SPACE)) {
            cuePower += dt;
            if (cuePower > 1.0f) cuePower = 1.0f;
        } else {
            // Отпуск пробела — наносим удар, если сила > 0
            // Используем метод isMoving() нового класса Ball
            if (cuePower > 0.01f && !balls[0].isMoving()) {
                balls[0].setVelocity(cueDirection * (cuePower * 5.0f));
            }
            cuePower = 0.0f;
        }

        // Обновление физики
        physics.Update(balls, dt);

        // В главном цикле после physics.Update
        for (auto& ball : balls) {
            for (const auto& pocket : pockets) {
                if (physics.CheckPocketCollision(ball, pocket, pocketRadius)) {
                    ball.setPosition(glm::vec3(-100.0f, -100.0f, -100.0f)); // Убираем шар с поля
                    ball.setVelocity(glm::vec3(0.0f));
                    break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix(window.getAspectRatio());

         // +++ Добавляем эту строку здесь +++
        renderer.SetCameraPos(camera->getPosition()); // Передаем позицию камеры в рендерер

        // Зеленый стол
        renderer.DrawTable(glm::vec3(0, 0, 0), glm::vec2(2.0f, 1.0f), glm::vec3(0.0f, 0.5f, 0.0f), view, projection);

        // После отрисовки стола
        const float wallHeight = 0.1f;
        const float wallThickness = 0.05f;
        glm::vec3 wallColor(0.3f, 0.2f, 0.1f); // коричневый цвет
        
        // Длинные стенки (по Z)
        renderer.DrawWall(glm::vec3(-1.0f, wallHeight/2, 0.0f), 
                 glm::vec2(wallThickness, 1.0f + 2*wallThickness), 
                 wallHeight, wallColor, view, projection);
        renderer.DrawWall(glm::vec3(1.0f, wallHeight/2, 0.0f), 
                 glm::vec2(wallThickness, 1.0f + 2*wallThickness), 
                 wallHeight, wallColor, view, projection);

                 // Короткие стенки (по X)
        renderer.DrawWall(glm::vec3(0.0f, wallHeight/2, -0.5f), 
                 glm::vec2(2.0f, wallThickness), 
                 wallHeight, wallColor, view, projection);
        renderer.DrawWall(glm::vec3(0.0f, wallHeight/2, 0.5f), 
                 glm::vec2(2.0f, wallThickness), 
                 wallHeight, wallColor, view, projection);

        // В отрисовке после стенок
        for (const auto& pocket : pockets) {
            renderer.DrawPocket(pocket, pocketRadius, view, projection);
        }

        

        // Отрисовка шаров: первый — черный, остальные — белые (светло-серые)
        // Используем методы нового класса Ball
        for (size_t i = 0; i < balls.size(); ++i) {
            glm::vec3 color = (i == 0) ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.7f, 0.7f, 0.7f);
            renderer.DrawBall(balls[i].getPosition(), balls[i].getRadius(), color, view, projection);
        }

        // Отрисовка кия — прямоугольник с толщиной
        const Ball& cueBall = balls[0];
        float cueLength = 2.0f * cuePower + 0.5f; // длина киа зависит от силы
        glm::vec3 cueEnd = cueBall.getPosition() - glm::normalize(cueDirection) * cueLength;
        glm::vec3 cueColor(1.0f, 0.7f, 0.3f);
        
        // Проверяем, движутся ли шары, используя метод isMoving()
        bool ballsMoving = false;
        for (const auto& ball : balls) {
            if (ball.isMoving()) {
                ballsMoving = true;
                break;
            }
        }
        
        if (!ballsMoving) {
            // Рисуем кий только если шары стоят
            renderer.DrawCue(cueBall.getPosition(), cueEnd, cueColor, cueThickness, view, projection);
        }

        window.swapBuffers();
    }

    return 0;
}