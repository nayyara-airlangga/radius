cmake_minimum_required(VERSION 3.10)

project(radius VERSION 1.0)

# Get OpenGL
find_package(OpenGL REQUIRED)

# Get GLUT
if (WIN32)
    set(CMAKE_PREFIX_PATH "C:\\tools\\msys64\\mingw64\\x86_64-w64-mingw32")
    set(GLUT_INCLUDE_DIR "C:\\tools\\msys64\\mingw64\\x86_64-w64-mingw32\\include")
endif()

find_package(GLUT REQUIRED)

# Set source files
set(PROJ_SOURCE_FILES main.cpp texture.cpp trig.cpp)

# Create executable
add_executable(radius ${PROJ_SOURCE_FILES})

target_include_directories(radius PRIVATE ${PROJECT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
target_link_libraries(radius ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

# Copy textures
add_custom_target(copy_textures
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/CopyTextures.cmake
)
add_dependencies(radius copy_textures)