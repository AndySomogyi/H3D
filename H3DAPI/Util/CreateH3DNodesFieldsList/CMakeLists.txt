if( WIN32 )
  cmake_minimum_required( VERSION 2.6.0 )
endif()

# The name of our project is "CreateH3DNodesFieldsList".  CMakeLists files in this project can
# refer to the root source directory of the project as ${CreateH3DNodesFieldsList_SOURCE_DIR} and
# to the root binary directory of the project as ${CreateH3DNodesFieldsList_BINARY_DIR}.
project( CreateH3DNodesFieldsList )

# Where to find Source files.
set( CreateH3DNodesFieldsList_SRCS "${CreateH3DNodesFieldsList_SOURCE_DIR}/CreateH3DNodesFieldsList.cpp" )

# Add optional libs to this variable.
set( optionalLibs )

# Include directories.
include_directories( . )

# Add required libs to this variable.
set( requiredLibs )

# Where to find modules, used when finding packages.
set( CMAKE_MODULE_PATH "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../build/modules/sharedModules" )

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} ) 
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  include_directories( ${H3DAPI_INCLUDE_DIR} ) 
  
  set( requiredLibs ${requiredLibs} H3DUtil HAPI H3DAPI )
else( H3D_USE_DEPENDENCIES_ONLY )
  #H3DAPI
  find_package( H3DAPI REQUIRED )

  if( H3DAPI_FOUND )
    include_directories( ${H3DAPI_INCLUDE_DIR} ) 
    set( requiredLibs ${requiredLibs} ${H3DAPI_LIBRARIES} )
  endif()

  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUTIL_FOUND )
    include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
    set( requiredLibs ${requiredLibs} ${H3DUTIL_LIBRARIES} )
  endif()

  #HAPI
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIR} ) 
    set( requiredLibs ${requiredLibs} ${HAPI_LIBRARIES} )
  endif()
endif()

set( H3D_MSVC_VERSION 6 )
set( TEMP_MSVC_VERSION 1299 )
while( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
  math( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
  math( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
endwhile()

set( DEFAULT_BIN_INSTALL "bin" )

if( WIN32 )
  set( DEFAULT_BIN_INSTALL "bin32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( DEFAULT_BIN_INSTALL "bin64" )
  endif()
endif()
set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL "${CreateH3DNodesFieldsList_SOURCE_DIR}/../nodesFieldsList" )

#UI
option( INCLUDE_UI
        "Add UI nodes to list"
        OFF )
if( INCLUDE_UI )
  if( TARGET UI )
    set( UI_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL}" )
    if( EXISTS "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../UI/doc" )
      set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                 "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../UI/doc/nodesFieldsList" )
    endif()
  else( TARGET UI )
    find_package( UI )

    if( UI_FOUND )
      get_filename_component( UI_SEARCH_PATH ${UI_LIBRARY} PATH )
      string( REGEX REPLACE "/([^/])*$" "/${DEFAULT_BIN_INSTALL}" UI_SEARCH_PATH "${UI_SEARCH_PATH}" )
      if( EXISTS "${UI_INCLUDE_DIR}/../../../UI/doc" )
        set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                  "${UI_INCLUDE_DIR}/../../../UI/doc/nodesFieldsList" )
      endif()
    endif()
  endif()
endif()

#H3DPhysics
option( INCLUDE_H3DPhysics
        "Add H3DPhysics nodes to list"
        OFF )
if( INCLUDE_H3DPhysics )
  if( TARGET H3DPhysics )
    set( H3DPhysics_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL}" )
    if( EXISTS "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../H3DPhysics/doc" )
      set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                 "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../H3DPhysics/doc/nodesFieldsList" )
    endif()
  else( TARGET H3DPhysics )
    find_package( H3DPhysics )

    if( H3DPhysics_FOUND )
      get_filename_component( H3DPhysics_SEARCH_PATH ${H3DPhysics_LIBRARY} PATH )
      string( REGEX REPLACE "/([^/])*$" "/${DEFAULT_BIN_INSTALL}" H3DPhysics_SEARCH_PATH "${H3DPhysics_SEARCH_PATH}" )
      if( EXISTS "${H3DPhysics_INCLUDE_DIR}/../../../H3DPhysics/doc" )
        set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                  "${H3DPhysics_INCLUDE_DIR}/../../../H3DPhysics/doc/nodesFieldsList" )
      endif()
    endif()
  endif()
