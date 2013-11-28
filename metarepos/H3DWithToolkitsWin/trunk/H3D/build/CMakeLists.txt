cmake_minimum_required(VERSION 2.6.0)
PROJECT( H3DWithToolkitsWin )

# set the install directory to the H3DWithToolkitsWin directory on Windows
IF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  SET( CMAKE_INSTALL_PREFIX ${H3DWithToolkitsWin_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  SET( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
ENDIF( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )

SET( H3D_USE_DEPENDENCIES_ONLY TRUE )

SET(CMAKE_MODULE_PATH "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build/modules" )

SET (INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES})

#pthread is required for using the H3DUtil library
FIND_PACKAGE(PTHREAD REQUIRED)

#set include directories
SET( H3DUTIL_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( HAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( H3DAPI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( UI_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../UI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( MEDX3D_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
SET( H3DPHYSICS_INCLUDE_DIR "${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
# PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
# for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
# is done then any of the included CMakeLists.txt is doing something wrong.
SET( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIR}" "${H3DWithToolkitsWin_SOURCE_DIR}/../External/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )

# Version of H3DWithToolkitsWin
SET( H3DWithToolkitsWin_MAJOR_VERSION 2 )
SET( H3DWithToolkitsWin_MINOR_VERSION 3 )
SET( H3DWithToolkitsWin_BUILD_VERSION 1 )
IF( NOT WIN32 )
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build )
    MESSAGE( STATUS "Including H3DUtil" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build )
  
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build )
    MESSAGE( STATUS "Including HAPI" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build )
ENDIF( NOT WIN32 )

IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build )
  MESSAGE( STATUS "Including H3DAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake)
ELSEIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build )
  MESSAGE( STATUS "Including HAPI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
ELSEIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build )
  MESSAGE( STATUS "Including H3DUtil" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build )

IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build )
  MESSAGE( STATUS "Including UI" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/UI )
  INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build/UICPack.cmake)
ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../UI/build )


IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build )
  MESSAGE( STATUS "Including MedX3D" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build
                    ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )
  INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake)
ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../MedX3D/build )

IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build )
  MESSAGE( STATUS "Including H3DPhysics" )
  ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build
                     ${CMAKE_CURRENT_BINARY_DIR}/H3DPhysics )
  INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake)
ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DPhysics/build )

IF( GENERATE_NODEROUTESTODOTFILE_BUILD )
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile )
    # This project is added here since H3DPhysics/UI should be defined before this is done.
    MESSAGE( STATUS "Including NodeRoutesToDotFile" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile
                      ${CMAKE_CURRENT_BINARY_DIR}/NodeRoutesToDotFile )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/NodeRoutesToDotFile )
ENDIF( GENERATE_NODEROUTESTODOTFILE_BUILD )

IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList )
  IF( NOT DEFINED CreateH3DNodesFieldsList_PROJECT )
    SET( CreateH3DNodesFieldsList_PROJECT "NO" CACHE BOOL "If set to YES the CreateH3DNodesFieldsList project will be included in this build." )
  ENDIF( NOT DEFINED CreateH3DNodesFieldsList_PROJECT )
  IF( CreateH3DNodesFieldsList_PROJECT )
  
    # This project is added here since H3DPhysics/UI/MedX3D should be defined before this is done.
    MESSAGE( STATUS "Including CreateH3DNodesFieldsList" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList
                      ${CMAKE_CURRENT_BINARY_DIR}/CreateH3DNodesFieldsList )
  ENDIF( CreateH3DNodesFieldsList_PROJECT )
ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/CreateH3DNodesFieldsList )

IF( NOT DEFINED H3D_EXTRA_TOOLKITS )
  SET( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DWithToolkitsWin_SOURCE_DIR}/../(name of toolkit)/build" FORCE )
ENDIF( NOT DEFINED H3D_EXTRA_TOOLKITS )

FOREACH( EXTRA_TOOLKIT ${H3D_EXTRA_TOOLKITS} )
  IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
    MESSAGE( STATUS "Including ${EXTRA_TOOLKIT}" )
    ADD_SUBDIRECTORY( ${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${EXTRA_TOOLKIT} )
    IF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build/${EXTRA_TOOLKIT}CPack.cmake )
      INCLUDE (${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build/${EXTRA_TOOLKIT}CPack.cmake)
    ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build/${EXTRA_TOOLKIT}CPack.cmake )
  ENDIF( EXISTS ${H3DWithToolkitsWin_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
ENDFOREACH()

# If cpack should be configured.
IF( GENERATE_CPACK_PROJECT AND TARGET H3DAPI )
  INSTALL( FILES ${H3DWithToolkitsWin_SOURCE_DIR}/CMakeLists.txt
           DESTINATION build
           COMPONENT H3DUtil_cpack_sources )
  # Make sure that the nsis template for H3DAPI is used for packaging.
  SET(CMAKE_MODULE_PATH ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/build/modules )
  SET(CPACK_PACKAGE_VERSION_MAJOR ${H3DWithToolkitsWin_MAJOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_MINOR ${H3DWithToolkitsWin_MINOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_PATCH ${H3DWithToolkitsWin_BUILD_VERSION})
	SET( CPACK_PACKAGE_NAME "H3DApi-Full" )
	SET( CPACK_PACKAGE_FILE_NAME "H3DApi-Full-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
	SET( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
  
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

	SET( CPACK_NSIS_EXECUTABLES_DIRECTORY bin32 )
  IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( CPACK_NSIS_EXECUTABLES_DIRECTORY bin64 )
  ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  
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

      foreach( base_dir ${H3DWithToolkitsWin_SOURCE_DIR}/../${NAME_TO_INSTALL}/ ${CMAKE_CURRENT_BINARY_DIR}/${NAME_TO_INSTALL}/ ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI/${NAME_TO_INSTALL}/ ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI/HAPI/${NAME_TO_INSTALL}/ )
        SET( FILE_NAME_TO_INSTALL ${base_dir}include/H3D/${NAME_TO_INSTALL}_${32_OR_64}.h )
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
      endforeach( base_dir )
    endforeach( NAME_TO_INSTALL )
    
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
    
    SET( FILE_NAMES_TO_INSTALL ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/H3DViewerPackageExtraFiles/ACKNOWLEDGEMENTS
                              ${H3DWithToolkitsWin_SOURCE_DIR}/../H3DAPI/Util/H3DViewerPackageExtraFiles/NOTICE )
    foreach( NAME_TO_INSTALL ${FILE_NAMES_TO_INSTALL} )
      IF( EXISTS ${NAME_TO_INSTALL} )
        INSTALL( FILES ${NAME_TO_INSTALL}
                 DESTINATION .
                 COMPONENT H3DUtil_cpack_runtime )
      ENDIF( EXISTS ${NAME_TO_INSTALL} )
    endforeach( NAME_TO_INSTALL )
    
    
  ENDIF( MSVC )
	INCLUDE(CPack)
ENDIF( GENERATE_CPACK_PROJECT AND TARGET H3DAPI )