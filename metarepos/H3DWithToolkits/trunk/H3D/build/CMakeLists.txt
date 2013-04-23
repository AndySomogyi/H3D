cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkits )
# set the install directory to the H3DWithToolkits directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkits_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

SET(CMAKE_MODULE_PATH "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build/modules" )

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

SET( H3DUTIL_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( HAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../HAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( H3DAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( UI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../UI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( MedX3D_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../MedX3D/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( H3DPhysics_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
# PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
# for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
# is done then any of the included CMakeLists.txt is doing something wrong.
SET( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIR}" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )

IF( NOT WIN32 )
  IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build )
    MESSAGE( STATUS "Including H3DUtil" )
    ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build )
  
  IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build )
    MESSAGE( STATUS "Including HAPI" )
    ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build )
ENDIF( NOT WIN32 )

IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build )
  MESSAGE( STATUS "Including H3DAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake)
ELSEIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build )
  MESSAGE( STATUS "Including HAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ELSEIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build )
  MESSAGE( STATUS "Including H3DUtil" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build )

IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../UI/build )
  MESSAGE( STATUS "Including UI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../UI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/UI )
  INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../UI/build/UICPack.cmake)
ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../UI/build )


IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build )
  MESSAGE( STATUS "Including MedX3D" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build
                    ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )
  INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake)
ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build )

IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build )
  MESSAGE( STATUS "Including H3DPhysics" )
  ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build
                     ${CMAKE_CURRENT_BINARY_DIR}/H3DPhysics )
  INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake)
ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build )

IF( NOT DEFINED H3D_EXTRA_TOOLKITS )
  SET( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DWithToolkits_SOURCE_DIR}/../(name of toolkit)/build" FORCE )
ENDIF( NOT DEFINED H3D_EXTRA_TOOLKITS )

FOREACH( EXTRA_TOOLKIT ${H3D_EXTRA_TOOLKITS} )
  IF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
    MESSAGE( STATUS "Including ${EXTRA_TOOLKIT}" )
    ADD_SUBDIRECTORY( ${H3DWithToolkits_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${EXTRA_TOOLKIT} )
  ENDIF( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
ENDFOREACH()
# If cpack should be configured.
IF( GENERATE_CPACK_PROJECT AND UNIX AND NOT WIN32 )
	INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake)
	INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../UI/build/UICPack.cmake)
	INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake)
	INCLUDE (${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake)
	SET(CPACK_SOURCE_IGNORE_FILES ${H3DAPI_CPACK_IGNORE_PATTERNS}
					${HAPI_CPACK_IGNORE_PATTERNS}
					${UI_CPACK_IGNORE_PATTERNS}
					${MedX3D_CPACK_IGNORE_PATTERNS}
					${H3DPhysics_CPACK_IGNORE_PATTERNS} )
	SET(CPACK_SOURCE_INSTALLED_DIRECTORIES "${H3DWithToolkits_SOURCE_DIR};build"
						"${H3DWithToolkits_SOURCE_DIR}/../H3DAPI;H3DAPI"
						"${H3DWithToolkits_SOURCE_DIR}/../HAPI;HAPI"
						"${H3DWithToolkits_SOURCE_DIR}/../H3DUtil;H3DUtil"
						"${H3DWithToolkits_SOURCE_DIR}/../UI;UI"
						"${H3DWithToolkits_SOURCE_DIR}/../MedX3D;MedX3D"
						"${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics;H3DPhysics" )
  SET(CPACK_PACKAGE_VERSION_MAJOR 2)
  SET(CPACK_PACKAGE_VERSION_MINOR 2)
  SET(CPACK_PACKAGE_VERSION_PATCH 0)
  
  # Set information properties about the project to install.
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DAPI. A cross platform, haptics device independent, X3D based API for 3D graphics and haptics.")
  SET(CPACK_PACKAGE_VENDOR "SenseGraphics AB")
  SET(CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${H3DAPI_SOURCE_DIR}/../ReadMe")
  SET(CPACK_RESOURCE_FILE_LICENSE "${H3DAPI_SOURCE_DIR}/../LICENSE")
  
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DummyFile
                      COMMAND echo )
  ADD_CUSTOM_TARGET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/DummyFile )
	IF( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD )
	  ADD_CUSTOM_COMMAND( TARGET INSTALL_RUNTIME_AND_LIBRARIES_ONLY
		              POST_BUILD
		              ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD} )
	  ADD_DEPENDENCIES( INSTALL_RUNTIME_AND_LIBRARIES_ONLY UI ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
	ENDIF( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD )
	INCLUDE(CPack)
ENDIF( GENERATE_CPACK_PROJECT AND UNIX AND NOT WIN32 )
