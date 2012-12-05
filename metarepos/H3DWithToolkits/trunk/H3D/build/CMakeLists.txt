cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkits )
# set the install directory to the H3DWithToolkits directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkits_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

SET(CMAKE_MODULE_PATH "${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build/modules" )

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

SET( H3DUTIL_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/include" )
SET( HAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../HAPI/include" )
SET( H3DAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/include" )
SET( UI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../UI/include" )
SET( MedX3D_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../MedX3D/include" )
SET( RigidBodyPhysics_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../RigidBodyPhysics/include" )

IF( NOT WIN32 )
	MESSAGE( STATUS "Including H3DUtil" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
	GET_DIRECTORY_PROPERTY( H3DUTIL_INCLUDE_DIR DIRECTORY ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build INCLUDE_DIRECTORIES )
  MESSAGE( STATUS "Including HAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
	GET_DIRECTORY_PROPERTY( HAPI_INCLUDE_DIR DIRECTORY ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build INCLUDE_DIRECTORIES )
ENDIF( NOT WIN32 )

MESSAGE( STATUS "Including H3DAPI" )
ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )

MESSAGE( STATUS "Including UI" )
ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../UI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/UI )

MESSAGE( STATUS "Including MedX3D" )
ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build
                  ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )

MESSAGE( STATUS "Including H3DPhysics" )
ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build
                   ${CMAKE_CURRENT_BINARY_DIR}/H3DPhysics )
