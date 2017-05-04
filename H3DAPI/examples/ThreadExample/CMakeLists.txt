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

set( tmp_h3d_include_dirs )
findIncludeDirsAndLibrariesForH3DProjects ( PROJECT_NAMES H3DUtil HAPI H3DAPI
                                            INCLUDE_DIRS_OUTPUT_VAR tmp_h3d_include_dirs
                                            LIBRARIES_OUTPUT_VAR required_libs
                                            REQUIRED_PROJECTS H3DUtil HAPI H3DAPI )
include_directories( ${tmp_h3d_include_dirs} )

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
getDefaultH3DOutputDirectoryName( default_bin_install default_lib_install )

# Install to these directories.
install( TARGETS ThreadExample
         LIBRARY DESTINATION ${default_lib_install}
         RUNTIME DESTINATION ${default_bin_install}
         COMPONENT H3DAPI_cpack_examples_runtime )

