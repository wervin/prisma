cmake_minimum_required(VERSION 3.14)

project(prisma)

add_subdirectory(externals/cglm)
add_subdirectory(externals/cimgui)
add_subdirectory(externals/glslang)
add_subdirectory(externals/gtk3)
add_subdirectory(externals/nativefiledialog)
add_subdirectory(externals/poulpe)
add_subdirectory(externals/sake)

set(PRISMA_SOURCES
    src/prisma/components/editor.c
    src/prisma/components/info.c
    src/prisma/components/logger.c
    src/prisma/components/menu.c
    src/prisma/application.c
    src/prisma/backend.c
    src/prisma/component.c
    src/prisma/error.c
    src/prisma/log.c
    src/prisma/main.c
    src/prisma/renderer.c
    src/prisma/style.c
    src/prisma/ui.c
    src/prisma/window.c)

add_executable(prisma ${PRISMA_SOURCES})

target_link_libraries(prisma
    m
    cglm
    glslang
    sake
    cimgui
    nfd
    gtk3
    poulpe
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(prisma PRIVATE
    inc)

set_property(TARGET prisma PROPERTY C_STANDARD 17)

target_compile_options(prisma PRIVATE 
    -Wall -Wextra -Wpedantic)

target_compile_definitions(prisma PRIVATE 
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    GLFW_INCLUDE_VULKAN
    CIMGUI_USE_GLFW
    CIMGUI_USE_VULKAN
)

add_custom_command(
    TARGET prisma POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_BINARY_DIR}/assets
)