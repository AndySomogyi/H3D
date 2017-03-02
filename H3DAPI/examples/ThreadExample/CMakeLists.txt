if(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
endif(WIN32)
# The name of our project is "ThreadExample". CMakeLists files in this project can
# refer to the root source directory of the project as ${ThreadExample_SOURCE_DIR} and
# to the root binary directory of the project as ${ThreadExample_BINARY_DIR}.
project(ThreadExample)

# set the install directory to the H3D directory on Windows
if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${ThreadExample_SOURCE_DIR}/../../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

# Optional libraries to link against are added to this variable.
set(optionalLibs)

# Required libraries to link against are added to this variable.
set(requiredLibs)

# Where to find modules used to find libraries.
set(CMAKE_MODULE_PATH "${ThreadExample_SOURCE_DIR}/../../build/modules/sharedModules")

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} ) 
endif( H3D_USE_DEPENDENCIES_ONLY )

if( TARGET H3DUtil )
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  set( requiredLibs ${requiredLibs} H3DUtil )
else( TARGET H3DUtil )
  #H3DUtil
  find_package(H3DUtil REQUIRED)

  if(H3DUTIL_FOUND)
    include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${H3DUTIL_LIBRARIES} )
  endif(H3DUTIL_FOUND)
endif( TARGET H3DUtil )

if( TARGET HAPI )
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  set( requiredLibs ${requiredLibs} HAPI )
else( TARGET HAPI )
  #HAPI
  find_package(HAPI REQUIRED)

  if(HAPI_FOUND)
    include_directories( ${HAPI_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${HAPI_LIBRARIES} )
  endif(HAPI_FOUND)
endif( TARGET HAPI )

if( TARGET H3DAPI )
  include_directories( ${H3DAPI_INCLUDE_DIR} ) 
  set( requiredLibs ${requiredLibs} H3DAPI )
else( TARGET H3DAPI )
  #H3DAPI
  find_package(H3DAPI REQUIRED)

  if(H3DAPI_FOUND)
    include_directories( ${H3DAPI_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${H3DAPI_LIBRARIES} )
  endif(H3DAPI_FOUND)
endif( TARGET H3DAPI )

# Build files created.
add_library(ThreadExample SHARED ${ThreadExample_SOURCE_DIR}/ThreadExample.h
                                 ${ThreadExample_SOURCE_DIR}/ThreadExample.cpp )

# Link against libraries.
target_link_libraries( ThreadExample ${requiredLibs} ${optionalLibs} )

# Debug versions have _d postfix.
set_target_properties( ThreadExample PROPERTIES DEBUG_POSTFIX "_d" )

if(UNIX)
  set_target_properties( ThreadExample PROPERTIES OUTPUT_NAME threadexample )
endif(UNIX)

if(MSVC)
  # Set compile and link properties for projects.
  set( ThreadExample_COMPILE_FLAGS "" )
  
  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 ( so far ) but we still set it for all.
  set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} /Zc:wchar_t")
  
  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif( ${MSVC_VERSION} GREATER 1399 )
  
  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( ThreadExample_COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS} /MP" )
  endif( ${MSVC_VERSION} GREATER 1499 )
  
  set_target_properties( ThreadExample PROPERTIES COMPILE_FLAGS "${ThreadExample_COMPILE_FLAGS}" )
endif(MSVC)

set( DEFAULT_BIN_INSTALL "bin" )
set( DEFAULT_LIB_INSTALL "lib" )

if( WIN32 )
  set( DEFAULT_BIN_INSTALL "bin32" )
  set( DEFAULT_LIB_INSTALL "lib32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( DEFAULT_BIN_INSTALL "bin64" )
    set( DEFAULT_LIB_INSTALL "lib64" )
  endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
endif( WIN32 )

# Install to these directories.
install( TARGETS ThreadExample
         LIBRARY DESTINATION ${DEFAULT_LIB_INSTALL}
         RUNTIME DESTINATION ${DEFAULT_BIN_INSTALL}
         COMPONENT H3DAPI_cpack_examples_runtime )

