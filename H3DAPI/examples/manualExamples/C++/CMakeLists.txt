cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

# The name of our project is "ManualExamples". CMakeLists files in this project can
# refer to the root source directory of the project as ${ManualExamples_SOURCE_DIR} and
# to the root binary directory of the project as ${ManualExamples_BINARY_DIR}.
project( ManualExamples )

# Where to find modules used to find libraries.
set( CMAKE_MODULE_PATH "${ManualExamples_SOURCE_DIR}/../../../build/modules" )

include( H3DCommonFunctions )
handleCommonCacheVar( CMAKE_INSTALL_PREFIX ${ManualExamples_SOURCE_DIR}/../../../.. )

setupRPathForLib()

# Optional libraries to link against are added to this variable.
set( optional_libs )

# Required libraries to link against are added to this variable.
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

# Optional extra library. Makes is possible to used GLUT windows handling with H3D API.
find_package( GLUT REQUIRED )

if( GLUT_FOUND )
  include_directories( ${GLUT_INCLUDE_DIR} )
  if( WIN32 )
    # This is used in order for the compiler to find freeglut.lib.
    # If freeglut.lib is in any other place than glut32 on your system
    # either this CMakeLists.txt or FindGLUTWin needs to be changed
    # to allow for choice of extra library directory.
    stripAndAddLibraryDirectories( ${GLUT_LIBRARIES} )
  else()
    set( required_libs ${required_libs} ${GLUT_LIBRARIES} )
  endif()
endif()

# Build files created. Two executables. Sphere_X3D and Spheres_X3D.
add_executable( Sphere_X3D ${ManualExamples_SOURCE_DIR}/Sphere_X3D.cpp )
add_executable( Spheres_X3D ${ManualExamples_SOURCE_DIR}/Spheres_X3D.cpp )

# Link against libraries.
target_link_libraries( Sphere_X3D ${required_libs} ${optional_libs} )
target_link_libraries( Spheres_X3D ${required_libs} ${optional_libs} )

# Debug versions have _d postfix.
set_target_properties( Sphere_X3D PROPERTIES DEBUG_POSTFIX "_d" )
set_target_properties( Spheres_X3D PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
set( default_bin_install "bin" )
set( default_lib_install "lib" )

if( WIN32 )
  set( default_bin_install "bin32" )
  set( default_lib_install "lib32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( default_bin_install "bin64" )
    set( default_lib_install "lib64" )
  endif()

  # Set compile and link properties for projects.
  set( ManualExamples_COMPILE_FLAGS "" )

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} /Zc:wchar_t" )

  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()

  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( ManualExamples_COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS} /MP" )
  endif()

  set_target_properties( Sphere_X3D PROPERTIES COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS}" )
  set_target_properties( Spheres_X3D PROPERTIES COMPILE_FLAGS "${ManualExamples_COMPILE_FLAGS}" )
endif()

# Install to these directories.
install( TARGETS Sphere_X3D
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )

install( TARGETS Spheres_X3D
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )

