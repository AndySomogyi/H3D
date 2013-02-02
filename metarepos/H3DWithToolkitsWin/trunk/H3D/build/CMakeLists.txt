cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkitsWin )

# set the install directory to the H3DWithToolkitsWin directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkitsWin_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

SET(CMAKE_MODULE_PATH "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/modules" )

SET (INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES})

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

#set include directories
SET( H3DUTIL_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/include" )
SET( HAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/include" )
SET( H3DAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/include" )
SET( UI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../UI/include" )
SET( MedX3D_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/include" )
SET( H3DPhysics_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/include" )
SET( EXTERNAL_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../External/include" )

# Version of H3DWithToolkitsWin
SET( H3DWithToolkitsWin_MAJOR_VERSION 2 )
SET( H3DWithToolkitsWin_MINOR_VERSION 2 )
SET( H3DWithToolkitsWin_BUILD_VERSION 0 )
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

MESSAGE( STATUS "Including H3DPhysics" )
ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build
                   ${CMAKE_CURRENT_BINARY_DIR}/H3DPhysics )
INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake)

IF( GENERATE_NODEROUTESTODOTFILE_BUILD )
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile )
    # This project is added here since H3DPhysics/UI should be defined before this is done.
    MESSAGE( STATUS "Including NodeRoutesToDotFile" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile
                      ${CMAKE_CURRENT_BINARY_DIR}/NodeRoutesToDotFile )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile )
ENDIF( GENERATE_NODEROUTESTODOTFILE_BUILD )

IF( NOT DEFINED CreateH3DNodesFieldsList_PROJECT )
  SET( CreateH3DNodesFieldsList_PROJECT "NO" CACHE BOOL "If set to YES the CreateH3DNodesFieldsList project will be included in this build." )
ENDIF( NOT DEFINED CreateH3DNodesFieldsList_PROJECT )
IF( CreateH3DNodesFieldsList_PROJECT )
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList )
    # This project is added here since H3DPhysics/UI/MedX3D should be defined before this is done.
    MESSAGE( STATUS "Including CreateH3DNodesFieldsList" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList
                      ${CMAKE_CURRENT_BINARY_DIR}/CreateH3DNodesFieldsList )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList )
ENDIF( CreateH3DNodesFieldsList_PROJECT )

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
	SET( CPACK_PACKAGE_NAME "H3DApi-Full" )
  
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
  
  ADD_CUSTOM_COMMAND( TARGET INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                      POST_BUILD
                      ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD} )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES H3DAPI ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES UI ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES MedX3D ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  SET( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES H3DPhysics ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  ADD_DEPENDENCIES( INSTALL_RUNTIME_AND_LIBRARIES_ONLY ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  
  IF( MSVC )
    # Additional, temporary command, to get 64 bit library for windows.
    SET( H3D_MSVC_VERSION 6 )
    SET( TEMP_MSVC_VERSION 1299 )
    WHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
      MATH( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
      MATH( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
    ENDWHILE( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
    
    SET( 32_OR_64 "64" )
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      SET( 32_OR_64 "32" )
    ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( NAMES_TO_INSTALL "H3DUtil" "HAPI" "H3DAPI" "H3DPhysics" "MedX3D" "UI" )
    foreach( NAME_TO_INSTALL ${NAMES_TO_INSTALL} )
      SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../lib${32_OR_64}/${NAME_TO_INSTALL}_vc${H3D_MSVC_VERSION}.lib )
      IF( EXISTS ${FILE_NAME_TO_INSTALL} )
        INSTALL( FILES ${FILE_NAME_TO_INSTALL}
                 DESTINATION lib${32_OR_64}
                 COMPONENT ${NAME_TO_INSTALL}_cpack_libraries )
      ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
      
      SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../bin${32_OR_64}/${NAME_TO_INSTALL}_vc${H3D_MSVC_VERSION}.dll )
      IF( EXISTS ${FILE_NAME_TO_INSTALL} )
        INSTALL( FILES ${FILE_NAME_TO_INSTALL}
                 DESTINATION bin${32_OR_64}
                 COMPONENT ${NAME_TO_INSTALL}_cpack_runtime )
      ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )

      SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../${NAME_TO_INSTALL}/include/H3D/${NAME_TO_INSTALL}_${32_OR_64}.h )
      IF( EXISTS ${FILE_NAME_TO_INSTALL} )
        INSTALL( FILES ${FILE_NAME_TO_INSTALL}
                 DESTINATION ${NAME_TO_INSTALL}/include/H3D
                 COMPONENT ${NAME_TO_INSTALL}_cpack_headers )
      ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
      
      SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../${NAME_TO_INSTALL}/include/${NAME_TO_INSTALL}/${NAME_TO_INSTALL}_${32_OR_64}.h )
      IF( EXISTS ${FILE_NAME_TO_INSTALL} )
        INSTALL( FILES ${FILE_NAME_TO_INSTALL}
                 DESTINATION ${NAME_TO_INSTALL}/include/${NAME_TO_INSTALL}
                 COMPONENT ${NAME_TO_INSTALL}_cpack_headers )
      ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
      
      SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../${NAME_TO_INSTALL}/include/H3D/${NAME_TO_INSTALL}/${NAME_TO_INSTALL}_${32_OR_64}.h )
      IF( EXISTS ${FILE_NAME_TO_INSTALL} )
        INSTALL( FILES ${FILE_NAME_TO_INSTALL}
                 DESTINATION ${NAME_TO_INSTALL}/include/H3D/${NAME_TO_INSTALL}
                 COMPONENT ${NAME_TO_INSTALL}_cpack_headers )
      ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
    endforeach( NAME_TO_INSTALL ${NAMES_TO_INSTALL} )
    
    SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../bin${32_OR_64}/H3DLoad.exe )
    IF( EXISTS ${FILE_NAME_TO_INSTALL} )
      INSTALL( FILES ${FILE_NAME_TO_INSTALL}
               DESTINATION bin${32_OR_64}
               COMPONENT H3DAPI_cpack_runtime )
    ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
    
    SET( FILE_NAME_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../bin${32_OR_64}/H3DViewer.exe )
    IF( EXISTS ${FILE_NAME_TO_INSTALL} )
      INSTALL( FILES ${FILE_NAME_TO_INSTALL}
               DESTINATION bin${32_OR_64}
               COMPONENT H3DAPI_cpack_runtime )
    ENDIF( EXISTS ${FILE_NAME_TO_INSTALL} )
    
    
  ENDIF( MSVC )
	INCLUDE(CPack)
ENDIF( GENERATE_CPACK_PROJECT )