cmake_minimum_required(VERSION 3.20)
project(Billiards VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GLFW & GLAD
include(FetchContent)
FetchContent_Declare(glfw
  :contentReference[oaicite:1]{index=1}
  :contentReference[oaicite:2]{index=2}
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(glad
  :contentReference[oaicite:3]{index=3}
  GIT_TAG        main
)
FetchContent_MakeAvailable(glad)

# Добавим include для GLM
FetchContent_Declare(glm
  :contentReference[oaicite:4]{index=4}
  :contentReference[oaicite:5]{index=5}
)
FetchContent_MakeAvailable(glm)

# Исполняемый файл
add_executable(${PROJECT_NAME}
    :contentReference[oaicite:6]{index=6}
    :contentReference[oaicite:7]{index=7}
    :contentReference[oaicite:8]{index=8}
    :contentReference[oaicite:9]{index=9}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    OpenGL::GL
)