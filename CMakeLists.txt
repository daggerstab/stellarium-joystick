cmake_minimum_required(VERSION 2.8.11) # 2.8.11 is recommended for Qt 5.*


# Project name and properties
project(JoystickSupport) # TODO: Unnecessary?


# Required packages
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED) # For QImage even if there's no GUI.
find_package(Qt5OpenGL REQUIRED) # For StelModule???

set (SDL_BUILDING_LIBRARY TRUE) # Otherwise SDL will insist on adding its main()
#find_package(SDL 2.0.1 REQUIRED) # This doesn't work (for now) so...
find_library(SDL2_LIBRARY NAMES SDL2 HINTS ENV SDLDIR PATH_SUFFIXES lib)
if(NOT SDL2_LIBRARY)
  message(FATAL_ERROR "Unable to find SDL2 library files.")
endif(NOT SDL2_LIBRARY)
find_path(SDL2_INCLUDE_DIR SDL.h
          HINTS ENV SDLDIR PATHS /usr/local/ PATH_SUFFIXES include/SDL2 include)
if(NOT SDL2_INCLUDE_DIR)
  message(FATAL_ERROR "Unable to find SDL2 include directory.")
endif(NOT SDL2_INCLUDE_DIR)


# Optional packages
find_file(GAME_CONTROLLER_DB_PATH NAME gamecontrollerdb.txt
                                  PATHS ${CMAKE_SOURCE_DIR}
                                  NO_DEFAULT_PATH)

# Stellarium paths
if(NOT STELLARIUM_SOURCE_DIR)
  message(FATAL_ERROR "STELLARIUM_SOURCE_DIR must contain the path to the Stellarium /src directory")
endif(NOT STELLARIUM_SOURCE_DIR)
if(NOT STELLARIUM_BINARY_DIR)
  message(FATAL_ERROR "STELLARIUM_BINARY_DIR must contain the path to a directory containing a Stellarium binary (e.g. the Stellarium build directory)")
endif(NOT STELLARIUM_BINARY_DIR)

if(UNIX AND NOT APPLE)
# Temporarily link SDL2 as a static library until I can find out
# why Stellarium can't find it when it's linked dynamically.
# This is done by creating a fictitious target?
  add_library(SDL2 STATIC IMPORTED)
  set_target_properties(SDL2 PROPERTIES
                        IMPORTED_LOCATION /usr/local/lib/libSDL2.a)
endif()


# Source code
include_directories(${STELLARIUM_SOURCE_DIR}
                    ${STELLARIUM_SOURCE_DIR}/core
                    ${SDL2_INCLUDE_DIR}
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/src
                    ${STELLARIUM_BINARY_DIR}) #because of stelmain_export.h
                    #${STELLARIUM_SOURCE_DIR}/core/external
                    #${STELLARIUM_SOURCE_DIR}/core/modules

link_directories(${STELLARIUM_BINARY_DIR})

set(JoystickSupport_SRCS src/JoystickSupport.hpp
                         src/JoystickSupport.cpp)


# Building the binary
SET(CMAKE_CXX_FLAGS -Wall)
add_library(JoystickSupport MODULE ${JoystickSupport_SRCS})
set_target_properties(JoystickSupport PROPERTIES AUTOMOC TRUE)
if(UNIX AND NOT APPLE)
  target_link_libraries(JoystickSupport
                        Qt5::Core Qt5::Gui Qt5::OpenGL
                        #${SDL2_LIBRARY})
                        SDL2) # Static linking - it's a target name.
elseif(WIN32)
  target_link_libraries(JoystickSupport
                        Qt5::Core Qt5::Gui Qt5::OpenGL
                        ${SDL2_LIBRARY}
                        stelMain)
  if(MSVC)
    set_target_properties(JoystickSupport PROPERTIES PREFIX lib)
    # MinGW's gcc uses it by default, so Stellarium's plug-in system was
    # made to expect it.
  endif()
endif()

# Installation
if(NOT CMAKE_INSTALL_PREFIX)
  if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.stellarium/modules/)
  endif(UNIX AND NOT APPLE)
  if(WIN32)
    set(CMAKE_INSTALL_PREFIX $ENV{APPDATA}/Stellarium/modules/)
  endif(WIN32)
endif(NOT CMAKE_INSTALL_PREFIX)

install(TARGETS JoystickSupport LIBRARY DESTINATION JoystickSupport/)
install(FILES ${GAME_CONTROLLER_DB_PATH} DESTINATION JoystickSupport/ OPTIONAL)


# Packaging
# TODO
