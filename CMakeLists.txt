cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
project(WebGPU-with-Cpp)

# dawn
if(DEFINED ENV{DAWN_DIR})
    set(Dawn_DIR $ENV{DAWN_DIR})
else()
    message(FATAL_ERROR "DAWN_DIR environment variable is not set")
endif()
find_package(Dawn REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(glfw3webgpu)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

add_executable(${PROJECT_NAME}
        main.cpp
        src/webgpu-utils.h
        src/webgpu-utils.cpp
        src/Application2.cpp # src/Application.cpp
        src/Application.h
        src/createFVertices.cpp
        src/createFVertices.h
)

# add shaders to build directory
# Set shaders directory
set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

# Get list of all shader files
file(GLOB SHADER_FILES "${SHADERS_DIR}/*")

# Custom target to copy shaders
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/
        DEPENDS ${SHADER_FILES}
)

# Ensure shaders are copied after the build
add_dependencies(${PROJECT_NAME} copy_shaders)

target_link_libraries(${PROJECT_NAME}
        dawn::webgpu_dawn
        glfw
        glfw3webgpu
)


if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if (XCODE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            XCODE_GENERATE_SCHEME ON
            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
    )
endif()

if (EMSCRIPTEN)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PRIVATE -sASYNCIFY)
endif()
