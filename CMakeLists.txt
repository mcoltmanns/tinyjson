cmake_minimum_required(VERSION 4.0)

project(tinyjson VERSION 0.0 DESCRIPTION "Small JSON library written in/for C.")

add_library(tinyjson SHARED
    tinyjson.c
)

set_target_properties(tinyjson PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(tinyjson PROPERTIES PUBLIC_HEADER tinyjson.h)

include(GNUInstallDirs)

install(TARGETS tinyjson
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
