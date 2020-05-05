cmake_minimum_required(VERSION 3.10)

# project definition
project(pcv_docker_poc)

# find the required libraries
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
message(STATUS "${GLEW_LIBRARIES}")
add_executable(pcv_docker_poc src/main.cpp)
target_link_libraries(pcv_docker_poc ${OPENGL_LIBRARIES} ${OPENGL_egl_LIBRARY}
                      ${GLEW_LIBRARIES})
