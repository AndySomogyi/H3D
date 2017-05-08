cmake_minimum_required( VERSION 2.8.7 )
cmake_policy( VERSION 2.8.7 )

project( H3DCore )

if( "$ENV{H3D_ROOT}" STREQUAL "" )
  set( ENV{H3D_ROOT} ${H3DCore_SOURCE_DIR}/../H3DAPI )
endif()

set( CMAKE_MODULE_PATH "${H3DCore_SOURCE_DIR}/../H3DUtil/build/modules" )

include( H3DCommonFunctions )
handleCommonCacheVar( CMAKE_INSTALL_PREFIX ${H3DCore_SOURCE_DIR}/.. )

if( NOT WIN32 )
  if( EXISTS ${H3DCore_SOURCE_DIR}/../H3DUtil/build )
    message( STATUS "Including H3DUtil" )
    add_subdirectory( ${H3DCore_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  endif()
  
  if( EXISTS ${H3DCore_SOURCE_DIR}/../HAPI/build )
    message( STATUS "Including HAPI" )
    add_subdirectory( ${H3DCore_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  endif()
endif()

if( EXISTS ${H3DCore_SOURCE_DIR}/../H3DAPI/build )
  message( STATUS "Including H3DAPI" )
  add_subdirectory( ${H3DCore_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  include( ${H3DCore_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake )
elseif( EXISTS ${H3DCore_SOURCE_DIR}/../HAPI/build )
  message( STATUS "Including HAPI" )
  add_subdirectory( ${H3DCore_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
elseif( EXISTS ${H3DCore_SOURCE_DIR}/../H3DUtil/build )
  message( STATUS "Including H3DUtil" )
  add_subdirectory( ${H3DCore_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
endif()

set( H3D_USE_DEPENDENCIES_ONLY TRUE )

#pthread is required for using the H3DUtil library
find_package( PTHREAD REQUIRED )

if( WIN32 )
  # PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
  # for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
  # is done then any of the included CMakeLists.txt is doing something wrong.
  set( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIRS}" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
endif()

if( NOT DEFINED H3D_EXTRA_TOOLKITS )
  set( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DCore_SOURCE_DIR}/../( name of toolkit )/build" FORCE )
endif()

foreach( extra_toolkit ${H3D_EXTRA_TOOLKITS} )
  if( EXISTS ${H3DCore_SOURCE_DIR}/../${extra_toolkit}/build )
    message( STATUS "Including ${extra_toolkit}" )
    add_subdirectory( ${H3DCore_SOURCE_DIR}/../${extra_toolkit}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${extra_toolkit} )
  endif()
endforeach()
