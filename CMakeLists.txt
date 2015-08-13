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

# Build type
if(NOT CMAKE_BUILD_TYPE)
   message(STATUS "No build type selected, defaulting to Debug")
   set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

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
set(DATA_DIR_NAME "data")
set(DATA_DIR "${PROJECT_SOURCE_DIR}/${DATA_DIR_NAME}")
set(RES_DIR "${PROJECT_SOURCE_DIR}/resources")
set(BIN_RES_DIR "${PROJECT_BINARY_DIR}/resources")

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

# tinyobj
set(TINYOBJ_DIR "${DEP_DIR}/tinyobj")
attach_lib("${TINYOBJ_DIR}/include" "${TINYOBJ_DIR}/src/tiny_obj_loader.cc" "")

## Static ##

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")

# Boxer
set(BOXER_DIR "${DEP_DIR}/Boxer")
add_subdirectory(${BOXER_DIR})
attach_lib("${BOXER_DIR}/include" "" "Boxer")

# bullet
set(BULLET_DIR "${DEP_DIR}/bullet3")
set(USE_GRAPHICAL_BENCHMARK OFF CACHE INTERNAL "Use Graphical Benchmark")
set(BUILD_CPU_DEMOS OFF CACHE INTERNAL "Build original Bullet CPU examples")
set(USE_GLUT OFF CACHE INTERNAL "Use Glut")
set(BUILD_BULLET3 OFF CACHE INTERNAL "Set when you want to build Bullet 3")
set(BUILD_BULLET2_DEMOS OFF CACHE INTERNAL "Set when you want to build the Bullet 2 demos")
set(BUILD_EXTRAS OFF CACHE INTERNAL "Set when you want to build the extras")
set(INSTALL_LIBS OFF CACHE INTERNAL "Set when you want to install libraries")
set(INSTALL_EXTRA_LIBS OFF CACHE INTERNAL "Set when you want extra libraries installed")
set(BUILD_UNIT_TESTS OFF CACHE INTERNAL "Build Unit Tests")
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)")
add_subdirectory(${BULLET_DIR})

# Bullet doesn't give us a nice includes folder, so make one at build time
macro(list_subdirs result curdir)
   file(GLOB children RELATIVE ${curdir} ${curdir}/*)
   set(dirlist "")
   foreach(child ${children})
      if(IS_DIRECTORY ${curdir}/${child})
         list(APPEND dirlist ${child})
      endif(IS_DIRECTORY ${curdir}/${child})
   endforeach(child)
   set(${result} ${dirlist})
endmacro(list_subdirs)

function(copy_headers source dest)
   file(MAKE_DIRECTORY "${dest}")

   file(GLOB HEADERS "${source}/*.h")
   file(COPY ${HEADERS} DESTINATION "${dest}")

   list_subdirs(SUBDIRS ${source})
   foreach(subdir ${SUBDIRS})
      copy_headers("${source}/${subdir}" "${dest}/${subdir}")
   endforeach()
endfunction(copy_headers)

copy_headers(${BULLET_DIR}/src ${BIN_INCLUDE_DIR}/bullet)

list(APPEND BULLET_INCLUDES
   ${BULLET_DIR}/src
   ${BIN_INCLUDE_DIR}
)

attach_lib("${BULLET_INCLUDES}" "" "LinearMath")
attach_lib("${BULLET_INCLUDES}" "" "BulletCollision")
attach_lib("${BULLET_INCLUDES}" "" "BulletDynamics")

## Shared ##

set(BUILD_SHARED_LIBS ON CACHE INTERNAL "Build shared libraries")

# GLFW
set(GLFW_DIR "${DEP_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
set(GLFW_USE_CHDIR OFF CACHE INTERNAL "Make glfwInit chdir to Contents/Resources")
add_subdirectory(${GLFW_DIR})
attach_lib("${GLFW_DIR}/include" "" "glfw")

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

# Includes
include_directories(${INCLUDES})

# Binary
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Link
target_link_libraries(${PROJECT_NAME} ${LIBS})

### Post-Build ###

# Symlink data folder
if(WIN32)
   set(SYMLINK_COMMAND "${RES_DIR}/win/symlink.bat")
else(WIN32)
   set(SYMLINK_COMMAND "${CMAKE_COMMAND} -E create_symlink")
endif(WIN32)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${SYMLINK_COMMAND} "${DATA_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DATA_DIR_NAME}"
)

# Copy DLLs
if(WIN32)
   foreach(COPY_LIB ${COPY_LIBS})
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${COPY_LIB}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
      )
   endforeach(COPY_LIB)
endif(WIN32)

### Install ###

if (APPLE)
   set(APP_DIR "${CMAKE_INSTALL_PREFIX}/${PROJECT_DISPLAY_NAME}.app")
   set(CONTENTS_DIR "${APP_DIR}/Contents")
   set(MACOS_DIR "${CONTENTS_DIR}/MacOS")
   set(RESOURCES_DIR "${CONTENTS_DIR}/Resources")

   # Generate the .app file
   install(CODE "
      file(MAKE_DIRECTORY \"${MACOS_DIR}\")
      file(MAKE_DIRECTORY \"${RESOURCES_DIR}\")")

   # Generate the Info.plist file (at build time, not install time)
   configure_file(
     "${RES_DIR}/osx/Info.plist.in"
     "${BIN_RES_DIR}/Info.plist"
   )

   # Copy the generated Info.plist file
   install(FILES "${BIN_RES_DIR}/Info.plist" DESTINATION "${CONTENTS_DIR}")

   # Copy the icon
   install(FILES "${RES_DIR}/osx/${PROJECT_NAME}.icns" DESTINATION "${RESOURCES_DIR}")

   # Copy the data folder
   install(DIRECTORY "${DATA_DIR}" DESTINATION "${MACOS_DIR}")

   # Copy the executable (not install, so that the internal lib paths don't change)
   install(FILES "$<TARGET_FILE:${PROJECT_NAME}>" DESTINATION "${MACOS_DIR}"
           PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

   # Fix the bundle (copies and fixes shared libraries)
   install(CODE "
      include(BundleUtilities)
      fixup_bundle(\"${APP_DIR}\" \"\" \"\")")
endif (APPLE)

if (WIN32)
   # Copy the data folder
   install(DIRECTORY "${DATA_DIR}" DESTINATION ".")

   # Install the executable
   install(TARGETS ${PROJECT_NAME} DESTINATION ".")

   # Install all shared libraries
   foreach(COPY_LIB ${COPY_LIBS})
      install(FILES "$<TARGET_FILE:${COPY_LIB}>" DESTINATION ".")
   endforeach(COPY_LIB)

   # Install system libraries
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP true) # Let us choose the install location
   include(InstallRequiredSystemLibraries)
   if(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
      install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION ".")
   endif(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
endif (WIN32)

if (LINUX)
   install(TARGETS ${PROJECT_NAME} DESTINATION "." COMPONENT Runtime)                                                  # Install the executable

   install(CODE "include(BundleUtilities)
      file(COPY \"${PROJECT_SOURCE_DIR}/data\" DESTINATION \"${CMAKE_INSTALL_PREFIX}\")                                # Copy in the data folder
      execute_process(COMMAND \"chrpath\" \"-r\" \"$ORIGIN/\" \"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\")             # Fix the rpath of the executable
      fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}\" \"\" \"${EXECUTABLE_OUTPUT_PATH}\")" COMPONENT Runtime) # Fix the executable / libraries
endif (LINUX)
