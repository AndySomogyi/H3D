cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

# The name of our project is "ParticleDemo".  CMakeLists files in this project can
# refer to the root source directory of the project as ${ParticleDemo_SOURCE_DIR} and
# to the root binary directory of the project as ${ParticleDemo_BINARY_DIR}.
project( ParticleDemo )

# Where to find cmake modules used to find packages.
set( CMAKE_MODULE_PATH "${ParticleDemo_SOURCE_DIR}/../../../build/modules" )

include( H3DCommonFunctions )
handleCommonCacheVar( CMAKE_INSTALL_PREFIX ${ParticleDemo_SOURCE_DIR}/../../../.. )

setupRPathForLib()

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

set( tmp_h3d_include_dirs )
findIncludeDirsAndLibrariesForH3DProjects ( PROJECT_NAMES H3DUtil HAPI H3DAPI
                                            INCLUDE_DIRS_OUTPUT_VAR tmp_h3d_include_dirs
                                            LIBRARIES_OUTPUT_VAR required_libs
                                            REQUIRED_PROJECTS H3DUtil HAPI H3DAPI )
include_directories( ${tmp_h3d_include_dirs} ) 

# WxWidgets.
set( WXWINDOWS_USE_GL 1 )
find_package( wxWidgets COMPONENTS base core gl adv )
if( wxWidgets_FOUND )
  if( WIN32 )
    include_directories( ${wxWidgets_INCLUDE_DIR} )
  else()
    include( ${wxWidgets_USE_FILE} )
  endif()
  set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
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
getDefaultH3DOutputDirectoryName( default_bin_install default_lib_install )

install( TARGETS ParticleDemo
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )
