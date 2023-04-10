cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(terrain)

add_subdirectory("lib/glfw")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/include
)

add_executable(
terrain 
# Project
src/shader.cpp src/main.cpp src/camera.cpp src/settings.cpp 
src/terrain.cpp src/water.cpp src/point_light.cpp

# Imgui
lib/imgui/src/imgui_demo.cpp lib/imgui/src/imgui_draw.cpp lib/imgui/src/imgui_impl_glfw.cpp lib/imgui/src/imgui_impl_opengl3.cpp lib/imgui/src/imgui_tables.cpp
lib/imgui/src/imgui_widgets.cpp lib/imgui/src/imgui.cpp

# stb image
lib/stb_image/stb_image.cpp

# GLAD
lib/glad/glad.c
)

target_link_libraries(terrain PUBLIC ${OPENGL_LIBRARIES} glfw)