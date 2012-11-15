cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkitsWin )

# set the install directory to the H3DWithToolkitsWin directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkitsWin_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
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

SET(CMAKE_MODULE_PATH "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/modules" )

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

SET( H3DUTIL_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/include" )
SET( HAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/include" )
SET( H3DAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/include" )
SET( UI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../UI/include" )
SET( MedX3D_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/include" )
SET( RigidBodyPhysics_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../RigidBodyPhysics/include" )
SET( EXTERNAL_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../External/include" )

IF( NOT WIN32 )
  MESSAGE( STATUS "Including H3DUtil" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build
				 ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  MESSAGE( STATUS "Including HAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ENDIF( NOT WIN32 )

MESSAGE( STATUS "Including H3DAPI" )
ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake)

MESSAGE( STATUS "Including UI" )
ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build
                  ${CMAKE_CURRENT_BINARY_DIR}/UI )
INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build/UICPack.cmake)


MESSAGE( STATUS "Including MedX3D" )
ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build
                  ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )
INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake)

MESSAGE( STATUS "Including RigidBodyPhysics" )
ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../RigidBodyPhysics/build
                   ${CMAKE_CURRENT_BINARY_DIR}/RigidBodyPhysics )
INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../RigidBodyPhysics/build/RigidBodyPhysicsCPack.cmake)

# If cpack should be configured.
IF( GENERATE_CPACK_PROJECT )
  INSTALL( FILES ${H3DWithToolkitsWin_SOURCE_DIR}/CMakeLists.txt
           DESTINATION build
           COMPONENT H3DUtil_cpack_sources )
  # Make sure that the nsis template for H3DAPI is used for packaging.
  SET(CMAKE_MODULE_PATH ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/modules )
  SET(CPACK_PACKAGE_VERSION_MAJOR 2)
  SET(CPACK_PACKAGE_VERSION_MINOR 2)
  SET(CPACK_PACKAGE_VERSION_PATCH 0)
	INCLUDE(CPack)
ENDIF( GENERATE_CPACK_PROJECT )