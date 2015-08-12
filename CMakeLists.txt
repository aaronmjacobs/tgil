### Project Info ###

cmake_minimum_required(VERSION 3.1)
project(tgil)

set(PROJECT_DISPLAY_NAME "The Ground Is Lava!")

# Version info
set(VERSION_TYPE "Pre-Alpha")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_MICRO 0)
set(VERSION_BUILD 1)

# Compiler requirements
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(LINUX true)
else(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(LINUX false)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(64_BIT true)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
   set(64_BIT false)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

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

list(APPEND INCLUDES
   ${SRC_DIR}
   ${BIN_INCLUDE_DIR}
)

### Libraries ###

function(attach_lib include source lib)
   if(include)
      list(APPEND INCLUDES ${include})
      set(INCLUDES ${INCLUDES} PARENT_SCOPE)
   endif(include)

   if(source)
      list(APPEND SOURCES ${source})
      set(SOURCES ${SOURCES} PARENT_SCOPE)
   endif(source)

   if(lib)
      list(APPEND LIBS ${lib})
      set(LIBS ${LIBS} PARENT_SCOPE)

      if(BUILD_SHARED_LIBS)
         list(APPEND COPY_LIBS ${lib})
         set(COPY_LIBS ${COPY_LIBS} PARENT_SCOPE)
      endif(BUILD_SHARED_LIBS)
   endif(lib)
endfunction(attach_lib)

function(set_import_locations target lib_release lib_debug imp_release imp_debug)
   if(lib_release)
      set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_RELEASE ${lib_release})
   endif(lib_release)

   if(lib_debug)
      set_target_properties(${target} PROPERTIES IMPORTED_LOCATION_DEBUG ${lib_debug})
   endif(lib_debug)

   if(imp_release)
      set_target_properties(${target} PROPERTIES IMPORTED_IMPLIB_RELEASE ${imp_release})
   endif(imp_release)

   if(imp_debug)
      set_target_properties(${target} PROPERTIES IMPORTED_IMPLIB_DEBUG ${imp_debug})
   endif(imp_debug)
endfunction(set_import_locations)

## Integrated ##

# cereal
set(CEREAL_DIR "${DEP_DIR}/cereal")
attach_lib("${CEREAL_DIR}/include" "" "")

# Folly
set(FOLLY_DIR "${DEP_DIR}/folly")
attach_lib("${FOLLY_DIR}/include" "" "")

# glad
set(GLAD_DIR "${DEP_DIR}/glad")
attach_lib("${GLAD_DIR}/include" "${GLAD_DIR}/src/glad.c" "")

# GLM
set(GLM_DIR "${DEP_DIR}/glm")
attach_lib("${GLM_DIR}/include" "" "")

# Pempek Assert
set(PEMPEK_DIR "${DEP_DIR}/PempekAssert")
attach_lib("${PEMPEK_DIR}/include" "${PEMPEK_DIR}/lib/pempek_assert.cpp" "")

# stb
set(STB_DIR "${DEP_DIR}/stb")
attach_lib("${STB_DIR}/include" "" "")

# templog
set(TEMPLOG_DIR "${DEP_DIR}/templog")
attach_lib("${TEMPLOG_DIR}/include" "${TEMPLOG_DIR}/imp/logging.cpp" "")

## Static ##

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")

# Boxer
set(BOXER_DIR "${DEP_DIR}/Boxer")
add_subdirectory(${BOXER_DIR})
attach_lib("${BOXER_DIR}/include" "" "Boxer")

# bullet
set(BULLET_DIR "${DEP_DIR}/bullet")
set(LINEAR_MATH_TARGET LinearMath)
set(BULLET_COLLISION_TARGET BulletCollision)
set(BULLET_DYNAMICS_TARGET BulletDynamics)
set(BULLET_SOFT_BODY_TARGET BulletSoftBody)

add_library(${LINEAR_MATH_TARGET} STATIC IMPORTED)
add_library(${BULLET_COLLISION_TARGET} STATIC IMPORTED)
add_library(${BULLET_DYNAMICS_TARGET} STATIC IMPORTED)
add_library(${BULLET_SOFT_BODY_TARGET} STATIC IMPORTED)

if (APPLE)
   set(OS_LIB_DIR osx)
endif (APPLE)
if (WIN32)
   set(OS_LIB_DIR win32)
endif (WIN32)
if (LINUX)
   set(OS_LIB_DIR linux)
endif (LINUX)

