cmake_minimum_required(VERSION 2.8.11) # 2.8.11 is recommended for Qt 5.*


# Project name and properties
project(JoystickSupport)

set(JOYSTICKSUPPORT_VERSION_MAJOR 0)
set(JOYSTICKSUPPORT_VERSION_MINOR 1)
set(JOYSTICKSUPPORT_VERSION_PATCH 0)
set(JOYSTICKSUPPORT_VERSION ${JOYSTICKSUPPORT_VERSION_MAJOR}.${JOYSTICKSUPPORT_VERSION_MINOR}.${JOYSTICKSUPPORT_VERSION_PATCH})
add_definitions(-DJOYSTICKSUPPORT_VERSION_STRING="${JOYSTICKSUPPORT_VERSION}")

# By default, the plug-in is built against the development version...
# so ensure that the default value will be greater than any possible comparison.
set(STELLARIUM_VERSION 0.99.0 CACHE STRING
                       "Version number of the targeted Stellarium release")



# Required packages
if(${STELLARIUM_VERSION} VERSION_GREATER "0.12.4")
  # Everything after that version requires Qt 5
  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED) # For QImage even if there's no GUI.
  find_package(Qt5OpenGL REQUIRED) # For StelModule???
  set(QT_LINK_PARAMETERS Qt5::Core Qt5::Gui Qt5::OpenGL)
else()
  set(QT_MIN_VERSION "4.8.0")
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
  set(QT_LINK_PARAMETERS ${QT_LIBRARIES})
endif()

set (SDL_BUILDING_LIBRARY TRUE) # Otherwise SDL will insist on adding its main()
#find_package(SDL 2.0.1 REQUIRED) # This doesn't work (for now) so...
find_library(SDL2_LIBRARY NAMES SDL2 HINTS ${SDL2_DIR} ENV SDLDIR PATH_SUFFIXES lib)
if(NOT SDL2_LIBRARY)
  message(FATAL_ERROR "Unable to find SDL2 library files.")
endif(NOT SDL2_LIBRARY)
find_path(SDL2_INCLUDE_DIR SDL.h
          HINTS ${SDL2_DIR} ENV SDLDIR PATHS /usr/local/ PATH_SUFFIXES include/SDL2 include)
if(NOT SDL2_INCLUDE_DIR)
  message(FATAL_ERROR "Unable to find SDL2 include directory.")
endif(NOT SDL2_INCLUDE_DIR)


# Optional packages
find_file(GAME_CONTROLLER_DB_PATH NAME gamecontrollerdb.txt
                                  PATHS ${CMAKE_SOURCE_DIR}
                                  NO_DEFAULT_PATH)
if(GAME_CONTROLLER_DB_PATH)
  message(STATUS "Found game controller database: " ${GAME_CONTROLLER_DB_PATH})
  if(${Qt5Core_FOUND})
    qt5_add_resources(RESOURCES_SRCS resources.qrc)
  else()
    if(${QT_VERSION_MAJOR} EQUAL 4)
      qt4_add_resources(RESOURCES_SRCS resources.qrc)
    endif()
  endif()
endif()



# Stellarium paths and defines
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

string(REPLACE "." "_" STELLARIUM_VERSION_UNDERLINED ${STELLARIUM_VERSION})
string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1"
                     STELLARIUM_VERSION_MAJOR "${STELLARIUM_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1"
                     STELLARIUM_VERSION_MINOR "${STELLARIUM_VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1"
                     STELLARIUM_VERSION_PATCH "${STELLARIUM_VERSION}")
add_definitions(-DSTELLARIUM_${STELLARIUM_VERSION_UNDERLINED}
                -DSTELLARIUM_VERSION_MAJOR=${STELLARIUM_VERSION_MAJOR}
                -DSTELLARIUM_VERSION_MINOR=${STELLARIUM_VERSION_MINOR}
                -DSTELLARIUM_VERSION_PATCH=${STELLARIUM_VERSION_PATCH})

# Or the alternative method - check the version in CMake for specific features
if(${STELLARIUM_VERSION} VERSION_LESS "0.13.0")
  # StelFileMgr throws exceptions when a file is not found
  add_definitions(-DSTELFILEMGR_THROWS=1)
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
add_library(JoystickSupport MODULE ${JoystickSupport_SRCS} ${RESOURCES_SRCS})
set_target_properties(JoystickSupport PROPERTIES AUTOMOC TRUE)
if(UNIX AND NOT APPLE)
  target_link_libraries(JoystickSupport
                        ${QT_LINK_PARAMETERS}
                        #${SDL2_LIBRARY})
                        SDL2) # Static linking - it's a target name.
elseif(WIN32)
  target_link_libraries(JoystickSupport
                        ${QT_LINK_PARAMETERS}
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
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/.stellarium/)
  endif(UNIX AND NOT APPLE)
  if(WIN32)
    string(REPLACE "\\" "/" CMAKE_INSTALL_PREFIX $ENV{APPDATA})
    set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/Stellarium/)
  endif(WIN32)
endif(NOT CMAKE_INSTALL_PREFIX)

install(TARGETS JoystickSupport LIBRARY
                                DESTINATION modules/JoystickSupport/)
#install(FILES ${GAME_CONTROLLER_DB_PATH}
#                                DESTINATION modules/JoystickSupport/ OPTIONAL)



# Packaging
# Re-run CMake explicitly after modifying some of these, the changes will not
# take effect if CMake is invoked implicitly by make.
set(CPACK_PACKAGE_VERSION_MAJOR ${JOYSTICKSUPPORT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${JOYSTICKSUPPORT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${JOYSTICKSUPPORT_VERSION_PATCH})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "stellarium-joystick-${JOYSTICKSUPPORT_VERSION}")
set(CPACK_GENERATOR "")           # No binary package should be built with CPack
if(WIN32)
  set(CPACK_SOURCE_GENERATOR "ZIP")
else()
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()
set(CPACK_SOURCE_STRIP_FILES TRUE)
set(CPACK_SOURCE_IGNORE_FILES "/.git*"
                              ${PROJECT_BINARY_DIR}
                              "/CMakeLists.txt.user$"
                              "/gamecontrollerdb.txt$")
include(CPack)

# And the Inno Setup installer on Windows
if(WIN32)
  find_file(SDL2_DLL SDL2.dll
                     HINTS ${SDL2_DIR} ENV SDLDIR PATH_SUFFIXES bin lib)

  set(INNO_SETUP_WARNING "This file was generated by CMake. All changes wiil be lost the next time CMake is run.")
  configure_file(stellarium-joystick-win32.iss.cmake
                 stellarium-joystick-win32.iss @ONLY NEWLINE_STYLE WIN32)

  foreach(PLAINTEXT_FILE README.md LICENSE)
    get_filename_component(FILENAME ${PLAINTEXT_FILE} NAME_WE)
    configure_file(${PLAINTEXT_FILE} ${FILENAME}.txt @ONLY NEWLINE_STYLE WIN32)
  endforeach()
endif()
