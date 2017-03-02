if(WIN32)
  cmake_minimum_required(VERSION 2.6.0)
endif(WIN32)

# The name of our project is "CheckH3DAgainstX3DSpec".  CMakeLists files in this project can
# refer to the root source directory of the project as ${CheckH3DAgainstX3DSpec_SOURCE_DIR} and
# to the root binary directory of the project as ${CheckH3DAgainstX3DSpec_BINARY_DIR}.
project(CheckH3DAgainstX3DSpec)

# Where to find Source files.
set( CheckH3DAgainstX3DSpec_SRCS "${CheckH3DAgainstX3DSpec_SOURCE_DIR}/CheckH3DAgainstX3DSpec.cpp" )

# Add optional libs to this variable.
set(optionalLibs)

# Include directories.
include_directories( . )

# Add required libs to this variable.
set(requiredLibs)

# Where to find modules, used when finding packages.
set(CMAKE_MODULE_PATH "${CheckH3DAgainstX3DSpec_SOURCE_DIR}/../../build/modules/sharedModules")

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} ) 
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  include_directories( ${H3DAPI_INCLUDE_DIR} ) 
  
  set( requiredLibs ${requiredLibs} H3DUtil HAPI H3DAPI )
else( H3D_USE_DEPENDENCIES_ONLY )
  #H3DAPI
  find_package(H3DAPI REQUIRED)

  if(H3DAPI_FOUND)
    include_directories( ${H3DAPI_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${H3DAPI_LIBRARIES} )
  endif(H3DAPI_FOUND)

  #H3DUtil
  find_package(H3DUtil REQUIRED)

  if(H3DUTIL_FOUND)
    include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${H3DUTIL_LIBRARIES} )
  endif(H3DUTIL_FOUND)

  #HAPI
  find_package(HAPI REQUIRED)

  if(HAPI_FOUND)
    include_directories( ${HAPI_INCLUDE_DIR} ) 
    set(requiredLibs ${requiredLibs} ${HAPI_LIBRARIES} )
  endif(HAPI_FOUND)
endif( H3D_USE_DEPENDENCIES_ONLY )

set( H3D_MSVC_VERSION 6 )
set( TEMP_MSVC_VERSION 1299 )
while( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  math( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
  math( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
endwhile( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )

set( DEFAULT_BIN_INSTALL "bin" )

if( WIN32 )
  set( DEFAULT_BIN_INSTALL "bin32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( DEFAULT_BIN_INSTALL "bin64" )
  endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
endif( WIN32 )

#Geospatial
option( INCLUDE_Geospatial
        "Add Geospatial nodes to list"
        OFF )
if( INCLUDE_Geospatial )
  if( TARGET Geospatial )
    set( Geospatial_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/bin" )
  else( TARGET Geospatial )
    find_file(Geospatial_DLL "Geospatial" )

    if(Geospatial_DLL)
      get_filename_component( Geospatial_SEARCH_PATH ${Geospatial_DLL} PATH )
      string( REGEX REPLACE "/([^/])*$" "/bin" Geospatial_SEARCH_PATH "${Geospatial_SEARCH_PATH}" )
    endif(Geospatial_DLL)
  endif( TARGET Geospatial )
endif( INCLUDE_Geospatial )

#H3DPhysics
option( INCLUDE_H3DPhysics
        "Add H3DPhysics nodes to list"
        OFF )
if( INCLUDE_H3DPhysics )
  if( TARGET H3DPhysics )
    set( H3DPhysics_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL}" )
  else( TARGET H3DPhysics )
    find_package(H3DPhysics)

    if(H3DPhysics_FOUND)
      get_filename_component( H3DPhysics_SEARCH_PATH ${H3DPhysics_LIBRARY} PATH )
      string( REGEX REPLACE "/([^/])*$" "/${DEFAULT_BIN_INSTALL}" H3DPhysics_SEARCH_PATH "${H3DPhysics_SEARCH_PATH}" )
    endif(H3DPhysics_FOUND)
  endif( TARGET H3DPhysics )
endif( INCLUDE_H3DPhysics )

#MedX3D
option( INCLUDE_MedX3D
        "Add MedX3D nodes to list"
        OFF )
if( INCLUDE_MedX3D )
  if( TARGET MedX3D )
    set( MedX3D_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL}" )
  else( TARGET MedX3D )
    find_package(MedX3D)

    if(MEDX3D_FOUND)
      get_filename_component( MedX3D_SEARCH_PATH ${MEDX3D_LIBRARY} PATH )
      string( REGEX REPLACE "/([^/])*$" "/${DEFAULT_BIN_INSTALL}" MedX3D_SEARCH_PATH "${MedX3D_SEARCH_PATH}" )
    endif(MEDX3D_FOUND)
  endif( TARGET MedX3D )
endif( INCLUDE_MedX3D )

configure_file( ${CheckH3DAgainstX3DSpec_SOURCE_DIR}/CheckH3DAgainstX3DSpec.cmake ${CheckH3DAgainstX3DSpec_SOURCE_DIR}/CheckH3DAgainstX3DSpec.cpp )

# Create build files.
add_executable(CheckH3DAgainstX3DSpec  ${CheckH3DAgainstX3DSpec_SRCS})
target_link_libraries( CheckH3DAgainstX3DSpec ${requiredLibs} ${optionalLibs} )

# Debug version should have _d postfix.
set_target_properties( CheckH3DAgainstX3DSpec PROPERTIES DEBUG_POSTFIX "_d" )

# Variable used to set compile properties.
set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "" )

option( DONT_LIST_INITIALIZE_TO_INPUT_OUTPUT
        "If true and X3D specification lists a field access type as INITIALIZE_ONLY and H3D lists it as INPUT_OUTPUT then don't show this difference"
        OFF )
if( DONT_LIST_INITIALIZE_TO_INPUT_OUTPUT )
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -DDONT_LIST_INITIALIZE_TO_INPUT_OUTPUT" )
endif( DONT_LIST_INITIALIZE_TO_INPUT_OUTPUT )

# set the install directory to the H3D directory on Windows
if(WIN32) 
  if( Geospatial_DLL )
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -DHAVE_Geospatial" )
    add_dependencies( CheckH3DAgainstX3DSpec Geospatial )
  endif( Geospatial_DLL )

  if( H3DPhysics_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -DHAVE_H3DPhysics" )
    add_dependencies( CheckH3DAgainstX3DSpec H3DPhysics )
  endif( H3DPhysics_FOUND OR H3D_USE_DEPENDENCIES_ONLY )

  if( MEDX3D_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -DHAVE_MedX3D" )
    add_dependencies( CheckH3DAgainstX3DSpec MedX3D )
  endif( MEDX3D_FOUND OR H3D_USE_DEPENDENCIES_ONLY )

  # Set compile and link properties for projects.
  if(MSVC80)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t-" )
  elseif(MSVC90)
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type. Compile on multiple processors/threads if possible.
    set( CheckH3DAgainstX3DSpec_COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t- /MP" )
  endif(MSVC80)

  set_target_properties( CheckH3DAgainstX3DSpec PROPERTIES COMPILE_FLAGS "${CheckH3DAgainstX3DSpec_COMPILE_FLAGS}" )

endif(WIN32)
