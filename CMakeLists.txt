cmake_minimum_required(VERSION 3.10)

project(radium VERSION 1.0)

# Get OpenGL
find_package(OpenGL REQUIRED)

# Get GLUT
if (WIN32)
    set(CMAKE_PREFIX_PATH "C:\\tools\\msys64\\mingw64\\x86_64-w64-mingw32")
    set(GLUT_INCLUDE_DIR "C:\\tools\\msys64\\mingw64\\x86_64-w64-mingw32\\include")
endif()

find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Create executable
add_executable(radium main.cpp)
target_include_directories(radium PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(radium ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})