set(LINEAR_MATH_LIB_RELEASE ${BULLET_DIR}/lib/${OS_LIB_DIR}/release/libLinearMath.a)
set(BULLET_COLLISION_LIB_RELEASE ${BULLET_DIR}/lib/${OS_LIB_DIR}/release/libBulletCollision.a)
set(BULLET_DYNAMICS_LIB_RELEASE ${BULLET_DIR}/lib/${OS_LIB_DIR}/release/libBulletDynamics.a)
set(BULLET_SOFT_BODY_LIB_RELEASE ${BULLET_DIR}/lib/${OS_LIB_DIR}/release/libBulletSoftBody.a)

set(LINEAR_MATH_LIB_DEBUG ${BULLET_DIR}/lib/${OS_LIB_DIR}/debug/libLinearMath.a)
set(BULLET_COLLISION_LIB_DEBUG ${BULLET_DIR}/lib/${OS_LIB_DIR}/debug/libBulletCollision.a)
set(BULLET_DYNAMICS_LIB_DEBUG ${BULLET_DIR}/lib/${OS_LIB_DIR}/debug/libBulletDynamics.a)
set(BULLET_SOFT_BODY_LIB_DEBUG ${BULLET_DIR}/lib/${OS_LIB_DIR}/debug/libBulletSoftBody.a)

set_import_locations(${LINEAR_MATH_TARGET} ${LINEAR_MATH_LIB_RELEASE} ${LINEAR_MATH_LIB_DEBUG} "" "")
set_import_locations(${BULLET_COLLISION_TARGET} ${BULLET_COLLISION_LIB_RELEASE} ${BULLET_COLLISION_LIB_DEBUG} "" "")
set_import_locations(${BULLET_DYNAMICS_TARGET} ${BULLET_DYNAMICS_LIB_RELEASE} ${BULLET_DYNAMICS_LIB_DEBUG} "" "")
set_import_locations(${BULLET_SOFT_BODY_TARGET} ${BULLET_SOFT_BODY_LIB_RELEASE} ${BULLET_SOFT_BODY_LIB_DEBUG} "" "")

list(APPEND BULLET_INCLUDES
   "${BULLET_DIR}/include"
   "${BULLET_DIR}/include/bullet"
)

attach_lib("${BULLET_INCLUDES}" "" "${LINEAR_MATH_TARGET}")
attach_lib("${BULLET_INCLUDES}" "" "${BULLET_COLLISION_TARGET}")
attach_lib("${BULLET_INCLUDES}" "" "${BULLET_DYNAMICS_TARGET}")
attach_lib("${BULLET_INCLUDES}" "" "${BULLET_SOFT_BODY_TARGET}")

## Shared ##

set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

# GLFW
set(GLFW_DIR "${DEP_DIR}/GLFW")
set(GLFW_TARGET glfw)
add_library(${GLFW_TARGET} SHARED IMPORTED)

set(GLFW_IMP_RELEASE false)
set(GLFW_IMP_DEBUG false)

if (APPLE)
   set(GLFW_LIB_RELEASE ${GLFW_DIR}/lib/osx/release/libglfw.3.1.dylib)
   set(GLFW_LIB_DEBUG ${GLFW_DIR}/lib/osx/release/libglfw.3.1.dylib)
endif (APPLE)

if (WIN32)
   set(GLFW_LIB_RELEASE ${GLFW_DIR}/lib/win32/release/glfw3.dll)
   set(GLFW_LIB_DEBUG ${GLFW_DIR}/lib/win32/debug/glfw3d.dll)
   set(GLFW_IMP_RELEASE ${GLFW_DIR}/lib/win32/release/glfw3dll.lib)
   set(GLFW_IMP_DEBUG ${GLFW_DIR}/lib/win32/debug/glfw3ddll.lib)
endif (WIN32)

if (LINUX)
   set(GLFW_LIB_RELEASE ${GLFW_DIR}/lib/linux/release/libglfw.so.3.1)
   set(GLFW_LIB_DEBUG ${GLFW_DIR}/lib/linux/debug/libglfwd.so.3.1)
endif (LINUX)

set_import_locations(${GLFW_TARGET} ${GLFW_LIB_RELEASE} ${GLFW_LIB_DEBUG} ${GLFW_IMP_RELEASE} ${GLFW_IMP_DEBUG})
attach_lib("${GLFW_DIR}/include" "" "${GLFW_TARGET}")

# Assimp
set(ASSIMP_DIR "${DEP_DIR}/assimp")
set(ASSIMP_TARGET assimp)
add_library(${ASSIMP_TARGET} SHARED IMPORTED)

set(ASSIMP_IMP_RELEASE false)
set(ASSIMP_IMP_DEBUG false)

