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
set(DATA_DIR "${PROJECT_SOURCE_DIR}/data")

# Options
option(LOG_TO_FILE "Enable logging to a file" OFF)

# Generated content
configure_file (
  "${SRC_DIR}/Constants.h.in"
  "${BIN_INCLUDE_DIR}/Constants.h"
)

# Source content
set(SOURCES
   ${SRC_DIR}/AssetManager.cpp
   ${SRC_DIR}/Context.cpp
   ${SRC_DIR}/ControllerInputMap.cpp
   ${SRC_DIR}/DebugDrawer.cpp
   ${SRC_DIR}/DebugRenderer.cpp
   ${SRC_DIR}/FlyCameraComponent.cpp
   ${SRC_DIR}/GameObject.cpp
   ${SRC_DIR}/GameObjectMotionState.cpp
   ${SRC_DIR}/GeometricGraphicsComponent.cpp
   ${SRC_DIR}/HumanInputComponent.cpp
   ${SRC_DIR}/InputHandler.cpp
   ${SRC_DIR}/IOUtils.cpp
   ${SRC_DIR}/KeyboardInputMap.cpp
   ${SRC_DIR}/main.cpp
   ${SRC_DIR}/Mesh.cpp
   ${SRC_DIR}/MeshAssetManager.cpp
   ${SRC_DIR}/MeshPhysicsComponent.cpp
   ${SRC_DIR}/Model.cpp
   ${SRC_DIR}/PhongMaterial.cpp
   ${SRC_DIR}/PhysicsComponent.cpp
   ${SRC_DIR}/PhysicsManager.cpp
   ${SRC_DIR}/PlayerCameraComponent.cpp
   ${SRC_DIR}/PlayerPhysicsComponent.cpp
   ${SRC_DIR}/PointLightComponent.cpp
   ${SRC_DIR}/Renderer.cpp
   ${SRC_DIR}/Scene.cpp
   ${SRC_DIR}/Shader.cpp
   ${SRC_DIR}/ShaderAssetManager.cpp
   ${SRC_DIR}/ShaderProgram.cpp
)

set(HEADERS
   ${BIN_INCLUDE_DIR}/Constants.h
   ${SRC_DIR}/AssetManager.h
   ${SRC_DIR}/CameraComponent.h
   ${SRC_DIR}/Component.h
   ${SRC_DIR}/Context.h
   ${SRC_DIR}/ControllerInputMap.h
   ${SRC_DIR}/Conversions.h
   ${SRC_DIR}/DebugDrawer.h
   ${SRC_DIR}/DebugRenderer.h
   ${SRC_DIR}/FancyAssert.h
   ${SRC_DIR}/FlyCameraComponent.h
   ${SRC_DIR}/GameObject.h
   ${SRC_DIR}/GameObjectMotionState.h
   ${SRC_DIR}/GeometricGraphicsComponent.h
   ${SRC_DIR}/GLIncludes.h
   ${SRC_DIR}/GraphicsComponent.h
   ${SRC_DIR}/HumanInputComponent.h
   ${SRC_DIR}/InputComponent.h
   ${SRC_DIR}/InputHandler.h
   ${SRC_DIR}/InputMap.h
   ${SRC_DIR}/IOUtils.h
   ${SRC_DIR}/KeyboardInputMap.h
   ${SRC_DIR}/LightComponent.h
   ${SRC_DIR}/LogHelper.h
   ${SRC_DIR}/Material.h
   ${SRC_DIR}/Mesh.h
   ${SRC_DIR}/MeshPhysicsComponent.h
   ${SRC_DIR}/Model.h
   ${SRC_DIR}/MeshAssetManager.h
   ${SRC_DIR}/Observer.h
   ${SRC_DIR}/PhongMaterial.h
   ${SRC_DIR}/PhysicsComponent.h
   ${SRC_DIR}/PhysicsManager.h
   ${SRC_DIR}/PlayerCameraComponent.h
   ${SRC_DIR}/PlayerPhysicsComponent.h
   ${SRC_DIR}/PointLightComponent.h
   ${SRC_DIR}/Renderer.h
   ${SRC_DIR}/Scene.h
   ${SRC_DIR}/Shader.h
   ${SRC_DIR}/ShaderAssetManager.h
   ${SRC_DIR}/ShaderProgram.h
   ${SRC_DIR}/Subject.h
   ${SRC_DIR}/Transform.h
   ${SRC_DIR}/Types.h
)

set(SHADERS
   ${DATA_DIR}/shaders/phong_vert.glsl
   ${DATA_DIR}/shaders/phong_frag.glsl
)

### Libraries ###

# GLFW
set(GLFW_DIR "${DEP_DIR}/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")

