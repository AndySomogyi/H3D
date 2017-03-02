if(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
endif(WIN32)

# The name of our project is "UpdateResourceFile".  CMakeLists files in this project can
# refer to the root source directory of the project as ${UpdateResourceFile_SOURCE_DIR} and
# to the root binary directory of the project as ${UpdateResourceFile_BINARY_DIR}.
project( UpdateResourceFile )

# Where to find Source files.
set( UpdateResourceFile_SRCS "${UpdateResourceFile_SOURCE_DIR}/UpdateResourceFile.cpp" )

# Add optional libs to this variable.
set(optionalLibs)

# Include directories.
include_directories( . )

# Add required libs to this variable.
set(requiredLibs)

# Where to find modules, used when finding packages.
set(CMAKE_MODULE_PATH "${UpdateResourceFile_SOURCE_DIR}/../../build/modules/sharedModules")

# Set properties so that the project is built statically with MFC to avoid
# linking against, and hence loading msvcp/r71.dll.
if( WIN32 )
  add_definitions(-D_AFXDLL)
  set(CMAKE_MFC_FLAG 1)
endif( WIN32 )

# Create build filess.
add_executable(UpdateResourceFile ${UpdateResourceFile_SRCS})
target_link_libraries( UpdateResourceFile ${requiredLibs} ${optionalLibs} )

# Debug version should have _d postfix.
set_target_properties( UpdateResourceFile PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
if(WIN32) 
  # Variable used to set compile properties.
  set( UPDATERESOURCEFILE_COMPILE_FLAGS "" )
  # Set compile and link properties for projects.
  if(MSVC80)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type
    set( UPDATERESOURCEFILE_COMPILE_FLAGS "${UPDATERESOURCEFILE_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t-" )
  elseif(MSVC90)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type. Compile on multiple processors/threads if possible.
    set( UPDATERESOURCEFILE_COMPILE_FLAGS "${UPDATERESOURCEFILE_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t- /MP" )
  endif(MSVC80)

  set_target_properties( UpdateResourceFile PROPERTIES COMPILE_FLAGS "${UPDATERESOURCEFILE_COMPILE_FLAGS}" )

endif(WIN32)
