cmake_minimum_required(VERSION 3.0.0)

project(prisma)

include(FetchContent)

set(FETCHCONTENT_QUIET OFF)

fetchcontent_declare(
    cglm
    GIT_REPOSITORY https://github.com/recp/cglm.git
    GIT_TAG v0.9.1
    GIT_PROGRESS TRUE
)

if(NOT cglm_POPULATED)
    message("populating cglm")
    fetchcontent_populate(cglm)
    add_subdirectory(${cglm_SOURCE_DIR} ${cglm_BUILD_DIR})
endif()

fetchcontent_declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG 1.89.5
    GIT_PROGRESS TRUE
)

if(NOT cimgui_POPULATED)
    message("populating cimgui")
    fetchcontent_populate(cimgui)
    add_subdirectory(${cimgui_SOURCE_DIR} ${cimgui_BUILD_DIR})
endif()

fetchcontent_declare(
    glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG sdk-1.3.261.0
    GIT_PROGRESS TRUE
)

if(NOT glslang_POPULATED)
    message("populating glslang")
    fetchcontent_populate(glslang)
    add_subdirectory(${glslang_SOURCE_DIR} ${glslang_BUILD_DIR})
endif()

fetchcontent_declare(
    sake
    GIT_REPOSITORY https://github.com/wervin/sake.git
    GIT_TAG v0.0.2
    GIT_PROGRESS TRUE
)

if(NOT sake_POPULATED)
    message("populating sake")
    fetchcontent_populate(sake)
    add_subdirectory(${sake_SOURCE_DIR} ${sake_BUILD_DIR})
endif()

set(PRISMA_SOURCES
    src/prisma/backend/buffers/indexbuffer.c
    src/prisma/backend/buffers/uniformbuffer.c
    src/prisma/backend/buffers/vertexbuffer.c
    src/prisma/backend/buffers/buffer.c
    src/prisma/backend/models/vertex2d.c
    src/prisma/backend/commandbuffers.c
    src/prisma/backend/commandpool.c
    src/prisma/backend/descriptorpool.c
    src/prisma/backend/descriptorsetlayout.c
    src/prisma/backend/descriptorsets.c
    src/prisma/backend/device.c
    src/prisma/backend/framebuffers.c
    src/prisma/backend/instance.c
    src/prisma/backend/pipeline.c
    src/prisma/backend/pipelinelayout.c
    src/prisma/backend/renderpass.c
    src/prisma/backend/shader.c
    src/prisma/backend/swapchain.c
    src/prisma/backend/sync.c
    src/prisma/application.c
    src/prisma/error.c
    src/prisma/log.c
    src/prisma/main.c
    src/prisma/renderer.c
    src/prisma/ui.c
    src/prisma/window.c)

add_executable(prisma ${PRISMA_SOURCES})

target_link_libraries(prisma
    m
    cglm
    glslang
    sake
    cimgui
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(prisma PRIVATE
    inc)

set_property(TARGET prisma PROPERTY C_STANDARD 17)

target_compile_options(prisma PRIVATE 
    -Wall -Wextra -Wpedantic)

add_custom_target(default.vert.spv
    COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shaders/default.vert -o ${CMAKE_BINARY_DIR}/default.vert.spv)

add_custom_target(default.frag.spv
    COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shaders/default.frag -o ${CMAKE_BINARY_DIR}/default.frag.spv)

add_dependencies(prisma
    default.vert.spv
    default.frag.spv)