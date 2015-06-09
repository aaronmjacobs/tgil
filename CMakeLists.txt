### Project Info ###

cmake_minimum_required(VERSION 2.8)
project(tgil)

set(PROJECT_DISPLAY_NAME "The Ground Is Lava!")

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
set(RES_DIR "${PROJECT_SOURCE_DIR}/resources")

# Custom executable output path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

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
   ${SRC_DIR}/AudioComponent.cpp
   ${SRC_DIR}/AudioManager.cpp
   ${SRC_DIR}/BvhMeshPhysicsComponent.cpp
   ${SRC_DIR}/Context.cpp
   ${SRC_DIR}/ControllerInputDevice.cpp
   ${SRC_DIR}/DebugDrawer.cpp
   ${SRC_DIR}/DebugRenderer.cpp
   ${SRC_DIR}/DynamicMesh.cpp
   ${SRC_DIR}/FlyCameraComponent.cpp
   ${SRC_DIR}/FlyCameraLogicComponent.cpp
   ${SRC_DIR}/Framebuffer.cpp
   ${SRC_DIR}/GameObject.cpp
   ${SRC_DIR}/GameObjectMotionState.cpp
   ${SRC_DIR}/GeometricGraphicsComponent.cpp
   ${SRC_DIR}/GhostPhysicsComponent.cpp
   ${SRC_DIR}/HUDRenderer.cpp
   ${SRC_DIR}/InputComponent.cpp
   ${SRC_DIR}/InputHandler.cpp
   ${SRC_DIR}/IOUtils.cpp
   ${SRC_DIR}/KeyMouseInputDevice.cpp
   ${SRC_DIR}/LightComponent.cpp
   ${SRC_DIR}/main.cpp
   ${SRC_DIR}/MenuLogicComponent.cpp
   ${SRC_DIR}/Mesh.cpp
   ${SRC_DIR}/MeshAssetManager.cpp
   ${SRC_DIR}/MeshPhysicsComponent.cpp
   ${SRC_DIR}/Model.cpp
   ${SRC_DIR}/OSUtils.cpp
   ${SRC_DIR}/PhongMaterial.cpp
   ${SRC_DIR}/PhysicsComponent.cpp
   ${SRC_DIR}/PhysicsManager.cpp
   ${SRC_DIR}/PlayerCameraComponent.cpp
   ${SRC_DIR}/PlayerGraphicsComponent.cpp
   ${SRC_DIR}/PlayerLogicComponent.cpp
   ${SRC_DIR}/PlayerPhysicsComponent.cpp
   ${SRC_DIR}/PostProcessRenderer.cpp
   ${SRC_DIR}/ProjectileLogicComponent.cpp
   ${SRC_DIR}/RenderData.cpp
   ${SRC_DIR}/Renderer.cpp
   ${SRC_DIR}/Scene.cpp
   ${SRC_DIR}/SceneLoader.cpp
   ${SRC_DIR}/Shader.cpp
   ${SRC_DIR}/ShaderAssetManager.cpp
   ${SRC_DIR}/ShaderProgram.cpp
   ${SRC_DIR}/ShadowMap.cpp
   ${SRC_DIR}/ShoveAbility.cpp
   ${SRC_DIR}/SkyRenderer.cpp
   ${SRC_DIR}/SunLogicComponent.cpp
   ${SRC_DIR}/TextRenderer.cpp
   ${SRC_DIR}/Texture.cpp
   ${SRC_DIR}/TextureAssetManager.cpp
   ${SRC_DIR}/TextureMaterial.cpp
   ${SRC_DIR}/TextureUnitManager.cpp
   ${SRC_DIR}/ThrowAbility.cpp
   ${SRC_DIR}/TimeMaterial.cpp
   ${SRC_DIR}/TintMaterial.cpp
)

