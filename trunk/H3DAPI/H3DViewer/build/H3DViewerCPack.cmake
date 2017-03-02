if(WIN32)
  cmake_minimum_required(VERSION 2.8.5)
endif(WIN32)
if( NOT TARGET H3DViewer )
  message( FATAL_ERROR "Include file H3DViewerCPack.cmake require the target H3DViewer to exist. Please add H3DAPI/H3DViewer/build/CMakeLists.txt as subdirectory first." )
endif( NOT TARGET H3DViewer )

# To allow other projects that use H3DVIEWER as a subproject to add extra include directories
# when packaging.
if( GENERATE_H3DVIEWER_CPACK_PROJECT )
  if( WIN32 )
    # Add a cache variable which indicates where the Externals directory used for packaging
    # H3DViewer is located. If not set then FIND modules will be used instead.
    if( NOT DEFINED H3DViewer_CPACK_EXTERNAL_ROOT )
      set( H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT "" )
      if( H3D_USE_DEPENDENCIES_ONLY )
        foreach( EXTERNAL_INCLUDE_DIR_TMP ${EXTERNAL_INCLUDE_DIR} )
          if( EXISTS ${EXTERNAL_INCLUDE_DIR_TMP}/../include/pthread )
            set( H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT "${EXTERNAL_INCLUDE_DIR_TMP}/.." )
          endif( EXISTS ${EXTERNAL_INCLUDE_DIR_TMP}/../include/pthread )
        endforeach( EXTERNAL_INCLUDE_DIR_TMP ${EXTERNAL_INCLUDE_DIR} )
      else( H3D_USE_DEPENDENCIES_ONLY )
        set( H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT "$ENV{H3D_EXTERNAL_ROOT}" )
      endif( H3D_USE_DEPENDENCIES_ONLY )
      set( H3DViewer_CPACK_EXTERNAL_ROOT "${H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT}" CACHE PATH "Set to the External directory used with H3DViewer, needed to pack properly. If not set FIND_modules will be used instead." )
      mark_as_advanced(H3DViewer_CPACK_EXTERNAL_ROOT)
    endif( NOT DEFINED H3DViewer_CPACK_EXTERNAL_ROOT )
  endif( WIN32 )
  
  # Set information properties about the project to install.
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DViewer. An X3D viewer with extensions for haptics." )
  set(CPACK_PACKAGE_VENDOR "SenseGraphics AB")
  set(CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
  set(CPACK_RESOURCE_FILE_LICENSE "${H3DViewer_SOURCE_DIR}/../../LICENSE")
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};H3DViewer;ALL;/")
  
  # Installation directory for package.
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "SenseGraphics" )
  
  set(CPACK_PACKAGE_VERSION_MAJOR ${H3DViewer_MAJOR_VERSION})
  set(CPACK_PACKAGE_VERSION_MINOR ${H3DViewer_MINOR_VERSION})
  set(CPACK_PACKAGE_VERSION_PATCH ${H3DViewer_BUILD_VERSION})
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
  set(CPACK_NSIS_PACKAGE_NAME "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
  set(CPACK_NSIS_UNINSTALL_NAME "H3DViewer-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
  
  if( APPLE )
    if( NOT DEFINED H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
      set( H3DVIEWER_CPACK_INCLUDE_LIBRARIES "NO" CACHE BOOL "Decides if all dependent shared libraries should be included in the bundle or not." )
      mark_as_advanced(H3DVIEWER_CPACK_INCLUDE_LIBRARIES)
    endif( NOT DEFINED H3DVIEWER_CPACK_INCLUDE_LIBRARIES )

    set( CPACK_BUNDLE_NAME "H3DViewer" ) #- provides the bundle name (displayed in the finder underneath the bundle icon). 
    set( CPACK_BUNDLE_ICON "${H3DViewer_SOURCE_DIR}/H3DViewer.icns" ) # - provides the bundle icon (displayed in the /Applications folder, on the dock, etc). 
    set( CPACK_BUNDLE_PLIST "${H3DViewer_SOURCE_DIR}/info.plist" ) # - path to a file that will become the bundle plist. 
    set( CPACK_BUNDLE_STARTUP_COMMAND "${H3DViewer_SOURCE_DIR}/start.sh" ) #- path to a file that will be executed when the user opens the bundle. Could be a shell-script or a binary.

    if( H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
      #Include all shared libraries in bundle
      include("${H3DViewer_SOURCE_DIR}/OSXCPackLibraries.txt" )

      install( FILES ${OSX_PLUGIN_LIBRARIES}
               DESTINATION Plugins )

      install( CODE "EXECUTE_PROCESS( COMMAND \"python\" ${H3DViewer_SOURCE_DIR}/osx_bundle.py   
                                      WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX})" )
    endif( H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
  endif( APPLE )
  
  if( WIN32 )
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    set(CPACK_MONOLITHIC_INSTALL "TRUE" )
    set( CPACK_PACKAGE_START_MENU_NAME "H3DViewer ${H3DViewer_MAJOR_VERSION}.${H3DViewer_MINOR_VERSION}" )
                           
    set( EXTERNAL_BIN_PATH "bin32" )
    set( CPACK_PACKAGE_NAME "H3DViewer" )
    # CPACK_NSIS_INSTALL_ROOT must be set properly because cmake does not set it correctly
    # for a 64 bit build.
    set( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES32" )
    set( CPACK_NSIS_DISPLAY_NAME_POSTFIX "(x86)" )
    set( CPACK_H3D_64_BIT "FALSE" )
    set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin32 )
    set( CPACK_H3DViewer_RegEntry "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}(x86)" )
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # check if the system is 64 bit
      set( EXTERNAL_BIN_PATH "bin64" )
      set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin64 )
      set( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64" )
      set( CPACK_H3D_64_BIT "TRUE" )
      set( CPACK_NSIS_DISPLAY_NAME_POSTFIX "" )
      set( CPACK_H3DViewer_RegEntry "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}(x64)" )
    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(CPACK_NSIS_DISPLAY_NAME "H3DViewer${CPACK_NSIS_DISPLAY_NAME_POSTFIX} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )

    set( FEATURES_TO_INSTALL "bin" "H3DViewer/${EXTERNAL_BIN_PATH}" )
    set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "\\n" )
    set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "\\n" )
    set( TMP_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
    set( CMAKE_MODULE_PATH "" )
    set( INCLUDES_TO_SEARCH "" )
    if( H3DUTIL_INCLUDE_DIR )
      set( INCLUDES_TO_SEARCH ${H3DUTIL_INCLUDE_DIR} )
    endif( H3DUTIL_INCLUDE_DIR )
    if( HAPI_INCLUDE_DIR )
      set( INCLUDES_TO_SEARCH ${INCLUDES_TO_SEARCH} ${HAPI_INCLUDE_DIR} )
    endif( HAPI_INCLUDE_DIR )
    if( H3DAPI_INCLUDE_DIR )
      set( INCLUDES_TO_SEARCH ${INCLUDES_TO_SEARCH} ${H3DAPI_INCLUDE_DIR} )
    endif( H3DAPI_INCLUDE_DIR )
    foreach( dir_name "H3DAPI" "HAPI" "H3DUtil" )
      foreach( include_dir ${INCLUDES_TO_SEARCH} )
        string( FIND ${include_dir} "/${dir_name}/build" build_pos )
        if( NOT build_pos EQUAL -1 )
          string( LENGTH "/${dir_name}/build" search_length )
          math( EXPR str_length "${build_pos} + ${search_length}" )
          string( SUBSTRING ${include_dir} 0 ${str_length} include_dir_part )
          set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${include_dir_part}/modules )
        endif( NOT build_pos EQUAL -1 )
      endforeach( include_dir )
    endforeach( dir_name )
    
    include( InstallH3DAPIAndExternals )
    set( CMAKE_MODULE_PATH ${TMP_CMAKE_MODULE_PATH} )

    # Modify path since in the NSIS template.
    set( CPACK_NSIS_MODIFY_PATH "ON" )
  
    if( EXISTS ${H3DViewer_CPACK_EXTERNAL_ROOT} )
      set( EXTERNAL_BINARIES ${EXTERNAL_BINARIES}
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxbase30u_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxbase30u_xml_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_adv_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_core_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_gl_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_qa_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_html_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_propgrid_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_richtext_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxbase30u_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxbase30u_xml_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_adv_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_core_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_gl_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_qa_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_html_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_propgrid_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/wxmsw30u_richtext_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${EXTERNAL_BIN_PATH}/ode_double.dll )
      install( FILES "${H3DViewer_CPACK_EXTERNAL_ROOT}/include/ACKNOWLEDGEMENTS"
               DESTINATION H3DViewer )
      install( DIRECTORY ${H3DViewer_CPACK_EXTERNAL_ROOT}/include/ExternalLicenses/
               DESTINATION H3DViewer/ExternalLicenses )
    endif( EXISTS ${H3DViewer_CPACK_EXTERNAL_ROOT} )

    foreach( binary ${EXTERNAL_BINARIES} )
      if( EXISTS ${binary} )
        install( FILES ${binary}
                 DESTINATION H3DViewer/${DEFAULT_BIN_INSTALL} )
      endif( EXISTS ${binary} )
    endforeach( binary )
    
    set( H3D_MSVC_VERSION 6 )
    set( TEMP_MSVC_VERSION 1299 )
    while( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )
      math( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
      math( EXPR TEMP_MSVC_VERSION "${TEMP_MSVC_VERSION} + 100" )
    endwhile( ${MSVC_VERSION} GREATER ${TEMP_MSVC_VERSION} )

    if( H3D_USE_DEPENDENCIES_ONLY )      
      # these part are added separately so that these plug in can be automatically added to H3DViewer
      install( FILES ${H3DViewer_BINARY_DIR}/../../H3DPhysics/Release/H3DPhysics_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_BINARY_DIR}/../../MedX3D/Release/MedX3D_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_BINARY_DIR}/../../UI/Release/UI_vc${H3D_MSVC_VERSION}.dll
                     CONFIGURATIONS Release
                     DESTINATION H3DViewer/plugins )
    else( H3D_USE_DEPENDENCIES_ONLY )
      # these part are added separately so that these plug in can be automatically added to H3DViewer
      install( FILES ${H3DViewer_SOURCE_DIR}/../../../${DEFAULT_BIN_INSTALL}/H3DPhysics_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_SOURCE_DIR}/../../../${DEFAULT_BIN_INSTALL}/MedX3D_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_SOURCE_DIR}/../../../${DEFAULT_BIN_INSTALL}/UI_vc${H3D_MSVC_VERSION}.dll
                     CONFIGURATIONS Release
                     DESTINATION H3DViewer/plugins )
    endif( H3D_USE_DEPENDENCIES_ONLY )
    
    if( EXISTS ${H3DViewer_SOURCE_DIR}/../../Util/H3DViewerPackageExtraFiles )
      install( FILES ${H3DViewer_SOURCE_DIR}/../../Util/H3DViewerPackageExtraFiles/ReadMe.txt
             DESTINATION H3DViewer )
    endif( EXISTS ${H3DViewer_SOURCE_DIR}/../../Util/H3DViewerPackageExtraFiles )
  endif( WIN32 )  
  
  include(CPack)

endif( GENERATE_H3DVIEWER_CPACK_PROJECT )