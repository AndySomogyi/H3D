cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DCore )
# set the install directory to the H3DCore directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET( CMAKE_INSTALL_PREFIX ${H3DCore_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

SET(CMAKE_MODULE_PATH "${H3DCore_SOURCE_DIR}/../H3DAPI/build/modules" )

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

SET( H3DUTIL_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../H3DUtil/include" )
SET( HAPI_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../HAPI/include" )
SET( H3DAPI_INCLUDE_DIR "${H3DCore_SOURCE_DIR}/../H3DAPI/include" )

IF( NOT WIN32 )
  ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ENDIF( NOT WIN32 )

ADD_SUBDIRECTORY( ${H3DCore_SOURCE_DIR}/../H3DAPI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )