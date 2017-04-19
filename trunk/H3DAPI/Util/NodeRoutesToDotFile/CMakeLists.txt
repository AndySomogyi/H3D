cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

# The name of our project is "NodeRoutesToDotFile".  CMakeLists files in this project can
# refer to the root source directory of the project as ${NodeRoutesToDotFile_SOURCE_DIR} and
# to the root binary directory of the project as ${NodeRoutesToDotFile_BINARY_DIR}.
project( NodeRoutesToDotFile )

# Add a cache variable NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES used
# to give extra release libraries which have a database which
# dot-files should be generated for.
if( NOT DEFINED NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES )
  set( NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES "" CACHE STRING "Add paths to extra release libraries that contain additional H3D nodes for which to generate dot-files. Each library path must be enclosed in \" and separated by semi colon( ; )" )
  mark_as_advanced( NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES )
endif()

# Add a cache variable NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES used
# to give extra debug libraries which have a database which
# dot-files should be generated for.
if( NOT DEFINED NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES )
  set( NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES "" CACHE STRING "Add paths to extra debug libraries that contain additional H3D nodes for which to generate dot-files. Each library path must be enclosed in \" and separated by semi colon( ; )" )
  mark_as_advanced( NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES )
endif()

set( NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES_CODE )
foreach( NodeRoutesToDotFile_EXTRA_LIBRARY ${NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES} )
  file( TO_CMAKE_PATH ${NodeRoutesToDotFile_EXTRA_LIBRARY} NodeRoutesToDotFile_EXTRA_LIBRARY_CMAKE_PATH )
  set( NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES_CODE "${NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES_CODE}\n  extra_libraries.push_back( ${NodeRoutesToDotFile_EXTRA_LIBRARY_CMAKE_PATH} );" )
endforeach()

set( NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES_CODE )
foreach( NodeRoutesToDotFile_EXTRA_LIBRARY ${NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES} )
  file( TO_CMAKE_PATH ${NodeRoutesToDotFile_EXTRA_LIBRARY} NodeRoutesToDotFile_EXTRA_LIBRARY_CMAKE_PATH )
  set( NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES_CODE "${NodeRoutesToDotFile_EXTRA_DEBUG_LIBRARIES_CODE}\n extra_libraries.push_back( ${NodeRoutesToDotFile_EXTRA_LIBRARY_CMAKE_PATH} );" )
endforeach()


# Where to find Source files.
set( NodeRoutesToDotFile_SRCS "${NodeRoutesToDotFile_SOURCE_DIR}/NodeRoutesToDotFile.cpp" )

# Add optional libs to this variable.
set( optional_libs )

# Include directories.
include_directories( . )

# Add required libs to this variable.
set( required_libs )

# Where to find modules, used when finding packages.
set( CMAKE_MODULE_PATH "${NodeRoutesToDotFile_SOURCE_DIR}/../../build/modules" )

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} )
  include_directories( ${H3DUTIL_INCLUDE_DIR} )
  include_directories( ${HAPI_INCLUDE_DIR} )
  include_directories( ${H3DAPI_INCLUDE_DIR} )
  include_directories( ${UI_INCLUDE_DIR} )
  include_directories( ${H3DPHYSICS_INCLUDE_DIR} )

  set( required_libs ${required_libs} H3DUtil HAPI H3DAPI UI H3DPhysics )
else()
  #H3DAPI
  find_package( H3DAPI REQUIRED )

  if( H3DAPI_FOUND )
    include_directories( ${H3DAPI_INCLUDE_DIRS} )
    set( required_libs ${required_libs} ${H3DAPI_LIBRARIES} )
  endif()

  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUtil_FOUND )
    include_directories( ${H3DUtil_INCLUDE_DIRS} )
    set( required_libs ${required_libs} ${H3DUtil_LIBRARIES} )
  endif()

  #HAPI
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIRS} )
    set( required_libs ${required_libs} ${HAPI_LIBRARIES} )
  endif()

  #UI
  find_package( UI )

  if( UI_FOUND )
    include_directories( ${UI_INCLUDE_DIRS} )
    set( optional_libs ${optional_libs} ${UI_LIBRARIES} )
  endif()

  #H3DPhysics
  find_package( H3DPhysics )

  if( H3DPhysics_FOUND )
    include_directories( ${H3DPhysics_INCLUDE_DIRS} )
    set( optional_libs ${optional_libs} ${H3DPhysics_LIBRARIES} )
  endif()
endif()

# Optional extra library. Used for sound in H3D API.
find_package( OpenAL )
if( OPENAL_FOUND )
  include_directories( ${OPENAL_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${OPENAL_LIBRARIES} )
endif()

configure_file( ${NodeRoutesToDotFile_SOURCE_DIR}/NodeRoutesToDotFile.cmake ${NodeRoutesToDotFile_SOURCE_DIR}/NodeRoutesToDotFile.cpp )

# Create build filess.
add_executable( NodeRoutesToDotFile  ${NodeRoutesToDotFile_SRCS} )
target_link_libraries( NodeRoutesToDotFile ${required_libs} ${optional_libs} )

# Debug version should have _d postfix.
set_target_properties( NodeRoutesToDotFile PROPERTIES DEBUG_POSTFIX "_d" )

# Variable used to set compile properties.
set( NODEROUTESTODOTFILE_COMPILE_FLAGS "-DH3D_GENERATE_DOTROUTE_FILES" )

getDefaultH3DOutputDirectoryName( default_bin_install default_lib_install )

if( WIN32 )

  if( UI_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( NODEROUTESTODOTFILE_COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS} -DHAVE_UI" )
  endif()

  if( H3DPhysics_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( NODEROUTESTODOTFILE_COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS} -DHAVE_H3DPhysics" )
  endif()

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( NODEROUTESTODOTFILE_COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS} /Zc:wchar_t" )

  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( NODEROUTESTODOTFILE_COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()

  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( NODEROUTESTODOTFILE_COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS} /MP" )
  endif()

endif()

set_target_properties( NodeRoutesToDotFile PROPERTIES COMPILE_FLAGS "${NODEROUTESTODOTFILE_COMPILE_FLAGS}" )

if( H3D_USE_DEPENDENCIES_ONLY )
  install( TARGETS NodeRoutesToDotFile
           LIBRARY DESTINATION ${default_lib_install}
           RUNTIME DESTINATION ${default_bin_install} )
endif()