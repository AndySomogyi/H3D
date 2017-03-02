if(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
endif(WIN32)
# The name of our project is "ManualExamples". CMakeLists files in this project can
# refer to the root source directory of the project as ${ManualExamples_SOURCE_DIR} and
# to the root binary directory of the project as ${ManualExamples_BINARY_DIR}.
project (ManualExamples)

if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${ManualExamples_SOURCE_DIR}/../../../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
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
set(CMAKE_MODULE_PATH ${ManualExamples_SOURCE_DIR}/../../../build/modules ${ManualExamples_SOURCE_DIR}/../../../build/modules/sharedModules )
if( COMMAND cmake_policy )
  if( POLICY CMP0011 )
    cmake_policy( SET CMP0011 NEW )
  endif( POLICY CMP0011 )
endif( COMMAND cmake_policy )
include( StripAndAddLibraryDirectories )

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

# Optional extra library. Makes is possible to used GLUT windows handling with H3D API.
find_package(GLUT REQUIRED)

if(GLUT_FOUND)
  include_directories( ${GLUT_INCLUDE_DIR} )
  if( WIN32 )
    # This is used in order for the compiler to find freeglut.lib.
    # If freeglut.lib is in any other place than glut32 on your system
    # either this CMakeLists.txt or FindGLUTWin needs to be changed
    # to allow for choice of extra library directory.
    STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${GLUT_LIBRARIES} )
  else( WIN32 )
    set(requiredLibs ${requiredLibs} ${GLUT_LIBRARIES} )
  endif( WIN32 )
endif(GLUT_FOUND)

# Build files created. Two executables. Sphere_X3D and Spheres_X3D.
add_executable(Sphere_X3D ${ManualExamples_SOURCE_DIR}/Sphere_X3D.cpp )
add_executable(Spheres_X3D ${ManualExamples_SOURCE_DIR}/Spheres_X3D.cpp )

# Link against libraries.
target_link_libraries( Sphere_X3D ${requiredLibs} ${optionalLibs} )
target_link_libraries( Spheres_X3D ${requiredLibs} ${optionalLibs} )

# Debug versions have _d postfix.
set_target_properties( Sphere_X3D PROPERTIES DEBUG_POSTFIX "_d" )
set_target_properties( Spheres_X3D PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
set( DEFAULT_BIN_INSTALL "bin" )
set( DEFAULT_LIB_INSTALL "lib" )

if( WIN32 )
  set( DEFAULT_BIN_INSTALL "bin32" )
  set( DEFAULT_LIB_INSTALL "lib32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( DEFAULT_BIN_INSTALL "bin64" )
    set( DEFAULT_LIB_INSTALL "lib64" )
  endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  
  # Set compile and link properties for projects.
  set( ManualExamples_COMPILE_FLAGS "" )
  
  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 ( so far ) but we still set it for all.
  set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} /Zc:wchar_t")
  
  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif( ${MSVC_VERSION} GREATER 1399 )
  
  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} /MP" )
  endif( ${MSVC_VERSION} GREATER 1499 )
  
  set_target_properties( Sphere_X3D PROPERTIES COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS}" )
  set_target_properties( Spheres_X3D PROPERTIES COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS}" )
endif(WIN32)

# Install to these directories.
install( TARGETS Sphere_X3D
         LIBRARY DESTINATION ${DEFAULT_LIB_INSTALL}
         RUNTIME DESTINATION ${DEFAULT_BIN_INSTALL}
         COMPONENT H3DAPI_cpack_examples_runtime )

install( TARGETS Spheres_X3D
         LIBRARY DESTINATION ${DEFAULT_LIB_INSTALL}
         RUNTIME DESTINATION ${DEFAULT_BIN_INSTALL}
         COMPONENT H3DAPI_cpack_examples_runtime )

