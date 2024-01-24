include(ExternalProject)

ExternalProject_Add(
    nfd-external
    GIT_REPOSITORY https://github.com/mlabbe/nativefiledialog.git
    GIT_TAG release_116
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cd build/gmake_linux && make config=release_x64
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""
)

ExternalProject_Get_property(nfd-external SOURCE_DIR)

add_library(nfd INTERFACE)
add_dependencies(nfd nfd-external)

target_include_directories(nfd SYSTEM INTERFACE ${SOURCE_DIR}/src/include)
target_link_libraries(nfd INTERFACE ${SOURCE_DIR}/build/lib/Release/x64/${CMAKE_STATIC_LIBRARY_PREFIX}nfd${CMAKE_STATIC_LIBRARY_SUFFIX})