set(HEADERS
   ${BIN_INCLUDE_DIR}/Constants.h
   ${SRC_DIR}/Ability.h
   ${SRC_DIR}/AssetManager.h
   ${SRC_DIR}/AudioComponent.h
   ${SRC_DIR}/AudioManager.h
   ${SRC_DIR}/BvhMeshPhysicsComponent.h
   ${SRC_DIR}/CameraComponent.h
   ${SRC_DIR}/Component.h
   ${SRC_DIR}/Context.h
   ${SRC_DIR}/ControllerInputDevice.h
   ${SRC_DIR}/Conversions.h
   ${SRC_DIR}/DebugDrawer.h
   ${SRC_DIR}/DebugRenderer.h
   ${SRC_DIR}/DefaultImageSource.h
   ${SRC_DIR}/DynamicMesh.h
   ${SRC_DIR}/FancyAssert.h
   ${SRC_DIR}/FlyCameraComponent.h
   ${SRC_DIR}/FlyCameraLogicComponent.h
   ${SRC_DIR}/Framebuffer.h
   ${SRC_DIR}/GameObject.h
   ${SRC_DIR}/GameObjectMotionState.h
   ${SRC_DIR}/GeometricGraphicsComponent.h
   ${SRC_DIR}/GhostPhysicsComponent.h
   ${SRC_DIR}/GLIncludes.h
   ${SRC_DIR}/GraphicsComponent.h
   ${SRC_DIR}/HUDRenderer.h
   ${SRC_DIR}/InputComponent.h
   ${SRC_DIR}/InputDevice.h
   ${SRC_DIR}/InputHandler.h
   ${SRC_DIR}/IOUtils.h
   ${SRC_DIR}/KeyMouseInputDevice.h
   ${SRC_DIR}/LightComponent.h
   ${SRC_DIR}/LogHelper.h
   ${SRC_DIR}/LogicComponent.h
   ${SRC_DIR}/Material.h
   ${SRC_DIR}/MenuLogicComponent.h
   ${SRC_DIR}/Mesh.h
   ${SRC_DIR}/MeshPhysicsComponent.h
   ${SRC_DIR}/Model.h
   ${SRC_DIR}/MeshAssetManager.h
   ${SRC_DIR}/Observer.h
   ${SRC_DIR}/OSUtils.h
   ${SRC_DIR}/PhongMaterial.h
   ${SRC_DIR}/PhysicsComponent.h
   ${SRC_DIR}/PhysicsManager.h
   ${SRC_DIR}/PlayerCameraComponent.h
   ${SRC_DIR}/PlayerGraphicsComponent.h
   ${SRC_DIR}/PlayerLogicComponent.h
   ${SRC_DIR}/PlayerPhysicsComponent.h
   ${SRC_DIR}/PostProcessRenderer.h
   ${SRC_DIR}/ProjectileLogicComponent.h
   ${SRC_DIR}/RenderData.h
   ${SRC_DIR}/Renderer.h
   ${SRC_DIR}/Scene.h
   ${SRC_DIR}/SceneLoader.h
   ${SRC_DIR}/Shader.h
   ${SRC_DIR}/ShaderAssetManager.h
   ${SRC_DIR}/ShaderProgram.h
   ${SRC_DIR}/ShadowMap.h
   ${SRC_DIR}/ShoveAbility.h
   ${SRC_DIR}/SkyRenderer.h
   ${SRC_DIR}/Subject.h
   ${SRC_DIR}/SunLogicComponent.h
   ${SRC_DIR}/TextRenderer.h
   ${SRC_DIR}/Texture.h
   ${SRC_DIR}/TextureAssetManager.h
   ${SRC_DIR}/TextureMaterial.h
   ${SRC_DIR}/TextureUnitManager.h
   ${SRC_DIR}/ThrowAbility.h
   ${SRC_DIR}/TimeMaterial.h
   ${SRC_DIR}/TintMaterial.h
   ${SRC_DIR}/Transform.h
   ${SRC_DIR}/Types.h
   ${SRC_DIR}/Viewport.h
)

### Libraries ###

