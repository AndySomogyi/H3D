if( WIN32 )
  cmake_minimum_required( VERSION 2.8.5 )
endif()
if( NOT TARGET H3DViewer )
  message( FATAL_ERROR "Include file H3DViewerCPack.cmake require the target H3DViewer to exist. Please add H3DAPI/H3DViewer/build/CMakeLists.txt as subdirectory first." )
endif()

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
          endif()
        endforeach()
      else( H3D_USE_DEPENDENCIES_ONLY )
        set( H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT "$ENV{H3D_EXTERNAL_ROOT}" )
      endif()
      set( H3DViewer_CPACK_EXTERNAL_ROOT "${H3DViewer_CPACK_EXTERNAL_ROOT_DEFAULT}" CACHE PATH "Set to the External directory used with H3DViewer, needed to pack properly. If not set FIND_modules will be used instead." )
      mark_as_advanced( H3DViewer_CPACK_EXTERNAL_ROOT )
    endif()
  endif()
  
  # Set information properties about the project to install.
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DViewer. An X3D viewer with extensions for haptics." )
  set( CPACK_PACKAGE_VENDOR "SenseGraphics AB" )
  set( CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
  set( CPACK_RESOURCE_FILE_LICENSE "${H3DViewer_SOURCE_DIR}/../../LICENSE" )
  set( CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};H3DViewer;ALL;/" )
  
  # Installation directory for package.
  set( CPACK_PACKAGE_INSTALL_DIRECTORY "SenseGraphics" )
  
  set( CPACK_PACKAGE_VERSION_MAJOR ${H3DViewer_MAJOR_VERSION} )
  set( CPACK_PACKAGE_VERSION_MINOR ${H3DViewer_MINOR_VERSION} )
  set( CPACK_PACKAGE_VERSION_PATCH ${H3DViewer_BUILD_VERSION} )
  set( CPACK_PACKAGE_INSTALL_REGISTRY_KEY "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
  set( CPACK_NSIS_PACKAGE_NAME "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
  set( CPACK_NSIS_UNINSTALL_NAME "H3DViewer-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
  
  if( APPLE )
    if( NOT DEFINED H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
      set( H3DVIEWER_CPACK_INCLUDE_LIBRARIES "NO" CACHE BOOL "Decides if all dependent shared libraries should be included in the bundle or not." )
      mark_as_advanced( H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
    endif()

    set( CPACK_BUNDLE_NAME "H3DViewer" ) #- provides the bundle name (displayed in the finder underneath the bundle icon). 
    set( CPACK_BUNDLE_ICON "${H3DViewer_SOURCE_DIR}/H3DViewer.icns" ) # - provides the bundle icon (displayed in the /Applications folder, on the dock, etc). 
    set( CPACK_BUNDLE_PLIST "${H3DViewer_SOURCE_DIR}/info.plist" ) # - path to a file that will become the bundle plist. 
    set( CPACK_BUNDLE_STARTUP_COMMAND "${H3DViewer_SOURCE_DIR}/start.sh" ) #- path to a file that will be executed when the user opens the bundle. Could be a shell-script or a binary.

    if( H3DVIEWER_CPACK_INCLUDE_LIBRARIES )
      #Include all shared libraries in bundle
      include( "${H3DViewer_SOURCE_DIR}/OSXCPackLibraries.txt" )

      install( FILES ${OSX_PLUGIN_LIBRARIES}
               DESTINATION Plugins )

      install( CODE "EXECUTE_PROCESS( COMMAND \"python\" ${H3DViewer_SOURCE_DIR}/osx_bundle.py   
                                      WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX} )" )
    endif()
  endif()
  
  if( WIN32 )
    set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    set( CPACK_MONOLITHIC_INSTALL "TRUE" )
    set( CPACK_PACKAGE_START_MENU_NAME "H3DViewer ${H3DViewer_MAJOR_VERSION}.${H3DViewer_MINOR_VERSION}" )
                           
    set( external_bin_path "bin32" )
    set( CPACK_PACKAGE_NAME "H3DViewer" )
    # CPACK_NSIS_INSTALL_ROOT must be set properly because cmake does not set it correctly
    # for a 64 bit build.
    set( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES32" )
    set( CPACK_NSIS_DISPLAY_NAME_POSTFIX "(x86)" )
    set( CPACK_H3D_64_BIT "FALSE" )
    set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin32 )
    set( CPACK_H3DViewer_RegEntry "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}(x86)" )
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # check if the system is 64 bit
      set( external_bin_path "bin64" )
      set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin64 )
      set( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64" )
      set( CPACK_H3D_64_BIT "TRUE" )
      set( CPACK_NSIS_DISPLAY_NAME_POSTFIX "" )
      set( CPACK_H3DViewer_RegEntry "H3DViewer ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}(x64)" )
    endif()
    set( CPACK_NSIS_DISPLAY_NAME "H3DViewer${CPACK_NSIS_DISPLAY_NAME_POSTFIX} ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )

    set( features_to_install "bin" "H3DViewer/${external_bin_path}" )
    set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "\\n" )
    set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "\\n" )
    set( temp_output_postfix ${CMAKE_MODULE_PATH} )
    set( CMAKE_MODULE_PATH "" )
    set( includes_to_search "" )
    if( H3DUTIL_INCLUDE_DIR )
      set( includes_to_search ${H3DUTIL_INCLUDE_DIR} )
    endif()
    if( HAPI_INCLUDE_DIR )
      set( includes_to_search ${includes_to_search} ${HAPI_INCLUDE_DIR} )
    endif()
    if( H3DAPI_INCLUDE_DIR )
      set( includes_to_search ${includes_to_search} ${H3DAPI_INCLUDE_DIR} )
    endif()
    foreach( dir_name "H3DAPI" "HAPI" "H3DUtil" )
      foreach( include_dir ${includes_to_search} )
        string( FIND ${include_dir} "/${dir_name}/build" build_pos )
        if( NOT build_pos EQUAL -1 )
          string( LENGTH "/${dir_name}/build" search_length )
          math( EXPR str_length "${build_pos} + ${search_length}" )
          string( SUBSTRING ${include_dir} 0 ${str_length} include_dir_part )
          set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${include_dir_part}/modules )
        endif()
      endforeach()
    endforeach()
    
    include( InstallH3DAPIAndExternals )
    set( CMAKE_MODULE_PATH ${temp_output_postfix} )

    # Modify path since in the NSIS template.
    set( CPACK_NSIS_MODIFY_PATH "ON" )
  
    if( EXISTS ${H3DViewer_CPACK_EXTERNAL_ROOT} )
      set( external_binaries ${external_binaries}
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxbase30u_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxbase30u_xml_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_adv_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_core_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_gl_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_qa_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_html_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_propgrid_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_richtext_vc_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxbase30u_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxbase30u_xml_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_adv_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_core_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_gl_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_qa_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_html_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_propgrid_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/wxmsw30u_richtext_vc_x64_custom.dll
                             ${H3DViewer_CPACK_EXTERNAL_ROOT}/${external_bin_path}/ode_double.dll )
      install( FILES "${H3DViewer_CPACK_EXTERNAL_ROOT}/include/ACKNOWLEDGEMENTS"
               DESTINATION H3DViewer )
      install( DIRECTORY ${H3DViewer_CPACK_EXTERNAL_ROOT}/include/ExternalLicenses/
               DESTINATION H3DViewer/ExternalLicenses )
    endif()

    foreach( binary ${external_binaries} )
      if( EXISTS ${binary} )
        install( FILES ${binary}
                 DESTINATION H3DViewer/${default_bin_install} )
      endif()
    endforeach()
    
    set( H3D_MSVC_VERSION 6 )
    set( temp_msvc_version 1299 )
    while( ${MSVC_VERSION} GREATER ${temp_msvc_version} )
      math( EXPR H3D_MSVC_VERSION "${H3D_MSVC_VERSION} + 1" )
      math( EXPR temp_msvc_version "${temp_msvc_version} + 100" )
    endwhile()

    if( H3D_USE_DEPENDENCIES_ONLY )      
      # these part are added separately so that these plug in can be automatically added to H3DViewer
      install( FILES ${H3DViewer_BINARY_DIR}/../../H3DPhysics/Release/H3DPhysics_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_BINARY_DIR}/../../MedX3D/Release/MedX3D_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_BINARY_DIR}/../../UI/Release/UI_vc${H3D_MSVC_VERSION}.dll
                     CONFIGURATIONS Release
                     DESTINATION H3DViewer/plugins )
    else( H3D_USE_DEPENDENCIES_ONLY )
      # these part are added separately so that these plug in can be automatically added to H3DViewer
      install( FILES ${H3DViewer_SOURCE_DIR}/../../../${default_bin_install}/H3DPhysics_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_SOURCE_DIR}/../../../${default_bin_install}/MedX3D_vc${H3D_MSVC_VERSION}.dll
                     ${H3DViewer_SOURCE_DIR}/../../../${default_bin_install}/UI_vc${H3D_MSVC_VERSION}.dll
                     CONFIGURATIONS Release
                     DESTINATION H3DViewer/plugins )
    endif()
    
    if( EXISTS ${H3DViewer_SOURCE_DIR}/../../Util/H3DViewerPackageExtraFiles )
      install( FILES ${H3DViewer_SOURCE_DIR}/../../Util/H3DViewerPackageExtraFiles/ReadMe.txt
             DESTINATION H3DViewer )
    endif()
  endif()  
  
  include( CPack )

endif()