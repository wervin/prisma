ExternalProject_Add(
    glslang-external
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG sdk-1.3.261.0
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(glslang-external SOURCE_DIR BINARY_DIR)

add_library(glslang INTERFACE)
add_dependencies(glslang glslang-external)

target_include_directories(glslang INTERFACE ${SOURCE_DIR}/inc)
target_link_libraries(glslang INTERFACE ${BINARY_DIR}/glslang/${CMAKE_STATIC_LIBRARY_PREFIX}glslang${CMAKE_STATIC_LIBRARY_SUFFIX})
