cmake_minimum_required(VERSION 3.20)

project(OpenGL3D LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# GLFW
# ============================================================

set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")

add_library(glfw STATIC IMPORTED)

set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION
        "${GLFW_DIR}/lib-mingw-w64/libglfw3.a"

    INTERFACE_INCLUDE_DIRECTORIES
        "${GLFW_DIR}/include"
)

# ============================================================
# GLAD
# ============================================================

add_library(glad STATIC
    external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# ============================================================
# GLM
# ============================================================

add_library(glm INTERFACE)

target_include_directories(glm INTERFACE
    "${CMAKE_SOURCE_DIR}/external/glm"
)

# ============================================================
# Application
# ============================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(OpenGL3D ${SOURCES})

target_link_libraries(OpenGL3D PRIVATE
    glfw
    glad
    glm
)

target_include_directories(OpenGL3D PRIVATE
    "${CMAKE_SOURCE_DIR}/external"
)

# Windows libraries required by GLFW
target_link_libraries(OpenGL3D PRIVATE
    opengl32
    gdi32
    user32
    shell32
)

# ============================================================
# Copy GLFW DLL beside executable
# ============================================================

add_custom_command(TARGET OpenGL3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_DIR}/lib-mingw-w64/glfw3.dll"
        "$<TARGET_FILE_DIR:OpenGL3D>"
)