if (APPLE)
   set(ASSIMP_LIB_RELEASE ${ASSIMP_DIR}/lib/osx/release/libassimp.3.1.1.dylib)
   set(ASSIMP_LIB_DEBUG ${ASSIMP_DIR}/lib/osx/debug/libassimpd.3.1.1.dylib)
endif (APPLE)

if (WIN32)
   set(ASSIMP_LIB_RELEASE ${ASSIMP_DIR}/lib/win32/release/assimp.dll)
   set(ASSIMP_LIB_DEBUG ${ASSIMP_DIR}/lib/win32/debug/assimpd.dll)
   set(ASSIMP_IMP_RELEASE ${ASSIMP_DIR}/lib/win32/release/assimp.lib)
   set(ASSIMP_IMP_DEBUG ${ASSIMP_DIR}/lib/win32/debug/assimpd.lib)
endif (WIN32)

if (LINUX)
   set(ASSIMP_LIB_RELEASE ${ASSIMP_DIR}/lib/linux/release/libassimp.so.3.1.1)
   set(ASSIMP_LIB_DEBUG ${ASSIMP_DIR}/lib/linux/debug/libassimpd.so.3.1.1)
endif (LINUX)

set_import_locations(${ASSIMP_TARGET} ${ASSIMP_LIB_RELEASE} ${ASSIMP_LIB_DEBUG} ${ASSIMP_IMP_RELEASE} ${ASSIMP_IMP_DEBUG})
attach_lib("${ASSIMP_DIR}/include" "" "${ASSIMP_TARGET}")

# FMOD
set(FMOD_DIR "${DEP_DIR}/FMOD")
set(FMOD_TARGET fmod)
add_library(${FMOD_TARGET} SHARED IMPORTED)

set(FMOD_IMP_RELEASE false)
set(FMOD_IMP_DEBUG false)

if (APPLE)
   set(FMOD_LIB_RELEASE ${FMOD_DIR}/lib/osx/release/libfmod.dylib)
   set(FMOD_LIB_DEBUG ${FMOD_DIR}/lib/osx/debug/libfmodL.dylib)
endif (APPLE)

if (WIN32)
   if (64_BIT)
      set(FMOD_WIN_ARCHITECTURE "x86_64")
   else (64_BIT)
      set(FMOD_WIN_ARCHITECTURE "x86")
   endif (64_BIT)

   set(FMOD_LIB_RELEASE ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/fmod.dll)
   set(FMOD_LIB_DEBUG ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/fmodL.dll)
   set(FMOD_IMP_RELEASE ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/release/fmod_vc.lib)
   set(FMOD_IMP_DEBUG ${FMOD_DIR}/lib/win32/${FMOD_WIN_ARCHITECTURE}/debug/fmodL_vc.lib)
endif (WIN32)

if (LINUX)
   if (64_BIT)
      set(FMOD_LINUX_ARCHITECTURE "x86_64")
   else (64_BIT)
      set(FMOD_LINUX_ARCHITECTURE "x86")
   endif (64_BIT)

   set(FMOD_LIB_RELEASE ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/release/libfmod.so.5.10)
   set(FMOD_LIB_DEBUG ${FMOD_DIR}/lib/linux/${FMOD_LINUX_ARCHITECTURE}/debug/libfmodL.so.5.10)
endif (LINUX)

set_import_locations(${FMOD_TARGET} ${FMOD_LIB_RELEASE} ${FMOD_LIB_DEBUG} ${FMOD_IMP_RELEASE} ${FMOD_IMP_DEBUG})
attach_lib("${FMOD_DIR}/include" "" "${FMOD_TARGET}")

### Building ###

# Fix the rpath
set(CMAKE_INSTALL_RPATH ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Includes
include_directories(${INCLUDES})

# Binary
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBS})

### Post-Build ###

# Copy DLLs
#if(WIN32)
   foreach(COPY_LIB ${COPY_LIBS})
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${COPY_LIB}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
      )
   endforeach(COPY_LIB)
#endif(WIN32)

### Install ###

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

if (LINUX)
   install(TARGETS ${PROJECT_NAME} DESTINATION "." COMPONENT Runtime)                                                  # Install the executable

   install(CODE "include(BundleUtilities)
      file(COPY \"${PROJECT_SOURCE_DIR}/data\" DESTINATION \"${CMAKE_INSTALL_PREFIX}\")                                # Copy in the data folder
      execute_process(COMMAND \"chrpath\" \"-r\" \"$ORIGIN/\" \"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\")             # Fix the rpath of the executable
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\" \"\" \"${EXECUTABLE_OUTPUT_PATH}\")" COMPONENT Runtime) # Fix the executable / libraries
endif (LINUX)
