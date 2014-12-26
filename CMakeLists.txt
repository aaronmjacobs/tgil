### Project Info ###

cmake_minimum_required(VERSION 2.8)
project(Struggle)

# Version info
set(VERSION_TYPE "Pre-Alpha")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_MICRO 0)
set(VERSION_BUILD 1)

# Directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/source")
set(DEP_DIR "${PROJECT_SOURCE_DIR}/dependencies")
set(BIN_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

# Options
option(LOG_TO_FILE "Enable logging to a file" OFF)

# Generated content
configure_file (
  "${SRC_DIR}/Constants.h.in"
  "${BIN_INCLUDE_DIR}/Constants.h"
)

# Source content
set(SOURCES
   ${SRC_DIR}/main.cpp
)

set(HEADERS
   ${BIN_INCLUDE_DIR}/Constants.h
   ${SRC_DIR}/GLIncludes.h
   ${SRC_DIR}/FancyAssert.h
)

### Libraries ###

# GLFW
set(GLFW_DIR "${DEP_DIR}/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")

if (APPLE)
   set(GLFW_LIBRARY ${GLFW_DIR}/lib/osx/libglfw.3.1.dylib)
   list(APPEND COPY_LIBRARIES
      ${GLFW_DIR}/lib/osx/libglfw.3.1.dylib
   )
endif (APPLE)

if (WIN32)
   if (MSVC11)
      set(GLFW_LIBRARY ${GLFW_DIR}/lib/win32/lib-msvc110/glfw3dll.lib)
      list(APPEND COPY_LIBRARIES
         ${GLFW_DIR}/lib/win32/lib-msvc110/glfw3.dll
      )
   endif (MSVC11)
   if (MSVC12)
      set(GLFW_LIBRARY ${GLFW_DIR}/lib/win32/lib-msvc120/glfw3dll.lib)
      list(APPEND COPY_LIBRARIES
         ${GLFW_DIR}/lib/win32/lib-msvc120/glfw3.dll
      )
   endif (MSVC12)
   # TODO Add support for MSVC14
   if (MINGW)
      set(GLFW_LIBRARY ${GLFW_DIR}/lib/win32/lib-mingw/glfw3dll.a)
      list(APPEND COPY_LIBRARIES
         ${GLFW_DIR}/lib/win32/lib-mingw/glfw3.dll
      )
   endif (MINGW)
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   # TODO Grab glfw 3.1 for Linux
   set(GLFW_LIBRARY ${GLFW_DIR}/lib/linux/libglfw.so.3.0)
   list(APPEND COPY_LIBRARIES
      ${GLFW_DIR}/lib/linux/libglfw.so.3.0
      ${GLFW_DIR}/lib/linux/libglfw.so.3
      ${GLFW_DIR}/lib/linux/libglfw.so
   )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# glad
set(GLAD_DIR "${DEP_DIR}/glad")
set(GLAD_INCLUDE_DIR "${GLAD_DIR}/include")
list(APPEND SOURCES
   ${GLAD_DIR}/src/glad.c
)

# Pempek Assert
set(PEMPEK_DIR "${DEP_DIR}/PempekAssert")
set(PEMPEK_INCLUDE_DIR "${PEMPEK_DIR}/include")
list(APPEND SOURCES
   ${PEMPEK_DIR}/lib/pempek_assert.cpp
)

# Boxer
set(BOXER_DIR "${DEP_DIR}/Boxer")
set(BOXER_INCLUDE_DIR "${BOXER_DIR}/include")
set(BOXER_LIBRARY Boxer)
add_subdirectory(${BOXER_DIR})

# templog
set(TEMPLOG_DIR "${DEP_DIR}/templog")
set(TEMPLOG_INCLUDE_DIR "${TEMPLOG_DIR}/include")
list(APPEND SOURCES
   ${TEMPLOG_DIR}/imp/logging.cpp
)

### Building ###

# C++11
if (NOT WIN32)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (NOT WIN32)

# OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OpenGL not found!")
endif(NOT OPENGL_FOUND)

# Fix the rpath
set(CMAKE_INSTALL_RPATH ${PROJECT_BINARY_DIR})
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Includes
include_directories(
   ${SRC_DIR}
   ${BIN_INCLUDE_DIR}
   ${GLFW_INCLUDE_DIR}
   ${GLAD_INCLUDE_DIR}
   ${PEMPEK_INCLUDE_DIR}
   ${BOXER_INCLUDE_DIR}
   ${TEMPLOG_INCLUDE_DIR}
)

# Libraries
set(LIBRARIES
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARY}
   ${BOXER_LIBRARY}
)

# Binary
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Copy libraries to binary dir
if (COPY_LIBRARIES)
   foreach(COPY_LIBRARY ${COPY_LIBRARIES})
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${COPY_LIBRARY}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
      )
   endforeach(COPY_LIBRARY)
endif (COPY_LIBRARIES)