endif()

#MedX3D
option( INCLUDE_MedX3D
        "Add MedX3D nodes to list"
        OFF )
if( INCLUDE_MedX3D )
  if( TARGET MedX3D )
    set( MedX3D_SEARCH_PATH "${CMAKE_INSTALL_PREFIX}/${DEFAULT_BIN_INSTALL}" )
    if( EXISTS "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../MedX3D/doc" )
      set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                 "${CreateH3DNodesFieldsList_SOURCE_DIR}/../../../MedX3D/doc/nodesFieldsList" )
    endif()
  else( TARGET MedX3D )
    find_package( MedX3D )

    if( MEDX3D_FOUND )
      get_filename_component( MedX3D_SEARCH_PATH ${MEDX3D_LIBRARY} PATH )
      string( REGEX REPLACE "/([^/])*$" "/${DEFAULT_BIN_INSTALL}" MedX3D_SEARCH_PATH "${MedX3D_SEARCH_PATH}" )
      if( EXISTS "${MEDX3D_INCLUDE_DIR}/../../../MedX3D/doc" )
        set( CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL}
                                                                  "${MEDX3D_INCLUDE_DIR}/../../../MedX3D/doc/nodesFieldsList" )
      endif()
    endif()
  endif()
endif()

# Add a cache variable NodeRoutesToDotFile_EXTRA_RELEASE_LIBRARIES used
# to give extra release libraries which have a database which
# dot-files should be generated for.
if( NOT DEFINED CreateH3DNodesFieldsList_EXTRA_BINARIES )
  set( CreateH3DNodesFieldsList_EXTRA_BINARIES "" CACHE STRING "Add paths to extra release binaries that contain additional H3D nodes for which to generate dot-files. Each binary path must be enclosed in \" and separated by semi colon( ; )" )
  mark_as_advanced( CreateH3DNodesFieldsList_EXTRA_BINARIES )
endif()

set( CreateH3DNodesFieldsList_EXTRA_BINARIES_CODE )
foreach( CreateH3DNodesFieldsList_EXTRA_BINARY ${CreateH3DNodesFieldsList_EXTRA_BINARIES} )
  file( TO_CMAKE_PATH ${CreateH3DNodesFieldsList_EXTRA_BINARY} CreateH3DNodesFieldsList_EXTRA_BINARY_CMAKE_PATH )
  set( CreateH3DNodesFieldsList_EXTRA_BINARIES_CODE "${CreateH3DNodesFieldsList_EXTRA_BINARIES_CODE}\n  extra_libraries.push_back( \"${CreateH3DNodesFieldsList_EXTRA_BINARY_CMAKE_PATH}\" );" )
endforeach()

if( NOT DEFINED CreateH3DNodesFieldsList_EXTRA_BINARIES_nodesFieldsList_OUTPUT_DIR )
  set( CreateH3DNodesFieldsList_EXTRA_BINARIES_nodesFieldsList_OUTPUT_DIR "${CreateH3DNodesFieldsList_SOURCE_DIR}/../" CACHE PATH "Path to the directory in which to output list of nodes and fields for extra binaries." )
  mark_as_advanced( CreateH3DNodesFieldsList_EXTRA_BINARIES_nodesFieldsList_OUTPUT_DIR )
endif()

