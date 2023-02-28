cmake_minimum_required(VERSION 3.23.0)
project(glenjin VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCES "src/*")
set(GLAD libs/glad/glad.c libs/glad/glad.h)
set(STB_IMAGE libs/image/stb_image.cpp libs/image/stb_image.h)

set(COMPILATION_UNIT ${SOURCES} ${GLAD} ${STB_IMAGE})

add_executable(${PROJECT_NAME} ${COMPILATION_UNIT})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/)

target_link_libraries(${PROJECT_NAME} OpenGL::GL ${SDL2_LIBRARIES})