# GLFW
set(GLFW_DIR "${DEP_DIR}/GLFW")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")

if (APPLE)
   set(GLFW_LIBRARY_RELEASE ${GLFW_DIR}/lib/osx/release/libglfw.3.1.dylib)
   set(GLFW_LIBRARY_DEBUG ${GLFW_DIR}/lib/osx/debug/libglfwd.3.1.dylib)
   list(APPEND COPY_LIBRARIES
      ${GLFW_LIBRARY_RELEASE}
      ${GLFW_LIBRARY_DEBUG}
   )
endif (APPLE)

if (WIN32)
   set(GLFW_LIBRARY_RELEASE ${GLFW_DIR}/lib/win32/release/glfw3dll.lib)
   set(GLFW_LIBRARY_DEBUG ${GLFW_DIR}/lib/win32/debug/glfw3ddll.lib)
   list(APPEND COPY_LIBRARIES
      ${GLFW_DIR}/lib/win32/release/glfw3.dll
      ${GLFW_DIR}/lib/win32/debug/glfw3d.dll
   )
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(GLFW_LIBRARY_RELEASE ${GLFW_DIR}/lib/linux/release/libglfw.so.3.1)
   set(GLFW_LIBRARY_DEBUG ${GLFW_DIR}/lib/linux/debug/libglfwd.so.3.1)
   list(APPEND COPY_LIBRARIES
      ${GLFW_DIR}/lib/linux/release/libglfw.so.3.1
      ${GLFW_DIR}/lib/linux/release/libglfw.so.3
      ${GLFW_DIR}/lib/linux/release/libglfw.so
      ${GLFW_DIR}/lib/linux/debug/libglfwd.so.3.1
      ${GLFW_DIR}/lib/linux/debug/libglfwd.so.3
      ${GLFW_DIR}/lib/linux/debug/libglfwd.so
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
   set(ASSIMP_LIBRARY_RELEASE "${ASSIMP_DIR}/lib/osx/release/libassimp.3.1.1.dylib")
   set(ASSIMP_LIBRARY_DEBUG "${ASSIMP_DIR}/lib/osx/debug/libassimpd.3.1.1.dylib")
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_LIBRARY_RELEASE}
      ${ASSIMP_LIBRARY_DEBUG}
   )
endif (APPLE)

if (WIN32)
   set(ASSIMP_LIBRARY_RELEASE "${ASSIMP_DIR}/lib/win32/release/assimp.lib")
   set(ASSIMP_LIBRARY_DEBUG "${ASSIMP_DIR}/lib/win32/debug/assimpd.lib")
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_DIR}/lib/win32/release/assimp.dll
      ${ASSIMP_DIR}/lib/win32/debug/assimpd.dll
   )
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(ASSIMP_LIBRARY_RELEASE ${ASSIMP_DIR}/lib/linux/release/libassimp.so.3.1.1)
   set(ASSIMP_LIBRARY_DEBUG ${ASSIMP_DIR}/lib/linux/debug/libassimpd.so.3.1.1)
   list(APPEND COPY_LIBRARIES
      ${ASSIMP_DIR}/lib/linux/release/libassimp.so.3.1.1
      ${ASSIMP_DIR}/lib/linux/release/libassimp.so.3
      ${ASSIMP_DIR}/lib/linux/release/libassimp.so
      ${ASSIMP_DIR}/lib/linux/debug/libassimpd.so.3.1.1
      ${ASSIMP_DIR}/lib/linux/debug/libassimpd.so.3
      ${ASSIMP_DIR}/lib/linux/debug/libassimpd.so
   )
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# FMOD
set(FMOD_DIR "${DEP_DIR}/FMOD")
set(FMOD_INCLUDE_DIR "${FMOD_DIR}/include")

if (APPLE)
   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/osx/release/libfmod.dylib")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/osx/debug/libfmodL.dylib")
   list(APPEND COPY_LIBRARIES
      ${FMOD_LIBRARY_RELEASE}
      ${FMOD_LIBRARY_DEBUG}
   )