set( CreateH3DNodesFieldsList_OUTPUT_FILE_CODE )
foreach( OUTPUT_FILE ${CreateH3DNodesFieldsList_OUTPUT_FILE_DEFAULT_INTERNAL} )
  file( TO_CMAKE_PATH ${OUTPUT_FILE} OUTPUT_FILE_CMAKE_PATH )
  set( CreateH3DNodesFieldsList_OUTPUT_FILE_CODE "${CreateH3DNodesFieldsList_OUTPUT_FILE_CODE}\n  nodes_fields_output_file.push_back( \"${OUTPUT_FILE_CMAKE_PATH}\" );" )
endforeach()

if( NOT DEFINED CreateH3DNodesFieldsList_OUTPUT_ABREVIATION_FILE )
  set( CreateH3DNodesFieldsList_OUTPUT_ABREVIATION_FILE "" CACHE STRING "Path to file in which to output node names and comma separated def name abreviation suggestions." )
  mark_as_advanced( CreateH3DNodesFieldsList_OUTPUT_ABREVIATION_FILE )
endif()

if( CreateH3DNodesFieldsList_OUTPUT_ABREVIATION_FILE )
  set( OUTPUT_ABREVIATION_OR_NAME_CLASH ON )
else()
  set( OUTPUT_ABREVIATION_OR_NAME_CLASH OFF )
endif()

if( NOT DEFINED CreateH3DNodesFieldsList_OUTPUT_NAME_CLASH_FILE )
  set( CreateH3DNodesFieldsList_OUTPUT_NAME_CLASH_FILE "" CACHE STRING "Path to file in which to output node names clashes between libraries." )
  mark_as_advanced( CreateH3DNodesFieldsList_OUTPUT_NAME_CLASH_FILE )
endif()

if( NOT OUTPUT_ABREVIATION_OR_NAME_CLASH )
  if( CreateH3DNodesFieldsList_OUTPUT_NAME_CLASH_FILE )
    set( OUTPUT_ABREVIATION_OR_NAME_CLASH ON )
  else()
    set( OUTPUT_ABREVIATION_OR_NAME_CLASH OFF )
  endif()
endif()

configure_file( ${CreateH3DNodesFieldsList_SOURCE_DIR}/CreateH3DNodesFieldsList.cmake ${CreateH3DNodesFieldsList_SOURCE_DIR}/CreateH3DNodesFieldsList.cpp )

# Create build filess.
add_executable( CreateH3DNodesFieldsList  ${CreateH3DNodesFieldsList_SRCS} )
target_link_libraries( CreateH3DNodesFieldsList ${requiredLibs} ${optionalLibs} )

# Debug version should have _d postfix.
set_target_properties( CreateH3DNodesFieldsList PROPERTIES DEBUG_POSTFIX "_d" )

# set the install directory to the H3D directory on Windows
if( WIN32 ) 
  # Variable used to set compile properties.
  set( CreateH3DNodesFieldsList_COMPILE_FLAGS "" )

  if( UI_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( CreateH3DNodesFieldsList_COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS} -DHAVE_UI" )
    add_dependencies( CreateH3DNodesFieldsList UI )
  endif()

  if( H3DPhysics_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( CreateH3DNodesFieldsList_COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS} -DHAVE_H3DPhysics" )
    add_dependencies( CreateH3DNodesFieldsList H3DPhysics )
  endif()

  if( MEDX3D_FOUND OR H3D_USE_DEPENDENCIES_ONLY )
    set( CreateH3DNodesFieldsList_COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS} -DHAVE_MedX3D" )
    add_dependencies( CreateH3DNodesFieldsList MedX3D )
  endif()

  # Set compile and link properties for projects.
  if( MSVC80 )
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type
    set( CreateH3DNodesFieldsList_COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t-" )
  elseif( MSVC90 )
    # Remove some warnings about deprecated functions. Do not treat wchar_t as built in type. Compile on multiple processors/threads if possible.
    set( CreateH3DNodesFieldsList_COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE /Zc:wchar_t- /MP" )
  endif()

  set_target_properties( CreateH3DNodesFieldsList PROPERTIES COMPILE_FLAGS "${CreateH3DNodesFieldsList_COMPILE_FLAGS}" )

endif()