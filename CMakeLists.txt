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
set(BIN_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

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
)

### Building ###

# Includes
include_directories(
   ${SRC_DIR}
   ${BIN_INCLUDE_DIR}
)

# Binary
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})