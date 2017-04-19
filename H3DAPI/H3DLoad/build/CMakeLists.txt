cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

# The name of our project is "H3DLoad".  CMakeLists files in this project can
# refer to the root source directory of the project as ${H3DLoad_SOURCE_DIR} and
# to the root binary directory of the project as ${H3DLoad_BINARY_DIR}.
project( H3DLoad )

# Where to find modules, used when finding packages.
set( CMAKE_MODULE_PATH "${H3DLoad_SOURCE_DIR}/../../build/modules" )

include( H3DCommonFunctions )
handleCommonCacheVar( CMAKE_INSTALL_PREFIX ${H3DLoad_SOURCE_DIR}/../../.. )

setupRPathForLib()

set( H3DLoad_MAJOR_VERSION "2" )
set( H3DLoad_MINOR_VERSION "3" )
set( H3DLoad_BUILD_VERSION "1" )

# Where to find Source files.
set( H3DLoad_SRCS "${H3DLoad_SOURCE_DIR}/../src/H3DLoad.cpp" )

# Add optional libs to this variable.
set( optional_libs )

# Include directories.
include_directories( ../src )

# Add required libs to this variable.
set( required_libs )

include( StripAndAddLibraryDirectories )

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} ) 
endif()

if( TARGET H3DUtil )
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} H3DUtil )
else()
  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUtil_FOUND )
    include_directories( ${H3DUtil_INCLUDE_DIRS} ) 
    set( required_libs ${required_libs} ${H3DUtil_LIBRARIES} )
  endif()
endif()

if( TARGET HAPI )
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} HAPI )
else()
  #HAPI
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIRS} ) 
    set( required_libs ${required_libs} ${HAPI_LIBRARIES} )
  endif()
endif()

if( TARGET H3DAPI )
  include_directories( ${H3DAPI_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} H3DAPI )
else()
  #H3DAPI
  find_package( H3DAPI REQUIRED )

  if( H3DAPI_FOUND )
    include_directories( ${H3DAPI_INCLUDE_DIRS} ) 
    set( required_libs ${required_libs} ${H3DAPI_LIBRARIES} )
  endif()
endif()

# Glut.
find_package( GLUT REQUIRED )

if( GLUT_FOUND )
  include_directories( ${GLUT_INCLUDE_DIR} )
  if( WIN32 )
    stripAndAddLibraryDirectories( ${GLUT_LIBRARIES} )
  else()
    set( required_libs ${required_libs} ${GLUT_LIBRARIES} )
  endif()
endif()
  
# OpenGL
find_package( OpenGL REQUIRED )

if( OPENGL_FOUND )
  include_directories( ${OPENGL_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} ${OPENGL_LIBRARIES} )
endif()

# Create build filess.
add_executable( H3DLoad  ${H3DLoad_SRCS} )
target_link_libraries( H3DLoad ${required_libs} ${optional_libs} )

# Debug version should have _d postfix.
set_target_properties( H3DLoad PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
if( MSVC ) 
  # Set compile and link properties for projects.
  set( H3DLoad_COMPILE_FLAGS "" )
  
  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( H3DLoad_COMPILE_FLAGS "${H3DLoad_COMPILE_FLAGS} /Zc:wchar_t" )
  
  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( H3DLoad_COMPILE_FLAGS "${H3DLoad_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()
  
  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( H3DLoad_COMPILE_FLAGS "${H3DLoad_COMPILE_FLAGS} /MP" )
  endif()
  
  set_target_properties( H3DLoad PROPERTIES COMPILE_FLAGS "${H3DLoad_COMPILE_FLAGS}" )
  set_target_properties( H3DLoad PROPERTIES LINK_FLAGS_DEBUG "/INCREMENTAL:NO /NODEFAULTLIB:msvcrt" )
  set_target_properties( H3DLoad PROPERTIES FOLDER "H3D" )
endif()
getDefaultH3DOutputDirectoryName( default_bin_install default_lib_install )

# Install to this destination.
install( TARGETS H3DLoad
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_runtime )