endif (APPLE)

if (WIN32)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_WIN_ARCHITECTURE "x86_64")
   else (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_WIN_ARCHITECTURE "x86")
   endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/fmod_vc.lib")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/fmodL_vc.lib")
   list(APPEND COPY_LIBRARIES
      ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/fmod.dll
      ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/fmodL.dll
   )
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86_64")
   else (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LINUX_ARCHITECTURE "x86")
   endif (CMAKE_SIZEOF_VOID_P EQUAL 8)

   set(FMOD_LIBRARY_RELEASE "${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.10")
   set(FMOD_LIBRARY_DEBUG "${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.10")
   list(APPEND COPY_LIBRARIES
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.10
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.10
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5
      ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so
   )
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
   set(BULLET_LIBRARIES_RELEASE
      ${BULLET_DIR}/lib/osx/release/libLinearMath.a
      ${BULLET_DIR}/lib/osx/release/libBulletCollision.a
      ${BULLET_DIR}/lib/osx/release/libBulletDynamics.a
      ${BULLET_DIR}/lib/osx/release/libBulletSoftBody.a
   )
   set(BULLET_LIBRARIES_DEBUG
      ${BULLET_DIR}/lib/osx/debug/libLinearMath.a
      ${BULLET_DIR}/lib/osx/debug/libBulletCollision.a
      ${BULLET_DIR}/lib/osx/debug/libBulletDynamics.a
      ${BULLET_DIR}/lib/osx/debug/libBulletSoftBody.a
   )
endif (APPLE)

if (WIN32)
   set(BULLET_LIBRARIES_RELEASE
      ${BULLET_DIR}/lib/win32/release/LinearMath.lib
      ${BULLET_DIR}/lib/win32/release/BulletCollision.lib
      ${BULLET_DIR}/lib/win32/release/BulletDynamics.lib
      ${BULLET_DIR}/lib/win32/release/BulletSoftBody.lib
   )
   set(BULLET_LIBRARIES_DEBUG
      ${BULLET_DIR}/lib/win32/debug/LinearMath_Debug.lib
      ${BULLET_DIR}/lib/win32/debug/BulletCollision_Debug.lib
      ${BULLET_DIR}/lib/win32/debug/BulletDynamics_Debug.lib
      ${BULLET_DIR}/lib/win32/debug/BulletSoftBody_Debug.lib
   )
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(BULLET_LIBRARIES_RELEASE
      ${BULLET_DIR}/lib/linux/release/libBulletSoftBody.a
      ${BULLET_DIR}/lib/linux/release/libBulletDynamics.a
      ${BULLET_DIR}/lib/linux/release/libBulletCollision.a
      ${BULLET_DIR}/lib/linux/release/libLinearMath.a
   )
   set(BULLET_LIBRARIES_DEBUG
      ${BULLET_DIR}/lib/linux/debug/libBulletSoftBody.a
      ${BULLET_DIR}/lib/linux/debug/libBulletDynamics.a
      ${BULLET_DIR}/lib/linux/debug/libBulletCollision.a
      ${BULLET_DIR}/lib/linux/debug/libLinearMath.a
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
set(CMAKE_INSTALL_RPATH ${EXECUTABLE_OUTPUT_PATH})
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
set(RELEASE_LIBRARIES
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARY_RELEASE}
   ${CMAKE_DL_LIBS} # Needed for glad on Linux
   ${BOXER_LIBRARY}
   ${ASSIMP_LIBRARY_RELEASE}
   ${FMOD_LIBRARY_RELEASE}
   ${BULLET_LIBRARIES_RELEASE}
)

set(DEBUG_LIBRARIES
   ${OPENGL_LIBRARIES}
   ${GLFW_LIBRARY_DEBUG}
   ${CMAKE_DL_LIBS} # Needed for glad on Linux
   ${BOXER_LIBRARY}
   ${ASSIMP_LIBRARY_DEBUG}
   ${FMOD_LIBRARY_DEBUG}
   ${BULLET_LIBRARIES_DEBUG}
)

