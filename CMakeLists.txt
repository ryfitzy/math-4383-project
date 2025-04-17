cmake_minimum_required(VERSION 3.10)
project(OpenGL_Radiosity_vs_RayTracing)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL and GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Find GLM
find_path(GLM_INCLUDE_DIR glm/glm.hpp)
include_directories(${GLM_INCLUDE_DIR})

# Include your custom headers
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/scene_loader.cpp
    src/radiosity.cpp
    src/monte_carlo.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
