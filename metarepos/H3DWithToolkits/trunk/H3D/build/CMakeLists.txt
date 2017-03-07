cmake_minimum_required( VERSION 2.6.0 )
project( H3DWithToolkits )
# set the install directory to the H3DWithToolkits directory on Windows
if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${H3DWithToolkits_SOURCE_DIR}/.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif()

set( H3D_USE_DEPENDENCIES_ONLY TRUE )

set( CMAKE_MODULE_PATH "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build/modules" )

#pthread is required for using the H3DUtil library
find_package( PTHREAD REQUIRED )

set( H3DUTIL_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( HAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../HAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( H3DAPI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( UI_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../UI/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( MEDX3D_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../MedX3D/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
set( H3DPHYSICS_INCLUDE_DIR "${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/include" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )
# PTHREAD_INCLUDE_DIR is added here since it is normally found by FindH3DUtil.
# for this type of top cmakelists build we do not want to use FindH3DUtil, ever. If that
# is done then any of the included CMakeLists.txt is doing something wrong.
set( EXTERNAL_INCLUDE_DIR "${PTHREAD_INCLUDE_DIR}" CACHE INTERNAL "Set to internal so it can be changed by sub directories." )

if( NOT WIN32 )
  if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build )
    message( STATUS "Including H3DUtil" )
    add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
  endif()
  
  if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build )
    message( STATUS "Including HAPI" )
    add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                      ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
  endif()
endif()

if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build )
  message( STATUS "Including H3DAPI" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI )
  include( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake )
elseif( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build )
  message( STATUS "Including HAPI" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )
elseif( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build )
  message( STATUS "Including H3DUtil" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../H3DUtil/build
                    ${CMAKE_CURRENT_BINARY_DIR}/H3DUtil )
endif()

if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../UI/build )
  message( STATUS "Including UI" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../UI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/UI )
  include( ${H3DWithToolkits_SOURCE_DIR}/../UI/build/UICPack.cmake )
endif()


if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build )
  message( STATUS "Including MedX3D" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build
                    ${CMAKE_CURRENT_BINARY_DIR}/MedX3D )
  include( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake )
endif()

if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build )
  message( STATUS "Including H3DPhysics" )
  add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build
                     ${CMAKE_CURRENT_BINARY_DIR}/H3DPhysics )
  include( ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake )
endif()

if( NOT DEFINED H3D_EXTRA_TOOLKITS )
  set( H3D_EXTRA_TOOLKITS "" CACHE STRING "Add names of directories of extra toolkits that you want to include. Separate each name by ;. The extra toolkit CMakeLists.txt must be located in ${H3DWithToolkits_SOURCE_DIR}/../( name of toolkit )/build" FORCE )
endif()

foreach( EXTRA_TOOLKIT ${H3D_EXTRA_TOOLKITS} )
  if( EXISTS ${H3DWithToolkits_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build )
    message( STATUS "Including ${EXTRA_TOOLKIT}" )
    add_subdirectory( ${H3DWithToolkits_SOURCE_DIR}/../${EXTRA_TOOLKIT}/build
                       ${CMAKE_CURRENT_BINARY_DIR}/${EXTRA_TOOLKIT} )
  endif()
endforeach()
# If cpack should be configured.
if( GENERATE_CPACK_PROJECT AND UNIX AND NOT WIN32 )
  include( ${H3DWithToolkits_SOURCE_DIR}/../H3DAPI/build/H3DAPICPack.cmake )
  include( ${H3DWithToolkits_SOURCE_DIR}/../UI/build/UICPack.cmake )
  include( ${H3DWithToolkits_SOURCE_DIR}/../MedX3D/build/MedX3DCPack.cmake )
  include( ${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics/build/H3DPhysicsCPack.cmake )
  set( CPACK_SOURCE_IGNORE_FILES ${H3DAPI_CPACK_IGNORE_PATTERNS}
          ${HAPI_CPACK_IGNORE_PATTERNS}
          ${UI_CPACK_IGNORE_PATTERNS}
          ${MedX3D_CPACK_IGNORE_PATTERNS}
          ${H3DPhysics_CPACK_IGNORE_PATTERNS} )
  set( CPACK_SOURCE_INSTALLED_DIRECTORIES "${H3DWithToolkits_SOURCE_DIR};build"
            "${H3DWithToolkits_SOURCE_DIR}/../H3DAPI;H3DAPI"
            "${H3DWithToolkits_SOURCE_DIR}/../HAPI;HAPI"
            "${H3DWithToolkits_SOURCE_DIR}/../H3DUtil;H3DUtil"
            "${H3DWithToolkits_SOURCE_DIR}/../UI;UI"
            "${H3DWithToolkits_SOURCE_DIR}/../MedX3D;MedX3D"
            "${H3DWithToolkits_SOURCE_DIR}/../H3DPhysics;H3DPhysics" )
  set( CPACK_PACKAGE_VERSION_MAJOR 2 )
  set( CPACK_PACKAGE_VERSION_MINOR 3 )
  set( CPACK_PACKAGE_VERSION_PATCH 1 )
  
  # Set information properties about the project to install.
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DAPI. A cross platform, haptics device independent, X3D based API for 3D graphics and haptics." )
  set( CPACK_PACKAGE_VENDOR "SenseGraphics AB" )
  set( CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
  set( CPACK_PACKAGE_DESCRIPTION_FILE "${H3DAPI_SOURCE_DIR}/../ReadMe" )
  set( CPACK_RESOURCE_FILE_LICENSE "${H3DAPI_SOURCE_DIR}/../LICENSE" )
  
  add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DummyFile
                      COMMAND echo )
  add_custom_target( INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/DummyFile )
  if( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD )
    add_custom_command( TARGET INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                  POST_BUILD
                  ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD} )
    add_dependencies( INSTALL_RUNTIME_AND_LIBRARIES_ONLY UI ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
  endif()
  include( CPack )
endif()
