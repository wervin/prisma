ExternalProject_Add(
    glslang-external
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG 14.0.0
    CMAKE_ARGS -DENABLE_OPT=OFF
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(glslang-external SOURCE_DIR BINARY_DIR)

ExternalProject_Add_Step(
    glslang-external
    bootstrap
    COMMAND python3 update_glslang_sources.py
    DEPENDEES download
    WORKING_DIRECTORY ${SOURCE_DIR}
)

add_library(glslang INTERFACE)
add_dependencies(glslang glslang-external)

target_include_directories(glslang INTERFACE ${SOURCE_DIR})
target_link_libraries(glslang INTERFACE 
    ${BINARY_DIR}/glslang/${CMAKE_STATIC_LIBRARY_PREFIX}glslang${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/glslang/${CMAKE_STATIC_LIBRARY_PREFIX}glslang-default-resource-limits${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/glslang/${CMAKE_STATIC_LIBRARY_PREFIX}MachineIndependent${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/glslang/${CMAKE_STATIC_LIBRARY_PREFIX}GenericCodeGen${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/glslang/OSDependent/Unix/${CMAKE_STATIC_LIBRARY_PREFIX}OSDependent${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/SPIRV/${CMAKE_STATIC_LIBRARY_PREFIX}SPVRemapper${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${BINARY_DIR}/SPIRV/${CMAKE_STATIC_LIBRARY_PREFIX}SPIRV${CMAKE_STATIC_LIBRARY_SUFFIX}
)
