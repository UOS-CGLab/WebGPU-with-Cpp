cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
project(WebGPU-with-Cpp)

# dawn
set(Dawn_DIR "/Users/spoof_uos/workspace/dawn/install/Release/lib/cmake/Dawn")
find_package(Dawn REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(glfw3webgpu)

set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)

add_executable(${PROJECT_NAME}
        main.cpp
        src/webgpu-utils.h
        src/webgpu-utils.cpp
        src/Application.cpp
        src/Application.h
)

# add shaders to build directory
set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/src/shaders)
file(GLOB SHADER_FILES "${SHADERS_DIR}/*")
foreach(SHADER_FILE ${SHADER_FILES})
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders/
    )
endforeach()

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









add_executable(main_c main_c.c)
target_link_libraries(main_c glfw)
target_link_libraries(main_c dawn::webgpu_dawn)