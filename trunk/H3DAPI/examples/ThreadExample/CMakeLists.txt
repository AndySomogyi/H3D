cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

# The name of our project is "ThreadExample". CMakeLists files in this project can
# refer to the root source directory of the project as ${ThreadExample_SOURCE_DIR} and
# to the root binary directory of the project as ${ThreadExample_BINARY_DIR}.
project( ThreadExample )

# Where to find modules used to find libraries.
set( CMAKE_MODULE_PATH "${ThreadExample_SOURCE_DIR}/../../build/modules" )

include( H3DCommonFunctions )
handleCommonCacheVar( CMAKE_INSTALL_PREFIX ${ThreadExample_SOURCE_DIR}/../../.. )

setupRPathForLib()

# Optional libraries to link against are added to this variable.
set( optional_libs )

# Required libraries to link against are added to this variable.
set( required_libs )

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

# Build files created.
add_library( ThreadExample SHARED ${ThreadExample_SOURCE_DIR}/ThreadExample.h
                                 ${ThreadExample_SOURCE_DIR}/ThreadExample.cpp )

# Link against libraries.
target_link_libraries( ThreadExample ${required_libs} ${optional_libs} )

# Debug versions have _d postfix.
set_target_properties( ThreadExample PROPERTIES DEBUG_POSTFIX "_d" )

if( UNIX )
  set_target_properties( ThreadExample PROPERTIES OUTPUT_NAME threadexample )
endif()

if( MSVC )
  # Set compile and link properties for projects.
  set( ThreadExample_COMPILE_FLAGS "" )

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} /Zc:wchar_t" )

  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()

  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} /MP" )
  endif()

  set_target_properties( ThreadExample PROPERTIES COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS}" )
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

# Install to these directories.
install( TARGETS ThreadExample
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )

