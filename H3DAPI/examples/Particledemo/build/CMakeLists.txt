if( WIN32 )
  cmake_minimum_required( VERSION 2.6.0 )
endif()

if( COMMAND cmake_policy )
  if( POLICY CMP0003 )
    cmake_policy( SET CMP0003 NEW )
  endif()
endif()

# The name of our project is "ParticleDemo".  CMakeLists files in this project can
# refer to the root source directory of the project as ${ParticleDemo_SOURCE_DIR} and
# to the root binary directory of the project as ${ParticleDemo_BINARY_DIR}.
project( ParticleDemo )

# set the install directory to the H3D directory on Windows
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${ParticleDemo_SOURCE_DIR}/../../../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif()

# use, i.e. don't skip the full RPATH for the build tree
set( CMAKE_SKIP_BUILD_RPATH FALSE )

# when building, don't use the install RPATH already
# (but later on when installing)
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# the RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir )
if( "${isSystemDir}" STREQUAL "-1" )
   set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif()

# Source files.
set( ParticleDemo_SRCS
       "${ParticleDemo_SOURCE_DIR}/../src/ConsoleDialog.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/ConsoleDialog.h"
       "${ParticleDemo_SOURCE_DIR}/../src/EmitterSettings.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/EmitterSettings.h"
       "${ParticleDemo_SOURCE_DIR}/../src/Envini.h"
       "${ParticleDemo_SOURCE_DIR}/../src/ParticleDemo.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/PhysicsModels.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/PhysicsModels.h"
       "${ParticleDemo_SOURCE_DIR}/../src/WxFrame.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/WxFrame.h"
       "${ParticleDemo_SOURCE_DIR}/../src/WxWidgetsWindow.cpp"
       "${ParticleDemo_SOURCE_DIR}/../src/WxWidgetsWindow.h" )

# Include directories.
include_directories( ../src )

# Add required libraries to this variable.
set( required_libs )

# Where to find cmake modules used to find packages.
set( CMAKE_MODULE_PATH "${ParticleDemo_SOURCE_DIR}/../../../build/modules/sharedModules" )

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} )
endif()

if( TARGET H3DUtil )
  include_directories( ${H3DUTIL_INCLUDE_DIR} )
  set( required_libs ${required_libs} H3DUtil )
else( TARGET H3DUtil )
  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUTIL_FOUND )
    include_directories( ${H3DUTIL_INCLUDE_DIR} )
    set( required_libs ${required_libs} ${H3DUTIL_LIBRARIES} )
  endif()
endif()

if( TARGET HAPI )
  include_directories( ${HAPI_INCLUDE_DIR} )
  set( required_libs ${required_libs} HAPI )
else( TARGET HAPI )
  #HAPI
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIR} )
    set( required_libs ${required_libs} ${HAPI_LIBRARIES} )
  endif()
endif()

if( TARGET H3DAPI )
  include_directories( ${H3DAPI_INCLUDE_DIR} )
  set( required_libs ${required_libs} H3DAPI )
else( TARGET H3DAPI )
  #H3DAPI
  find_package( H3DAPI REQUIRED )

  if( H3DAPI_FOUND )
    include_directories( ${H3DAPI_INCLUDE_DIR} )
    set( required_libs ${required_libs} ${H3DAPI_LIBRARIES} )
  endif()
endif()

# WxWidgets.
set( TMP_wxWidgets_FOUND FALSE )
set( WXWINDOWS_USE_GL 1 )
if( WIN32 )
  find_package( wxWidgetsWin )
  if( wxWidgets_FOUND )
    include_directories( ${wxWidgets_INCLUDE_DIR} )
    set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
    set( TMP_wxWidgets_FOUND TRUE )
  endif()
endif()

if( TMP_wxWidgets_FOUND )
else( TMP_wxWidgets_FOUND )
  set( wxWidgets_USE_LIBS base core gl adv )
  find_package( wxWidgets REQUIRED )
  if( wxWidgets_FOUND )
    include( ${wxWidgets_USE_FILE} )
    set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
  endif()
endif()

# Create build files.
add_executable( ParticleDemo WIN32 MACOSX_BUNDLE ${ParticleDemo_SRCS} )

# Link against required_libs and optional_libs.
target_link_libraries( ParticleDemo ${required_libs} ${optional_libs} )

# Set postfix for debug version of H3DViewer.
set_target_properties( ParticleDemo PROPERTIES DEBUG_POSTFIX "_d" )

if( MSVC )
  # Set compile and link properties for projects.
  set( ParticleDemo_COMPILE_FLAGS "-DWXUSINGDLL" )

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( ParticleDemo_COMPILE_FLAGS "${ParticleDemo_COMPILE_FLAGS} /Zc:wchar_t" )

  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( ParticleDemo_COMPILE_FLAGS "${ParticleDemo_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()

  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( ParticleDemo_COMPILE_FLAGS "${ParticleDemo_COMPILE_FLAGS} /MP" )
  endif()

  if( ${MSVC_VERSION} LESS 1500 )
    # Turn off optimization for visual studio versions 8 and below.
    # Old setting which I (Markus) have no idea why it was added.
    set( ParticleDemo_COMPILE_FLAGS "${ParticleDemo_COMPILE_FLAGS} /Od" )
  endif()

  set_target_properties( ParticleDemo PROPERTIES COMPILE_FLAGS "${ParticleDemo_COMPILE_FLAGS}" )
  # Remove link warning in debug build.
  set_target_properties( ParticleDemo PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt" )
endif()

set( default_bin_install "bin" )
set( default_lib_install "lib" )

if( WIN32 )
  set( default_bin_install "bin32" )
  set( default_lib_install "lib32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( default_bin_install "bin64" )
    set( default_lib_install "lib64" )
  endif()
endif()

install( TARGETS ParticleDemo
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )
