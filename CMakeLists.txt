cmake_minimum_required(VERSION 2.8.11) # 2.8.11 is recommended for Qt 5.*

# Project name and properties
project(JoystickSupport) # TODO: Unnecessary?

# Required packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED) # For QImage even if there's no GUI.
find_package(Qt5OpenGL REQUIRED) # For StelModule???

set (SDL_BUILDING_LIBRARY TRUE) # Otherwise SDL will insist on adding its main()
#find_package(SDL REQUIRED)


# Stellarium paths
if(NOT STELLARIUM_SOURCE_DIR)
    message(FATAL_ERROR "STELLARIUM_SOURCE_DIR must contain the path to the Stellarium /src directory")
endif(NOT STELLARIUM_SOURCE_DIR)
if(NOT STELLARIUM_BINARY_DIR)
    message(FATAL_ERROR "STELLARIUM_BINARY_DIR must contain the path to a directory containing a Stellarium binary (e.g. the Stellarium build directory)")
endif(NOT STELLARIUM_BINARY_DIR)


# Source code
include_directories(${STELLARIUM_SOURCE_DIR}
                    ${STELLARIUM_SOURCE_DIR}/core
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/src)
                    #${STELLARIUM_SOURCE_DIR}/core/external
                    #${STELLARIUM_SOURCE_DIR}/core/modules

link_directories(${STELLARIUM_BINARY_DIR})

set(JoystickSupport_SRCS src/JoystickSupport.hpp
                         src/JoystickSupport.cpp)


# Building the binary
SET(CMAKE_CXX_FLAGS -Wall)
add_library(JoystickSupport MODULE ${JoystickSupport_SRCS})
set_target_properties(JoystickSupport PROPERTIES AUTOMOC TRUE)
target_link_libraries(JoystickSupport Qt5::Core Qt5::Gui Qt5::OpenGL)


# Installation
# TODO


# Packaging
# TODO
