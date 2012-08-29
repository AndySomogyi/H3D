cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkits )
IF( WIN32 )
  # set the install directory to the H3D directory on Windows
  IF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkits_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  ENDIF( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
ENDIF( WIN32 )

SET( H3D_USE_DEPENDENCIES_ONLY "TRUE" )

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
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ENDIF( NOT WIN32 )

ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )

ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../UI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/UI )

ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build
                  ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )

ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../RigidBodyPhysics/build
                  ${CMAKE_CURRENT_BINARY_DIR}/RigidBodyPhysics )
