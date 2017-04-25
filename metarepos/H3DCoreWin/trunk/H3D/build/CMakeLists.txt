cmake_minimum_required( VERSION 2.6.0 )
project( H3DCoreWin )
# set the install directory to the H3DCoreWin directory on Windows
if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${H3DCoreWin_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif()

set( H3D_USE_DEPENDENCIES_ONLY TRUE )

set( CMAKE_MODULE_PATH "${H3DCoreWin_SOURCE_DIR}/../H3DUtil/build/modules" )

#pthread is required for using the H3DUtil library
find_package( PTHREAD REQUIRED )

set( H3DUTIL_INCLUDE_DIR "${H3DCoreWin_SOURCE_DIR}/../H3DUtil/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( HAPI_INCLUDE_DIR "${H3DCoreWin_SOURCE_DIR}/../HAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( H3DAPI_INCLUDE_DIR "${H3DCoreWin_SOURCE_DIR}/../H3DAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( H3DAPI_INCLUDE_DIR "${H3DCoreWin_SOURCE_DIR}/../External/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )

if( WIN32 )
  # PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
  # for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
  # is done then any of the included CMakeLists.txt is doing something wrong.
  set( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIR}" "${H3DCoreWin_SOURCE_DIR}/../External/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
endif()

if( NOT WIN32 )
  if( EXISTS ${H3DCoreWin_SOURCE_DIR}/../H3DUtil/build )
    message( STATUS "Including H3DUtil" )
    add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  endif()
  
  if( EXISTS ${H3DCoreWin_SOURCE_DIR}/../HAPI/build )
    message( STATUS "Including HAPI" )
    add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  endif()
endif()

if( EXISTS ${H3DCoreWin_SOURCE_DIR}/../H3DAPI/build )
  message( STATUS "Including H3DAPI" )
  add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  include( ${H3DCoreWin_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake )
elseif( EXISTS ${H3DCoreWin_SOURCE_DIR}/../HAPI/build )
  message( STATUS "Including HAPI" )
  add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
elseif( EXISTS ${H3DCoreWin_SOURCE_DIR}/../H3DUtil/build )
  message( STATUS "Including H3DUtil" )
  add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
endif()
if( NOT DEFINED H3D_EXTRA_TOOLKITS )
  set( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DCoreWin_SOURCE_DIR}/../( name of toolkit )/build" FORCE )
endif()

foreach( EXTRA_TOOLKIT ${H3D_EXTRA_TOOLKITS} )
  if( EXISTS ${H3DCoreWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
    message( STATUS "Including ${EXTRA_TOOLKIT}" )
    add_subdirectory( ${H3DCoreWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${EXTRA_TOOLKIT} )
  endif()
endforeach()