if (APPLE)
   set(GLFW_LIBRARY ${GLFW_DIR}/lib/osx/libglfw.3.1.dylib)
   list(APPEND COPY_LIBRARIES
      ${GLFW_DIR}/lib/osx/libglfw.3.1.dylib
      ${GLFW_DIR}/lib/osx/libglfw.3.dylib
      ${GLFW_DIR}/lib/osx/libglfw.dylib
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
   # TODO Add proper support for MSVC14
   if (MSVC14)
      set(GLFW_LIBRARY ${GLFW_DIR}/lib/win32/lib-msvc120/glfw3dll.lib)
      list(APPEND COPY_LIBRARIES
         ${GLFW_DIR}/lib/win32/lib-msvc120/glfw3.dll
      )
   endif (MSVC14)
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

# GLM
set(GLM_DIR "${DEP_DIR}/glm")
set(GLM_INCLUDE_DIR "${GLM_DIR}/include")

# Folly
set(FOLLY_DIR "${DEP_DIR}/folly")
set(FOLLY_INCLUDE_DIR "${FOLLY_DIR}/include")

# Assimp
set(ASSIMP_DIR "${DEP_DIR}/assimp")
set(ASSIMP_INCLUDE_DIR "${ASSIMP_DIR}/include")

if (APPLE)
   set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/osx/libassimp.3.1.1.dylib")
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_DIR}/lib/osx/libassimp.3.1.1.dylib
   )
endif (APPLE)

if (WIN32)
   set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/win32/assimp.lib")
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_DIR}/lib/win32/assimp.dll
   )
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(ASSIMP_LIBRARY ${ASSIMP_DIR}/lib/linux/libassimp.so.3.1.1)
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_DIR}/lib/linux/libassimp.so.3.1.1
      ${ASSIMP_DIR}/lib/linux/libassimp.so.3
      ${ASSIMP_DIR}/lib/linux/libassimp.so
   )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# FMOD
set(FMOD_DIR "${DEP_DIR}/FMOD")
set(FMOD_INCLUDE_DIR "${FMOD_DIR}/include")

if (APPLE)
   if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY "${FMOD_DIR}/lib/osx/release/libfmod.dylib")
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/osx/release/libfmod.dylib
      )
   else (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY "${FMOD_DIR}/lib/osx/debug/libfmodL.dylib")
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/osx/debug/libfmodL.dylib
      )
   endif (CMAKE_BUILD_TYPE MATCHES "RELEASE")
endif (APPLE)

if (WIN32)
   if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY "${FMOD_DIR}/lib/win32/release/fmod_vc.lib")
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/win32/release/fmod.dll
      )
   else (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY "${FMOD_DIR}/lib/win32/debug/fmodL_vc.lib")
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/win32/debug/fmodL.dll
      )
   endif (CMAKE_BUILD_TYPE MATCHES "RELEASE")
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86_64")
   else (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86")
   endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

   if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.9)
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.9
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so
      )
   else (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(FMOD_LIBRARY ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.9)
      list(APPEND COPY_LIBRARIES
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.9
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5
         ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so
      )
   endif (CMAKE_BUILD_TYPE MATCHES "RELEASE")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# stb
set(STB_DIR "${DEP_DIR}/stb")
set(STB_INCLUDE_DIR "${STB_DIR}/include")

# cereal
set(CEREAL_DIR "${DEP_DIR}/cereal")
set(CEREAL_INCLUDE_DIR "${CEREAL_DIR}/include")

# bullet
set(BULLET_DIR "${DEP_DIR}/bullet")
set(BULLET_INCLUDE_DIR "${BULLET_DIR}/include" "${BULLET_DIR}/include/bullet")

if (APPLE)
   set(BULLET_LIBRARIES
      ${BULLET_DIR}/lib/osx/libLinearMath.a
      ${BULLET_DIR}/lib/osx/libBulletCollision.a
      ${BULLET_DIR}/lib/osx/libBulletDynamics.a
      ${BULLET_DIR}/lib/osx/libBulletSoftBody.a
   )
endif (APPLE)

if (WIN32)
   if (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(BULLET_LIBRARIES
         ${BULLET_DIR}/lib/win32/release/LinearMath.lib
         ${BULLET_DIR}/lib/win32/release/BulletCollision.lib
         ${BULLET_DIR}/lib/win32/release/BulletDynamics.lib
         ${BULLET_DIR}/lib/win32/release/BulletSoftBody.lib
      )
   else (CMAKE_BUILD_TYPE MATCHES "RELEASE")
      set(BULLET_LIBRARIES
         ${BULLET_DIR}/lib/win32/debug/LinearMath_Debug.lib
         ${BULLET_DIR}/lib/win32/debug/BulletCollision_Debug.lib
         ${BULLET_DIR}/lib/win32/debug/BulletDynamics_Debug.lib
         ${BULLET_DIR}/lib/win32/debug/BulletSoftBody_Debug.lib
      )
   endif (CMAKE_BUILD_TYPE MATCHES "RELEASE")
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(BULLET_LIBRARIES
      ${BULLET_DIR}/lib/linux/libBulletSoftBody.a
      ${BULLET_DIR}/lib/linux/libBulletDynamics.a
      ${BULLET_DIR}/lib/linux/libBulletCollision.a
      ${BULLET_DIR}/lib/linux/libLinearMath.a
   )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

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
   ${GLM_INCLUDE_DIR}
   ${FOLLY_INCLUDE_DIR}
   ${ASSIMP_INCLUDE_DIR}
   ${FMOD_INCLUDE_DIR}
   ${STB_INCLUDE_DIR}
   ${CEREAL_INCLUDE_DIR}
   ${BULLET_INCLUDE_DIR}
)

# Libraries
set(LIBRARIES
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARY}
   ${CMAKE_DL_LIBS} # Needed for glad on Linux
   ${BOXER_LIBRARY}
   ${ASSIMP_LIBRARY}
   ${FMOD_LIBRARY}
   ${BULLET_LIBRARIES}
)

# Binary
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${SHADERS})

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
