cmake_minimum_required(VERSION 2.8)

project(ArRender)
add_compile_options(-std=c++11)
set(CMAKE_BUILD_TYPE Release)
add_executable(${PROJECT_NAME} "./example/ArRender.cpp"
"./src/Camera.cpp"
"./src/Model3D.cpp"
"./src/Renderer.cpp")
include_directories( "./include"
"/usr/include" )
set(GL_LIBS glapi glut GL GLU)
find_package(OpenCV REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/examples)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${GL_LIBS})
