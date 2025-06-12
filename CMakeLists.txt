cmake_minimum_required(VERSION 3.20)
project(Billiards VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${stb_SOURCE_DIR}
    src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
    OpenGL::GL
)

# Копирование текстур в бинарную директорию (добавлено)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/textures)
file(GLOB TEXTURE_FILES "textures/*.jpg")
file(COPY ${TEXTURE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/textures)