foreach(RELEASE_LIBRARY ${RELEASE_LIBRARIES})
   list(APPEND TAGGED_RELEASE_LIBRARIES
      optimized ${RELEASE_LIBRARY}
   )
endforeach(RELEASE_LIBRARY)

foreach(DEBUG_LIBRARY ${DEBUG_LIBRARIES})
   list(APPEND TAGGED_DEBUG_LIBRARIES
      debug ${DEBUG_LIBRARY}
   )
endforeach(DEBUG_LIBRARY)

# Binary
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${TAGGED_RELEASE_LIBRARIES} ${TAGGED_DEBUG_LIBRARIES})

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

# Install
if (APPLE)
   set(APP_DIR "${PROJECT_DISPLAY_NAME}.app")
   set(CONTENTS_DIR "${APP_DIR}/Contents")
   set(MACOS_DIR "${CONTENTS_DIR}/MacOS")

   install(CODE "file(MAKE_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/${MACOS_DIR}\")")                       # Make the .app file

   install(TARGETS ${PROJECT_NAME} DESTINATION "${MACOS_DIR}" COMPONENT Runtime)                       # Install the executable

   install(CODE "
      file(COPY \"${RES_DIR}/osx/Info.plist\" DESTINATION \"${CMAKE_INSTALL_PREFIX}/${CONTENTS_DIR}\") # Copy in the .plist file
      file(COPY \"${PROJECT_SOURCE_DIR}/data\" DESTINATION \"${CMAKE_INSTALL_PREFIX}/${MACOS_DIR}\")"  # Copy in the data folder
      COMPONENT Runtime)
                                                                                                       # Fix the executable / libraries
   install(CODE "include(BundleUtilities)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${APP_DIR}\" \"\" \"${EXECUTABLE_OUTPUT_PATH}/Release\")"
      COMPONENT Runtime CONFIGURATIONS Release)
   install(CODE "include(BundleUtilities)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${APP_DIR}\" \"\" \"${EXECUTABLE_OUTPUT_PATH}/Debug\")"
      COMPONENT Runtime CONFIGURATIONS Debug)
endif (APPLE)

if (WIN32)
   # Manually install system libs (so that they're not in bin/)
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP on)
   include(InstallRequiredSystemLibraries.cmake)
   if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION "." COMPONENT Runtime)
   endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)

   install(TARGETS ${PROJECT_NAME} DESTINATION "." COMPONENT Runtime)                    # Install the executable

   install(CODE "
      file(COPY \"${PROJECT_SOURCE_DIR}/data\" DESTINATION \"${CMAKE_INSTALL_PREFIX}\")" # Copy in the data folder
      COMPONENT Runtime)
                                                                                         # Fix the executable / libraries
   install(CODE "include(BundleUtilities)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.exe\" \"\" \"${EXECUTABLE_OUTPUT_PATH}/Release\")"
      COMPONENT Runtime CONFIGURATIONS Release)
   install(CODE "include(BundleUtilities)
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.exe\" \"\" \"${EXECUTABLE_OUTPUT_PATH}/Debug\")"
      COMPONENT Runtime CONFIGURATIONS Debug)
endif (WIN32)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   install(TARGETS ${PROJECT_NAME} DESTINATION "." COMPONENT Runtime)                                                  # Install the executable

   install(CODE "include(BundleUtilities)
      file(COPY \"${PROJECT_SOURCE_DIR}/data\" DESTINATION \"${CMAKE_INSTALL_PREFIX}\")                                # Copy in the data folder
      execute_process(COMMAND \"chrpath\" \"-r\" \"$ORIGIN/\" \"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\")             # Fix the rpath of the executable
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\" \"\" \"${EXECUTABLE_OUTPUT_PATH}\")" COMPONENT Runtime) # Fix the executable / libraries
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
