cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DCore )
# set the install directory to the H3DCore directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  SET( CMAKE_INSTALL_PREFIX ${H3DCore_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

SET(CMAKE_MODULE_PATH "${H3DCore_SOURCE_DIR}/../H3DUtil/build/modules" )

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

SET( H3DUTIL_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../H3DUtil/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( HAPI_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../HAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( H3DAPI_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../H3DAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
# PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
# for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
# is done then any of the included CMakeLists.txt is doing something wrong.
SET( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIR}" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )

IF( NOT WIN32 )
  IF( EXISTS ${H3DCore_SOURCE_DIR}/../H3DUtil/build )
    MESSAGE( STATUS "Including H3DUtil" )
    ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  ENDIF( EXISTS ${H3DCore_SOURCE_DIR}/../H3DUtil/build )
  
  IF( EXISTS ${H3DCore_SOURCE_DIR}/../HAPI/build )
    MESSAGE( STATUS "Including HAPI" )
    ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  ENDIF( EXISTS ${H3DCore_SOURCE_DIR}/../HAPI/build )
ENDIF( NOT WIN32 )

IF( EXISTS ${H3DCore_SOURCE_DIR}/../H3DAPI/build )
  MESSAGE( STATUS "Including H3DAPI" )
  ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  INCLUDE (${H3DCore_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake)
ELSEIF( EXISTS ${H3DCore_SOURCE_DIR}/../HAPI/build )
  MESSAGE( STATUS "Including HAPI" )
  ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ELSEIF( EXISTS ${H3DCore_SOURCE_DIR}/../H3DUtil/build )
  MESSAGE( STATUS "Including H3DUtil" )
  ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
ENDIF( EXISTS ${H3DCore_SOURCE_DIR}/../H3DAPI/build )
IF( NOT DEFINED H3D_EXTRA_TOOLKITS )
  SET( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DCore_SOURCE_DIR}/../(name of toolkit)/build" FORCE )
ENDIF( NOT DEFINED H3D_EXTRA_TOOLKITS )

FOREACH( EXTRA_TOOLKIT ${H3D_EXTRA_TOOLKITS} )
  IF( EXISTS ${H3DCore_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
    MESSAGE( STATUS "Including ${EXTRA_TOOLKIT}" )
    ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${EXTRA_TOOLKIT} )
  ENDIF( EXISTS ${H3DCore_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
ENDFOREACH()
