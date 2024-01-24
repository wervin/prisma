include(ExternalProject)

ExternalProject_Add(
    poulpe-external
    GIT_REPOSITORY https://github.com/wervin/poulpe.git
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(poulpe-external SOURCE_DIR BINARY_DIR)

add_library(poulpe INTERFACE)
add_dependencies(poulpe poulpe-external)

target_include_directories(poulpe INTERFACE ${SOURCE_DIR}/inc)
target_link_libraries(poulpe INTERFACE ${BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}poulpe${CMAKE_SHARED_LIBRARY_SUFFIX})
