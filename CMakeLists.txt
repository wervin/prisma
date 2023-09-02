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
    GIT_TAG 1.89.8dock
    GIT_PROGRESS TRUE
)

if(NOT cimgui_POPULATED)
    message("populating cimgui")
    fetchcontent_populate(cimgui)
    add_subdirectory(${cimgui_SOURCE_DIR} ${cimgui_BUILD_DIR})
endif()

add_custom_target(cimgui_backend
    COMMAND luajit ./generator.lua gcc "internal" glfw vulkan
    WORKING_DIRECTORY ${cimgui_SOURCE_DIR}/generator
)

add_library(cimgui_core STATIC
    ${cimgui_SOURCE_DIR}/cimgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_demo.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${cimgui_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp
    ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
)

target_include_directories(cimgui_core PUBLIC
  ${cimgui_SOURCE_DIR}
  ${cimgui_SOURCE_DIR}/generator/output
  ${cimgui_SOURCE_DIR}/imgui
  ${cimgui_SOURCE_DIR}/imgui/backends
)

target_compile_definitions(cimgui_core PRIVATE 
    IMGUI_IMPL_API=extern\ \"C\"
)

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
    cimgui_core
    /usr/lib/x86_64-linux-gnu/libvulkan.so
    /usr/lib/x86_64-linux-gnu/libglfw.so)

target_include_directories(prisma PRIVATE
    inc)

set_property(TARGET prisma PROPERTY C_STANDARD 17)

target_compile_options(prisma PRIVATE 
    -Wall -Wextra -Wpedantic)

target_compile_definitions(prisma PRIVATE 
    IMGUI_IMPL_API=\ )

add_custom_target(default.vert.spv
    COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shaders/default.vert -o ${CMAKE_BINARY_DIR}/default.vert.spv)

add_custom_target(default.frag.spv
    COMMAND glslangValidator --quiet -V ${CMAKE_SOURCE_DIR}/shaders/default.frag -o ${CMAKE_BINARY_DIR}/default.frag.spv)

add_dependencies(prisma
    default.vert.spv
    default.frag.spv
    cimgui_backend)