find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_library(gtk3 INTERFACE)
target_include_directories(gtk3 SYSTEM INTERFACE ${GTK3_INCLUDE_DIRS})
target_link_directories(gtk3 INTERFACE ${GTK3_LIBRARY_DIRS})
target_link_libraries(gtk3 INTERFACE ${GTK3_LIBRARIES})