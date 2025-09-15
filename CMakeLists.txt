cmake_minimum_required(VERSION 4.0)

project(tinyjson VERSION 0.0 DESCRIPTION "Small JSON library written in/for C.")
add_subdirectory(test)

# LIBRARY BUILD/INSTALL
add_library(tinyjson SHARED
        src/tinyjson.c
)

set_target_properties(tinyjson PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(tinyjson PROPERTIES PUBLIC_HEADER src/tinyjson.h)

include(GNUInstallDirs)

install(TARGETS tinyjson
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
