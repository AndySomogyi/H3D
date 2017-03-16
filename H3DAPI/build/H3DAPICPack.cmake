if( NOT TARGET H3DAPI )
  message( FATAL_ERROR "Include file H3DAPICPack.cmake require the target H3DAPI to exist. Please add H3DAPI/build/CMakeLists.txt as subdirectory first." )
endif()

# Add all sources, they are added to a variable called H3DAPI_SRCS defined
# in the included file. All header files are added to a variable called
# H3DAPI_HEADERS.
include( ${H3DAPI_SOURCE_DIR}/H3DAPISourceFiles.txt )
list( APPEND H3DAPI_HEADERS ${H3DAPI_SOURCE_DIR}/../include/H3D/StdAfx.h )
list( APPEND H3DAPI_SRCS ${H3DAPI_SOURCE_DIR}/../src/StdAfx.cpp )

# If cpack should be configured.
if( GENERATE_CPACK_PROJECT )
  if( WIN32 )
    # Add a cache variable which indicates where the Externals directory used for packaging
    # HAPI is located. If not set then FIND modules will be used instead.
    if( NOT DEFINED H3DAPI_CPACK_EXTERNAL_ROOT )
      set( H3DAPI_CPACK_EXTERNAL_ROOT_DEFAULT "" )
      if( H3D_USE_DEPENDENCIES_ONLY )
        foreach( EXTERNAL_INCLUDE_DIR_TMP ${EXTERNAL_INCLUDE_DIR} )
          if( EXISTS ${EXTERNAL_INCLUDE_DIR_TMP}/../include/pthread )
            set( H3DAPI_CPACK_EXTERNAL_ROOT_DEFAULT "${EXTERNAL_INCLUDE_DIR_TMP}/.." )
          endif()
        endforeach()
      else( H3D_USE_DEPENDENCIES_ONLY )
        set( H3DAPI_CPACK_EXTERNAL_ROOT_DEFAULT "$ENV{H3D_EXTERNAL_ROOT}" )
      endif()
      set( H3DAPI_CPACK_EXTERNAL_ROOT "${H3DAPI_CPACK_EXTERNAL_ROOT_DEFAULT}" CACHE PATH "Set to the External directory used with H3DAPI, needed to pack properly. If not set FIND_modules will be used instead." )
      mark_as_advanced( H3DAPI_CPACK_EXTERNAL_ROOT )
    endif()

    if( TARGET OpenHapticsRenderer )
      set( OpenHaptics_FOUND TRUE )
    endif()
    if( TARGET Chai3DRenderer )
      set( Chai3D_FOUND TRUE )
    endif()
    include( ${H3DAPI_SOURCE_DIR}/../../HAPI/build/HAPICPack.cmake )
  endif()


  # Set information properties about the project to install.
  set( CPACK_ALL_INSTALL_TYPES Full Developer )
  set( CMAKE_MODULE_PATH ${H3DAPI_SOURCE_DIR}/modules ${H3DAPI_SOURCE_DIR}/modules/sharedModules )
  set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "H3DAPI. A cross platform, haptics device independent, X3D based API for 3D graphics and haptics." )
  set( CPACK_PACKAGE_VENDOR "SenseGraphics AB" )
  set( CPACK_PACKAGE_CONTACT "support@sensegraphics.com" )
  set( CPACK_PACKAGE_DESCRIPTION_FILE "${H3DAPI_SOURCE_DIR}/../ReadMe" )
  set( CPACK_RESOURCE_FILE_LICENSE "${H3DAPI_SOURCE_DIR}/../LICENSE" )

  # Project to install.
  set( CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};H3DAPI;ALL;/" )

  # Installation directory for package.
  set( CPACK_PACKAGE_INSTALL_DIRECTORY "H3D" )

  # Our project depends on these debian packages for Linux.
  set( DEBIAN_PACKAGE_DEPENDS "libcurl3, libfreetype6-dev, ftgl-dev, python2.4-dev, libaudiofile0, libvorbis-dev, libopenal-dev, zlib1g-dev, libxerces27-dev, libfreeimage-dev, hapi(>= 1.0.0), h3dutil(>=1.0.0)" )

  # File patterns to ignore, common for all operating systems.
  set( H3DAPI_CPACK_IGNORE_PATTERNS /\\\\.svn/
                  \\\\.obj$
                  \\\\.ncb$
                  \\\\.log$
                  \\\\.suo$
                  \\\\.dir/
                  \\\\.user$
                  \\\\.cv$
                  "/Debug(.)*/"
                  "/debug(.)*/"
                  /Release
                  /release
                  /linux
                  /build/win32/
                  "/((C\\\\+\\\\+|ThreadExample)|(build|examples))/vc(7|(8|9))"
                  /osx
                  /H3DAPI/Util/
                  /berk/berk\\\\.wrl$
                  /berk/berk_orig\\\\.x3d$
                  /fish/Kumanomi\\\\.wrl$
                  /fish/Kumanomi_orig\\\\.x3d$
                  /humvee/humvee\\\\.WRL$
                  /humvee/humvee_orig\\\\.x3d$
                  /manikin/manikin\\\\.wrl$
                  /manikin/manikin_orig\\\\.x3d$
                  /moondial/moondial_orig\\\\.x3d$
                  /moondial/themoondial\\\\.wrl$
                  /plane/bobcat2\\\\.x3d$
                  /plane/bobcat_nh\\\\.x3d$
                  /plane/bobcat_orig\\\\.x3d$ )

  set( CPACK_PACKAGE_VERSION_MAJOR ${H3DAPI_MAJOR_VERSION} )
  set( CPACK_PACKAGE_VERSION_MINOR ${H3DAPI_MINOR_VERSION} )
  set( CPACK_PACKAGE_VERSION_PATCH ${H3DAPI_BUILD_VERSION} )

  # TODO: fix different for windows and unix if needed.
  if( WIN32 AND NOT UNIX )
    set( CPACK_NSIS_INSTALL_ROOT "C:" )
    set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON" )
    set( CPACK_PACKAGE_START_MENU_NAME "H3DAPI ${H3DAPI_MAJOR_VERSION}.${H3DAPI_MINOR_VERSION}" )

    # External binary directory to add to path.
    set( CPACK_EXTERNAL_BIN "bin32" )
    set( CPACK_H3D_64_BIT "FALSE" )
    set( external_bin_replace_path "bin64" )
    set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin32 )
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( CPACK_EXTERNAL_BIN "bin64" )
      set( external_bin_replace_path "bin32" )
      set( CPACK_H3D_64_BIT "TRUE" )
      set( CPACK_NSIS_EXECUTABLES_DIRECTORY bin64 )
    endif()
    set( external_bin_path "${CPACK_EXTERNAL_BIN}" )

    # external_includes and external_include_install_paths must be of equal lengths.
    # The reason for defining these variables here is in case we want to add functionality
    # to configure installation in some other way (using FIND-modules for example).
    set( external_includes "" )
    set( external_include_install_paths "" )
    # The external_include_files are installed directly in External/include
    set( external_include_files "" )
    set( external_libraries "" )
    set( external_static_libraries "" )
    set( external_binaries "" )

    if( EXISTS ${H3DAPI_CPACK_EXTERNAL_ROOT} )
      set( external_includes ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/xercesc/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/curl/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/Cg/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/AL/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/vorbis/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/ogg/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/libaudiofile/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/freetype/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/3dconnexion/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/FTGL/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/DirectShow/
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/js/ )
      set( external_include_install_paths External/include/xercesc
                                          External/include/curl
                                          External/include/Cg
                                          External/include/AL
                                          External/include/vorbis
                                          External/include/ogg
                                          External/include/libaudiofile
                                          External/include/freetype
                                          External/include/3dconnexion
                                          External/include/FTGL
                                          External/include/DirectShow
                                          External/include/js )

      set( external_include_files ${H3DAPI_CPACK_EXTERNAL_ROOT}/include/.h )

      set( external_libraries ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/glew32.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/xerces-c_3.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/xerces-c_3D.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/libcurl.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/cg.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/cgGL.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/OpenAL32.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/libvorbisfile.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/libvorbis.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/libogg.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/audiofile.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/siapp.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/spwmath.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/ftgl.lib
                              ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/js32.lib )

      set( external_static_libraries ${external_static_libraries}
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/glew32s.lib
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/libcurl.lib
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/libvorbisfile_static.lib
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/libvorbis_static.lib
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/libogg_static.lib
                                     ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/freetype2312.lib
                                     ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/freetype2312MT.lib
                                     #${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/ftgl_static.lib
                                     ${H3DAPI_CPACK_EXTERNAL_ROOT}/lib32/static/strmbase.lib )

      set( external_binaries ${external_binaries}
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/glew32.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/xerces-c_3_1.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/xerces-c_3_1D.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/libcurl.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/cg.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/cgGL.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/libvorbisfile.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/libvorbis.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/libogg.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/audiofile.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/ftgl.dll
                             ${H3DAPI_CPACK_EXTERNAL_ROOT}/${external_bin_path}/js32.dll )

    else( EXISTS ${H3DAPI_CPACK_EXTERNAL_ROOT} )
      message( WARNING "H3DAPI_CPACK_EXTERNAL_ROOT must be set to the External directory used by H3DAPI in order to package properly." )
    endif()

    if( external_includes )
      list( LENGTH external_includes external_includes_length )
      math( EXPR external_includes_length "${external_includes_length} - 1" )
      foreach( val RANGE ${external_includes_length} )
        list( GET external_includes ${val} val1 )
        list( GET external_include_install_paths ${val} val2 )
        install( DIRECTORY ${val1}
                 DESTINATION ${val2}
                 COMPONENT H3DAPI_cpack_external_source
                 REGEX "(/.svn)|(/CVS)" EXCLUDE )
      endforeach()
    endif()

    foreach( include_file ${external_include_files} )
      if( EXISTS ${include_file} )
        install( FILES ${include_file}
                 DESTINATION External/include
                 COMPONENT H3DAPI_cpack_external_source )
      endif()
    endforeach()

    foreach( library ${external_libraries} )
      if( EXISTS ${library} )
        install( FILES ${library}
                 DESTINATION External/lib32
                 COMPONENT H3DAPI_cpack_external_source )
      endif()
      # Add the other library path as well
      string( REGEX REPLACE "(/lib32/)" "/lib64/" other_library ${library} )
      if( EXISTS ${other_library} )
        install( FILES ${other_library}
                 DESTINATION External/lib64
                 COMPONENT H3DAPI_cpack_external_source )
      endif()
    endforeach()

    foreach( library ${external_static_libraries} )
      if( EXISTS ${library} )
        install( FILES ${library}
                 DESTINATION External/lib32/static
                 COMPONENT H3DAPI_cpack_external_source )
      endif()
      # Add the other library path as well
      string( REGEX REPLACE "(/lib32/)" "/lib64/" other_library ${library} )
      if( EXISTS ${other_library} )
        install( FILES ${other_library}
                 DESTINATION External/lib64/static
                 COMPONENT H3DAPI_cpack_external_source )
      endif()
    endforeach()

    foreach( binary ${external_binaries} )
      if( EXISTS ${binary} )
        install( FILES ${binary}
                 DESTINATION External/${external_bin_path}
                 COMPONENT H3DAPI_cpack_external_runtime )
      endif()

      string( REGEX REPLACE "(/${external_bin_path}/)" "/${external_bin_replace_path}/" other_binary ${binary} )
      if( EXISTS ${other_binary} )
        install( FILES ${other_binary}
                 DESTINATION External/${external_bin_replace_path}
                 COMPONENT H3DAPI_cpack_external_runtime )
      endif()
    endforeach()

    # setting names and dependencies between components and also grouping them.
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_RUNTIME_DISPLAY_NAME "External runtime" )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_RUNTIME_DESCRIPTION "External runtime binaries needed by H3DAPI." )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_RUNTIME_DEPENDS HAPI_cpack_external_runtime )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_RUNTIME_GROUP "H3DAPI_cpack_group" )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_RUNTIME_INSTALL_TYPES Developer Full )

    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_SOURCE_DISPLAY_NAME "External header/libraries" )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_SOURCE_DESCRIPTION "External headers and libraries needed by H3DAPI." )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_SOURCE_DEPENDS HAPI_cpack_external_source H3DAPI_cpack_external_runtime )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_SOURCE_GROUP "H3DAPI_cpack_group" )
    set( CPACK_COMPONENT_H3DAPI_CPACK_EXTERNAL_SOURCE_INSTALL_TYPES Developer Full )

    set( H3DAPI_CPACK_INSTALLED_DIRECTORIES "" )

    if( H3DAPI_LOADER_PROJECTS )
      set( CPACK_ADD_H3DLOAD_DEMOS_LINKS "ON" )
    endif()

    # Create cached variable for getting the plugin folder.
    set( H3DVIEWER_PLUGIN_FOLDER "" CACHE PATH "Path to folder containing plugins for H3DViewer." )
    mark_as_advanced( H3DVIEWER_PLUGIN_FOLDER )
    if( H3DVIEWER_PLUGIN_FOLDER )
      # Create cached variable for getting the VHTK examples folder.
      set( VHTK_EXAMPLES_FOLDER "" CACHE PATH "Path to folder containing VHTK examples." )
      mark_as_advanced( VHTK_EXAMPLES_FOLDER )
      if( VHTK_EXAMPLES_FOLDER )
        set( H3DAPI_CPACK_INSTALLED_DIRECTORIES ${H3DAPI_CPACK_INSTALLED_DIRECTORIES}
                                                "${VHTK_EXAMPLES_FOLDER};H3DAPI/examples/VHTK" )
        set( CPACK_ADD_VHTK_DEMOS_LINKS "ON" )
      else( VHTK_EXAMPLES_FOLDER )
        set( CPACK_ADD_VHTK_DEMOS_LINKS "OFF" )
      endif()
    endif()

    #Extra links to start menu if values are "ON"
    set( CPACK_ADD_H3DDOC_LINKS "ON" )
    set( CPACK_ADD_H3DSETTINGS_LINKS "ON" )

    # Extra install commands will be set to install python and OpenAL
    set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "\\n" )
    set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "\\n" )

    set( redist_versions 8 9 10 )
    foreach( redist_version ${redist_versions} )
      set( redist_architectures 32 )
      if( ${redist_version} GREATER 9 )
        set( redist_architectures ${redist_architectures} 64 )
      endif()
      foreach( redist_architecture ${redist_architectures} )
        set( redist_version_with_arch ${redist_version}_${redist_architecture} )
        # Add cache variable vc${redist_version}_redist which should be set to the install file
        # for microsoft visual studio redistributables, they can be found in the
        # installation folder for each visual studio installation.
        if( NOT DEFINED vc${redist_version_with_arch}_redist )
          set( vc${redist_version_with_arch}_redist CACHE FILEPATH "Set this to the exe installing microsoft visual studio redistributable for visual studio ${redist_version}" )
          mark_as_advanced( vc${redist_version_with_arch} )
        endif()
        if( vc${redist_version_with_arch}_redist )
          string( REPLACE "/" "\\\\" Temp_vc${redist_version_with_arch}_redist ${vc${redist_version_with_arch}_redist} )
          get_filename_component( VC${redist_version_with_arch}_FILE_NAME ${vc${redist_version_with_arch}_redist} NAME )
          if( redist_architecture EQUAL 64 )
            set( MS_REDIST_ARCH_COMMAND "A comment\\n \\\${If} \\\${RunningX64}\\n" )
            set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                   ${MS_REDIST_ARCH_COMMAND} )
            set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                     ${MS_REDIST_ARCH_COMMAND} )
          endif()
          set( MS_REDIST_INSTALL_COMMAND_1 " Set output Path\\n  SetOutPath \\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\"\\n"
                                           " Code to install Visual studio redistributable\\n  File \\\"${Temp_vc${redist_version_with_arch}_redist}\\\"\\n" )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 ${MS_REDIST_INSTALL_COMMAND_1} )
          set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   " Check if uninstall vc redist \\n  MessageBox MB_YESNO \\\"Do you want to uninstall Visual studio ${redist_version} ${redist_architecture} bit redistributable? It is recommended if no other applications use it.\\\" IDYES uninstall_vcredist_yes${redist_architecture} IDNO uninstall_vcredist_no${redist_architecture}\\n"
                                                   " A comment \\n  uninstall_vcredist_yes${redist_architecture}:\\n"
                                                   ${MS_REDIST_INSTALL_COMMAND_1} )
          if( ${redist_version} LESS 9 )
            set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                   " Execute silent and wait\\n  ExecWait '\\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\\${VC${redist_version_with_arch}_FILE_NAME}\\\" /q:a /norestart /c:\\\"msiexec /i vcredist.msi /qn\\\"'\\n" )
            set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   " Execute silent and wait\\n  ExecWait '\\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\\${VC${redist_version_with_arch}_FILE_NAME}\\\" /q:a /norestart /c:\\\"msiexec /x vcredist.msi /qn\\\"'\\n" )
          else( )
            set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                   " Execute silent and wait\\n  ExecWait '\\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\\${VC${redist_version_with_arch}_FILE_NAME}\\\" /q /norestart \\\"'\\n" )
            set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   " Execute silent and wait\\n  ExecWait '\\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\\${VC${redist_version_with_arch}_FILE_NAME}\\\" /q /uninstall \\\"'\\n" )
          endif()
          set( MS_REDIST_INSTALL_COMMAND_2 " Wait a bit for system to unlock file.\\n  Sleep 1000\\n"
                                           " Delete file\\n  Delete \\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\\${VC${redist_version_with_arch}_FILE_NAME}\\\"\\n"
                                           " Reset output Path\\n  SetOutPath \\\"$INSTDIR\\\"\\n"
                                           " Wait a bit for system to unlock directory.\\n  Sleep 1000\\n"
                                           " Remove folder\\n  RMDir /r \\\"$INSTDIR\\\\vc${redist_version_with_arch}\\\"\\n\\n" )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 ${MS_REDIST_INSTALL_COMMAND_2} )
          set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   ${MS_REDIST_INSTALL_COMMAND_2}
                                                   " A comment \\n  uninstall_vcredist_no${redist_architecture}:\\n\\n" )
          if( redist_architecture EQUAL 64 )
            set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                             "A comment\\n \\\${EndIf}\\n" )
            set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                     "A comment\\n \\\${EndIf}\\n" )
          endif()
        endif()
      endforeach()
    endforeach()

    # Install python if not already installed
    set( python_architectures 32 )
    if( ${MSVC_VERSION} GREATER 1599 )
      set( python_architectures ${python_architectures} 64 )
    endif()
    foreach( python_architecture ${python_architectures} )
      set( PYTHONMSI_CACHE_NAME PythonInstallMSI_${python_architecture} )
      set( ${PYTHONMSI_CACHE_NAME} "" CACHE FILEPATH "Needs to be set to add python installation to the package." )
      mark_as_advanced( ${PYTHONMSI_CACHE_NAME} )
      if( ${PYTHONMSI_CACHE_NAME} )
        set( PYTHON_TARGET_DIR "" )
        string( REGEX MATCH 2\\.[456789] CPACK_PYTHON_VERSION ${${PYTHONMSI_CACHE_NAME}} )
        string( REGEX REPLACE \\. "" CPACK_PYTHON_VERSION_NO_DOT ${CPACK_PYTHON_VERSION} )
        get_filename_component( PYTHON_FILE_NAME ${${PYTHONMSI_CACHE_NAME}} NAME )
        string( REPLACE "/" "\\\\" TEMP_PythonInstallMSI ${${PYTHONMSI_CACHE_NAME}} )
        set( PYTHON_ARCH_COMMAND "A comment\\n \\\${If} \\\${RunningX64}\\n" )
        set( PYTHON_EXECUTE " Execute python installer silent, wait for completion\\n  ExecWait '\\\"msiexec\\\" /i \\\"$INSTDIR\\\\${PYTHON_FILE_NAME}\\\" /qn ALLUSERS=1'\\n" )
        set( PYTHON_EXECUTE_TARGET_DIR " Get system dir \\n  StrCpy $1 $WINDIR 3 0\\n"
                                       " Execute python installer silent, wait for completion\\n  ExecWait '\\\"msiexec\\\" /i \\\"$INSTDIR\\\\${PYTHON_FILE_NAME}\\\" /qn ALLUSERS=1 TARGETDIR=$1Python${CPACK_PYTHON_VERSION_NO_DOT}_${python_architecture}'\\n" )
        set( PYTHON_INSTALL_COMMAND_1 " Code to install Python\\n  ReadRegStr $0 HKLM SOFTWARE\\\\Python\\\\PythonCore\\\\${CPACK_PYTHON_VERSION}\\\\InstallPath \\\"\\\"\\n" )
        set( PYTHON_INSTALL_COMMAND_2 " Extract python installer\\n  File \\\"${TEMP_PythonInstallMSI}\\\"\\n" )
        set( PYTHON_INSTALL_COMMAND_3 " Wait a bit for system to unlock file.\\n  Sleep 1000\\n"
                                      " Delete python installer\\n  Delete \\\"$INSTDIR\\\\${PYTHON_FILE_NAME}\\\"\\n\\n" )

        if( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   ${PYTHON_ARCH_COMMAND} )
        endif()
        set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                 " Set reg view\\n  SetRegView ${python_architecture} \\n"
                                                 ${PYTHON_INSTALL_COMMAND_1}
                                                 " Check if python is installed\\n  StrCmp $0 \\\"\\\" uninstall_python_no${python_architecture} 0\\n"
                                                 " Check if uninstall python \\n  MessageBox MB_YESNO \\\"Do you want to uninstall python ${python_architecture} bit? It is recommended if no other applications use python ${CPACK_PYTHON_VERSION}.\\\" IDYES uninstall_python_yes${python_architecture} IDNO uninstall_python_no${python_architecture}\\n"
                                                 " A comment \\n  uninstall_python_yes${python_architecture}:\\n"
                                                 ${PYTHON_INSTALL_COMMAND_2}
                                                 " Execute python installer, wait for completion\\n  ExecWait '\\\"msiexec\\\" /x \\\"$INSTDIR\\\\${PYTHON_FILE_NAME}\\\" /qn'\\n"
                                                 ${PYTHON_INSTALL_COMMAND_3}
                                                 " A comment \\n  uninstall_python_no${python_architecture}:\\n"
                                                 " Set reg view\\n  SetRegView 32 \\n" )
        set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                               " Set reg view\\n  SetRegView ${python_architecture} \\n"
                                                " Check if H3DAPI selected for installation\\n IntOp $0 $H3DAPI_cpack_sources_selected | $H3DAPI_cpack_runtime_selected\\n"
                                                " Check if H3DAPI selected for installation\\n \\\${If} $0 > 0\\n" )
        if( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 ${PYTHON_ARCH_COMMAND} )
        endif()
        set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                               ${PYTHON_INSTALL_COMMAND_1}
                                               " Check if python is installed\\n  StrCmp $0 \\\"\\\" 0 install_python_no${python_architecture}\\n"
                                               ${PYTHON_INSTALL_COMMAND_2}
                                               "A comment \\n  ClearErrors\\n" )
        if( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                               "Check if python install path is free \\n  GetFullPathName $0 C:\\\\Python${CPACK_PYTHON_VERSION_NO_DOT}\\n" )
        else( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 "A comment\\n \\\${If} \\\${RunningX64}\\n"
                                                 "Check if python install path is free \\n  GetFullPathName $0 C:\\\\Python${CPACK_PYTHON_VERSION_NO_DOT}\\n"
                                                 "A comment\\n \\\${Else}\\n"
                                                 "Check if python install path is free \\n  GetFullPathName $0 C:\\\\Python${CPACK_PYTHON_VERSION_NO_DOT}_${python_architecture}\\n"
                                                 "A comment \\n \\\${EndIf}\\n" )
        endif()
        set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                               "If errors then path was not found, i.e. empty\\n  IfErrors 0 python_install_not_hidden${python_architecture} \\n"
                                               "A comment \\n    ClearErrors\\n" )
        if( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 ${PYTHON_EXECUTE} )
        else( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 "A comment\\n \\\${If} \\\${RunningX64}\\n"
                                                 ${PYTHON_EXECUTE_TARGET_DIR}
                                                 "A comment\\n \\\${Else}\\n"
                                                 ${PYTHON_EXECUTE}
                                                 "A comment \\n \\\${EndIf}\\n" )
        endif()
        set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                               "A comment \\n Goto python_end_install${python_architecture}\\n"
                                               "A comment \\n python_install_not_hidden${python_architecture}:\\n"
                                               " Execute python installer, wait for completion\\n  ExecWait '\\\"msiexec\\\" /i \\\"$INSTDIR\\\\${PYTHON_FILE_NAME}\\\"'\\n"
                                               " A comment \\n  python_end_install${python_architecture}:\\n"
                                               ${PYTHON_INSTALL_COMMAND_3}
                                               "A comment \\n  install_python_no${python_architecture}:\\n"
                                               "A comment \\n \\\${EndIf}\\n" )
        if( python_architecture EQUAL 64 )
          set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                   "A comment\\n \\\${EndIf}\\n" )
          set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                                 "A comment\\n \\\${EndIf}\\n"
                                                 " Set reg view\\n  SetRegView 32 \\n" )
        endif()
      endif()
    endforeach()

    # Install OpenAL.
    set( OpenAlInstallExe "" CACHE FILEPATH "Needs to be set to add openal installation to the package." )
    mark_as_advanced( OpenAlInstallExe )
    if( OpenAlInstallExe )
      get_filename_component( OpenAL_FILE_NAME ${OpenAlInstallExe} NAME )
      string( REPLACE "/" "\\\\" TEMP_OpenAlInstallExe ${OpenAlInstallExe} )
      set( OPENAL_INSTALL_COMMAND_1 " Code to install OPENAL\\n  File \\\"${TEMP_OpenAlInstallExe}\\\"\\n" )
      set( OPENAL_INSTALL_COMMAND_2 " Wait a bit for system to unlock file.\\n  Sleep 1000\\n"
                                    " Delete install file\\n  Delete \\\"$INSTDIR\\\\${OpenAL_FILE_NAME}\\\"\\n" )
      set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                             ${OPENAL_INSTALL_COMMAND_1}
                                             " Execute install file\\n  ExecWait '\\\"$INSTDIR\\\\${OpenAL_FILE_NAME}\\\" /s'\\n"
                                             ${OPENAL_INSTALL_COMMAND_2} )
      if( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # check if the system is 64 bit
        set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                 "A comment\\n \\\${If} \\\${RunningX64}\\n"
                                                 "A comment\\n   SetRegView 32\\n"
                                                 "A comment\\n \\\${EndIf}\\n" )
      endif()
      set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                               " Get OpenAL uninstall registry string\\n  ReadRegStr $0 HKLM SOFTWARE\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Uninstall\\\\OpenAL \\\"UninstallString\\\"\\n"
                                               " Check if OpenAL is installed\\n  StrCmp $0 \\\"\\\" uninstall_openal_no 0\\n"
                                               " Check if uninstall OpenAL \\n  MessageBox MB_YESNO \\\"Do you want to uninstall OpenAL? It is recommended if no other applications use it.\\\" IDYES uninstall_openal_yes IDNO uninstall_openal_no\\n"
                                               " A comment \\n  uninstall_openal_yes:\\n"
                                               ${OPENAL_INSTALL_COMMAND_1}
                                               " Execute install file\\n  ExecWait '\\\"$INSTDIR\\\\${OpenAL_FILE_NAME}\\\" /u /s'\\n"
                                               ${OPENAL_INSTALL_COMMAND_2}
                                               " A comment \\n  uninstall_openal_no:\\n\\n" )
      if( CMAKE_SIZEOF_VOID_P EQUAL 8 ) # check if the system is 64 bit
        set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
                                                 "A comment\\n \\\${If} \\\${RunningX64}\\n"
                                                 "A comment\\n   SetRegView 64\\n"
                                                 "A comment\\n \\\${EndIf}\\n" )
      endif()
    endif()

    set( CMakeInstallExe "" CACHE FILEPATH "Needs to be set to add CMake installation to the package." )
    mark_as_advanced( CMakeInstallExe )
    if( CMakeInstallExe )
      set( CMAKE_VERSION_TO_CHECK "" )
      string( REGEX MATCH [0123456789]+\\.[0123456789]+\\.[0123456789]+\\.[0123456789]+ CMAKE_VERSION_TO_CHECK ${CMakeInstallExe} )
      get_filename_component( CMAKE_INSTALL_EXE_FILE_NAME ${CMakeInstallExe} NAME )
      string( REPLACE "/" "\\\\" TEMP_CMakeInstallExe ${CMakeInstallExe} )
      set( CMAKE_INSTALL_EXE_INSTALL_COMMAND_2 " Extract CMake installer\\n  File \\\"${TEMP_CMakeInstallExe}\\\"\\n" )
      set( CMAKE_INSTALL_EXE_INSTALL_COMMAND_3 " Wait a bit for system to unlock file.\\n  Sleep 1000\\n"
                                       " Delete CMake installer\\n  Delete \\\"$INSTDIR\\\\${CMAKE_INSTALL_EXE_FILE_NAME}\\\"\\n\\n" )

      # could not find a way to uninstall using the exe command line, no idea how to do that yet.
      #set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
      #                                         " Code to install CMake\\n  ReadRegStr $0 HKLM \\\"SOFTWARE\\\\Kitware\\\\CMake ${CMAKE_VERSION_TO_CHECK}\\\" \\\"\\\"\\n"
      #                                         " Check if CMake is installed\\n  StrCmp $0 \\\"\\\" uninstall_cmake_no 0\\n"
      #                                         " Check if uninstall CMake \\n  MessageBox MB_YESNO \\\"Do you want to uninstall CMake? It is recommended if no other builds on your system use CMake for configuration.\\\" IDYES uninstall_cmake_yes IDNO uninstall_cmake_no\\n"
      #                                         " A comment \\n  uninstall_cmake_yes:\\n"
      #                                         ${CMAKE_INSTALL_EXE_INSTALL_COMMAND_2}
      #                                         " Execute CMake installer, wait for completion\\n  ExecWait '\\\"$INSTDIR\\\\${CMAKE_INSTALL_EXE_FILE_NAME}\\\" /S'\\n"
      #                                         ${CMAKE_INSTALL_EXE_INSTALL_COMMAND_3}
      #                                         " A comment \\n  uninstall_cmake_no:\\n" )
      set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
                                             " Check if any sources selected for installation\\n IntOp $0 $H3DUtil_cpack_sources_selected | $HAPI_cpack_sources_selected\\n"
                                             " Because of dependencies check only H3DUtil and HAPI sources\\n \\\${If} $0 > 0\\n"
                                             " CMake major version\\n  StrCpy \\\$R0 \\\"-1\\\"\\n"
                                             " A comment\\n  trypreviouscmakeversion:\\n"
                                             " CMake reg key\\n  IntOp \\\$R0 \\\$R0 + 1\\n"
                                             " A comment\\n  ClearErrors\\n"
                                             " Read registry\\n  EnumRegKey $R1 SHCTX \\\"Software\\\\Kitware\\\" \\\"\\\$R0\\\"\\n"
                                             " Check if string found\\n  IfErrors install_cmake 0\\n"
                                             " Check if string empty\\n  StrCmp $R1 \\\"\\\" install_cmake 0\\n"
                                             " Copy first five\\n  StrCpy $R1 $R1 5\\n"
                                             " Check if string starts with CMake\\n  StrCmp $R1 \\\"CMake\\\" install_cmake_no trypreviouscmakeversion\\n"
                                             " A comment \\n install_cmake:\\n"
                                             ${CMAKE_INSTALL_EXE_INSTALL_COMMAND_2}
                                             "A comment \\n    ClearErrors\\n"
                                             " Execute cmake installer silent, wait for completion\\n  ExecWait '\\\"$INSTDIR\\\\${CMAKE_INSTALL_EXE_FILE_NAME}\\\" /S'\\n"
                                             "A comment \\n Goto cmake_end_install\\n"
                                             "A comment \\n cmake_install_not_hidden:\\n"
                                             " Execute cmake installer, wait for completion\\n  ExecWait '\\\"$INSTDIR\\\\${CMAKE_INSTALL_EXE_FILE_NAME}\\\"'\\n"
                                             " A comment \\n  cmake_end_install:\\n"
                                             ${CMAKE_INSTALL_EXE_INSTALL_COMMAND_3}
                                             "A comment \\n  install_cmake_no:\\n"
                                             "A comment \\n \\\${EndIf}\\n" )
    endif()

    # Modify path in the the NSIS template.
    set( CPACK_NSIS_MODIFY_PATH "ON" )

    configure_file( ${H3DAPI_SOURCE_DIR}/modules/NSIS.InstallOptions_64.ini.cmake ${CMAKE_CURRENT_BINARY_DIR}/NSIS.InstallOptions_64.ini )
    string( REPLACE "/" "\\\\" Temp_CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} )
    set( CPACK_NSIS_INSTALLOPTIONS_64 "${Temp_CMAKE_CURRENT_BINARY_DIR}\\\\NSIS.InstallOptions_64.ini" )
  else( WIN32 AND NOT UNIX )
    set( H3DAPI_CPACK_IGNORE_PATTERNS ${H3DAPI_CPACK_IGNORE_PATTERNS} /examples/Particledemo/
                                                                     "~$" )
    set( H3DAPI_CPACK_IGNORE_PATTERNS ${H3DAPI_CPACK_IGNORE_PATTERNS}
            "/CVS/;/.svn/;/.bzr/;/.hg/;/.git.*/;.swp$;.#;/#;~$" )
    set( CPACK_SOURCE_GENERATOR TGZ ZIP )
    set( CPACK_SOURCE_PACKAGE_FILE_NAME "h3dapi-${H3DAPI_MAJOR_VERSION}.${H3DAPI_MINOR_VERSION}.${H3DAPI_BUILD_VERSION}" )
    set( CPACK_SOURCE_IGNORE_FILES ${H3DAPI_CPACK_IGNORE_PATTERNS} )
    set( CPACK_SOURCE_INSTALLED_DIRECTORIES "${H3DAPI_SOURCE_DIR}/..;/" )
  endif()

  # Install header files
  install( FILES ${H3DAPI_HEADERS} ${H3DAPI_SOURCE_DIR}/../include/H3D/H3DApi.cmake
           DESTINATION H3DAPI/include/H3D
           COMPONENT H3DAPI_cpack_headers )

  # H3DApi.cmake that goes to headers is not needed unless sources is required.
  install( FILES ${H3DAPI_SOURCE_DIR}/../include/H3D/H3DApi.cmake
      DESTINATION H3DAPI/include/H3D
      COMPONENT H3DAPI_cpack_sources )

  # Install src files.
  install( FILES ${H3DAPI_SRCS}
           DESTINATION H3DAPI/src
           COMPONENT H3DAPI_cpack_sources )

  install( FILES ${H3DAPI_SOURCE_DIR}/../changelog
                 ${H3DAPI_SOURCE_DIR}/../LICENSE
                 ${H3DAPI_SOURCE_DIR}/../ReadMe
           DESTINATION H3DAPI
           COMPONENT H3DAPI_cpack_sources )

  install( FILES ${H3DAPI_SOURCE_DIR}/CMakeLists.txt
                 ${H3DAPI_SOURCE_DIR}/H3DAPI.rc.cmake
                 ${H3DAPI_SOURCE_DIR}/H3DAPICPack.cmake
                 ${H3DAPI_SOURCE_DIR}/H3DAPISourceFiles.txt
                 ${H3DAPI_SOURCE_DIR}/UpdateResourceFile.exe
           DESTINATION H3DAPI/build
           COMPONENT H3DAPI_cpack_sources )

  install( FILES ${H3DAPI_SOURCE_DIR}/modules/NSIS.InstallOptions.ini.in
                 ${H3DAPI_SOURCE_DIR}/modules/NSIS.InstallOptions_64.ini.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/NSIS.template.in
           DESTINATION H3DAPI/build/modules
           COMPONENT H3DAPI_cpack_sources )

  install( FILES ${H3DAPI_SOURCE_DIR}/modules/sharedModules/Find3DXWARE.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindAudiofile.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindBZip2.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindChai3D.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindCURL.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindDCMTK.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindDirectShow.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindDirectX.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindFFmpeg.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindFontConfig.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindFreeImage.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindFTGL.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindFreetype.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindGLEW.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindGLUT.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindGLUTWin.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DAPI.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DBZip2.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DCURL.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DFreetype.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DOpenAL.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DTeem.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DUtil.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindH3DZLIB.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindHAPI.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindLibOVR.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindMd5sum.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindNvidiaCG.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindOpenAL.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindOpenEXR.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindOpenHaptics.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindPTHREAD.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindSixenseSDK.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindSpiderMonkey.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindTeem.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindV8.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindVirtualHand.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindVorbis.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindWxWidgetsWin.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindXerces.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindXercesC.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/FindZLIB.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/H3DCommonFunctions.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/H3DExternalSearchPath.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/InstallH3DAPIAndExternals.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/StripAndAddLibraryDirectories.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/TestIfVCExpress.cmake
                 ${H3DAPI_SOURCE_DIR}/modules/sharedModules/UseDebian.cmake
           DESTINATION H3DAPI/build/modules/sharedModules
           COMPONENT H3DAPI_cpack_sources )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../Conformance
           DESTINATION H3DAPI
           COMPONENT H3DAPI_cpack_sources
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../examples
           DESTINATION H3DAPI
           COMPONENT H3DAPI_cpack_sources
           REGEX "(/.svn)|(/CVS)|(/Particledemo)" EXCLUDE
           PATTERN "berk.wrl" EXCLUDE
           PATTERN "berk_orig.x3d" EXCLUDE
           PATTERN "Kumanomi.wrl" EXCLUDE
           PATTERN "Kumanomi_orig.x3d" EXCLUDE
           PATTERN "humvee.WRL" EXCLUDE
           PATTERN "humvee_orig.x3d" EXCLUDE
           PATTERN "manikin.wrl" EXCLUDE
           PATTERN "manikin_orig.x3d" EXCLUDE
           PATTERN "moondial_orig.x3d" EXCLUDE
           PATTERN "themoondial.wrl" EXCLUDE
           PATTERN "bobcat2.x3d" EXCLUDE
           PATTERN "bobcat_nh.x3d" EXCLUDE
           PATTERN "bobcat_orig.x3d" EXCLUDE )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../H3DLoad
           DESTINATION H3DAPI
           COMPONENT H3DAPI_cpack_sources
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  if( TARGET H3DViewer )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI/H3DViewer/include/H3DViewerConfig.h
             DESTINATION H3DAPI/H3DViewer/src
             COMPONENT H3DAPI_cpack_sources )
  endif()
  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../H3DViewer
           DESTINATION H3DAPI
           COMPONENT H3DAPI_cpack_sources
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  if( EXISTS ${H3DAPI_SOURCE_DIR}/../lib/doc )
    install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../lib/doc
           DESTINATION H3DAPI/lib
           COMPONENT H3DAPI_cpack_headers
           REGEX "(/.svn)|(/CVS)" EXCLUDE )
  endif()

  install( FILES ${H3DAPI_SOURCE_DIR}/../lib/H3DInterface.py
                 ${H3DAPI_SOURCE_DIR}/../lib/H3DUtils.py
           DESTINATION H3DAPI/lib
           COMPONENT H3DAPI_cpack_sources )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../settings/common
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../settings/current
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../settings/display
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime
           REGEX "(/.svn)|(/CVS)" EXCLUDE )

  install( FILES ${H3DAPI_SOURCE_DIR}/../settings/h3dload.ini
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime )

  install( FILES ${H3DAPI_SOURCE_DIR}/../settings/dist.py
                 ${H3DAPI_SOURCE_DIR}/../settings/SettingsGUI.py
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_sources )

  if( EXISTS ${H3DAPI_SOURCE_DIR}/../settings/dist )
    install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../settings/dist/
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime
           REGEX "(/.svn)|(/CVS)" EXCLUDE )
  endif()

  if( EXISTS ${H3DAPI_SOURCE_DIR}/../settings/icons )
    install( DIRECTORY ${H3DAPI_SOURCE_DIR}/../settings/icons
           DESTINATION H3DAPI/settings
           COMPONENT H3DAPI_cpack_runtime
           REGEX "(/.svn)|(/CVS)" EXCLUDE )
  endif()

  # Add a cache variable H3DAPI_DOCS_DIRECTORY used to indicate where the H3DAPI docs are.
  if( NOT DEFINED H3DAPI_DOCS_DIRECTORY )
    set( H3DAPI_DOCS_DIRECTORY_DEFAULT "" )
    if( H3D_USE_DEPENDENCIES_ONLY )
      set( H3DAPI_DOCS_DIRECTORY_DEFAULT "${H3DAPI_SOURCE_DIR}/../../doc" )
    endif()
    set( H3DAPI_DOCS_DIRECTORY "${H3DAPI_DOCS_DIRECTORY_DEFAULT}" CACHE PATH "Set this to the directory containing the documentation of H3DAPI." )
    mark_as_advanced( H3DAPI_DOCS_DIRECTORY )
  endif()

  if( EXISTS ${H3DAPI_DOCS_DIRECTORY} )
    install( DIRECTORY ${H3DAPI_DOCS_DIRECTORY}/H3DAPI
             DESTINATION doc
             COMPONENT H3DAPI_cpack_headers
             REGEX "(/.svn)|(/CVS)" EXCLUDE )
    install( FILES "${H3DAPI_DOCS_DIRECTORY}/H3D API Manual.pdf"
             DESTINATION doc
             COMPONENT H3DAPI_cpack_headers )
  endif()

  # setting names and dependencies between components and also grouping them.
  set( CPACK_COMPONENT_H3DAPI_CPACK_RUNTIME_DISPLAY_NAME "Runtime" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_RUNTIME_DESCRIPTION "The runtime libraries ( dlls ) for H3DAPI." )
  set( CPACK_COMPONENT_H3DAPI_CPACK_RUNTIME_DEPENDS HAPI_cpack_runtime H3DAPI_cpack_external_runtime )
  set( CPACK_COMPONENT_H3DAPI_CPACK_RUNTIME_GROUP "H3DAPI_cpack_group" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_RUNTIME_INSTALL_TYPES Developer Full )

  set( CPACK_COMPONENT_H3DAPI_CPACK_LIBRARIES_DISPLAY_NAME "Libraries" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_LIBRARIES_DESCRIPTION "H3DAPI libraries, needed for building against H3DAPI." )
  set( CPACK_COMPONENT_H3DAPI_CPACK_LIBRARIES_DEPENDS HAPI_cpack_libraries H3DAPI_cpack_external_source H3DAPI_cpack_headers )
  set( CPACK_COMPONENT_H3DAPI_CPACK_LIBRARIES_GROUP "H3DAPI_cpack_group" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_LIBRARIES_INSTALL_TYPES Developer Full )

  set( CPACK_COMPONENT_H3DAPI_CPACK_HEADERS_DISPLAY_NAME "C++ Headers" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_HEADERS_DESCRIPTION "H3DAPI C++ headers, needed for building against H3DAPI." )
  set( CPACK_COMPONENT_H3DAPI_CPACK_HEADERS_DEPENDS HAPI_cpack_headers H3DAPI_cpack_external_source H3DAPI_cpack_libraries )
  set( CPACK_COMPONENT_H3DAPI_CPACK_HEADERS_GROUP "H3DAPI_cpack_group" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_HEADERS_INSTALL_TYPES Developer Full )

  set( CPACK_COMPONENT_H3DAPI_CPACK_SOURCES_DISPLAY_NAME "C++ Source" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_SOURCES_DESCRIPTION "Everything needed to build H3DAPI." )
  set( CPACK_COMPONENT_H3DAPI_CPACK_SOURCES_DEPENDS H3DAPI_cpack_headers HAPI_cpack_sources )
  set( CPACK_COMPONENT_H3DAPI_CPACK_SOURCES_GROUP "H3DAPI_cpack_group" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_SOURCES_INSTALL_TYPES Full )

  set( CPACK_COMPONENT_H3DAPI_CPACK_EXAMPLES_RUNTIME_DISPLAY_NAME "Example applications" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_EXAMPLES_RUNTIME_DESCRIPTION "The example applications for H3DAPI." )
  set( CPACK_COMPONENT_H3DAPI_CPACK_EXAMPLES_RUNTIME_DEPENDS H3DAPI_cpack_runtime )
  set( CPACK_COMPONENT_H3DAPI_CPACK_EXAMPLES_RUNTIME_GROUP "H3DAPI_cpack_group" )
  set( CPACK_COMPONENT_H3DAPI_CPACK_EXAMPLES_RUNTIME_INSTALL_TYPES Developer Full )

  set( CPACK_COMPONENT_GROUP_H3DAPI_CPACK_GROUP_DISPLAY_NAME "H3DAPI" )
  set( CPACK_COMPONENT_GROUP_H3DAPI_CPACK_GROUP_DESCRIPTION "H3DAPI is an open source, cross platform, scene graph API. Build X3D scenes by using the nodes written in H3DAPI. Load scenes using H3DViewer or H3DLoad that comes with this package." )
  set( CPACK_COMPONENT_GROUP_HAPI_CPACK_GROUP_PARENT_GROUP "H3DAPI_cpack_group" )

  # Add a cache variable H3D_cmake_runtime_path to point to cmake binary.
  set( H3D_cmake_runtime_path_default "" )
  if( NOT DEFINED H3D_cmake_runtime_path )
    if( WIN32 AND NOT UNIX )
      set( VERSION_CMAKES "4.0" "3.9" "3.8" "3.7" "3.6" "3.5" "3.4" "3.3" "3.2" "3.1" "3.0" "2.9" "2.8" "2.7" "2.6" )
      foreach( version_cmake ${VERSION_CMAKES} )
        if( EXISTS "C:/Program Files/CMake ${version_cmake}/bin/cmake.exe" )
          set( H3D_cmake_runtime_path_default "C:/Program Files/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        endif()

        if( EXISTS "C:/Program Files (x86)/CMake ${version_cmake}/bin/cmake.exe" )
          set( H3D_cmake_runtime_path_default "C:/Program Files (x86)/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        endif()

        if( EXISTS "C:/Program/CMake ${version_cmake}/bin/cmake.exe" )
          set( H3D_cmake_runtime_path_default "C:/Program/CMake ${version_cmake}/bin/cmake.exe" )
          break()
        endif()
      endforeach()
    else( WIN32 AND NOT UNIX )
      set( H3D_cmake_runtime_path_default "cmake" )
    endif()
    set( H3D_cmake_runtime_path ${H3D_cmake_runtime_path_default} CACHE FILEPATH "The path to the cmake runtime." )
    mark_as_advanced( H3D_cmake_runtime_path )
  endif()

  if( H3D_cmake_runtime_path )
    set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD}
                                                       COMMAND ${H3D_cmake_runtime_path}
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DAPI_cpack_runtime -P cmake_install.cmake
                                                       COMMAND ${H3D_cmake_runtime_path}
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DAPI_cpack_libraries -P cmake_install.cmake
                                                       COMMAND ${H3D_cmake_runtime_path}
                                                       ARGS -DBUILD_TYPE=$(Configuration) -DCOMPONENT=H3DAPI_cpack_examples_runtime -P cmake_install.cmake)

    if( NOT H3D_USE_DEPENDENCIES_ONLY )
      add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DummyFile
                          COMMAND echo )
      add_custom_target( INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                         DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/DummyFile )

      add_custom_command( TARGET INSTALL_RUNTIME_AND_LIBRARIES_ONLY
                          POST_BUILD
                          ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_POST_BUILD} )
      add_dependencies( INSTALL_RUNTIME_AND_LIBRARIES_ONLY H3DAPI ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} )
    endif()
  else( H3D_cmake_runtime_path )
    message( STATUS "H3D_cmake_runtime_path is not set, please set it to continue" )
  endif()

  if( NOT H3D_USE_DEPENDENCIES_ONLY )
    include( CPack )
    include( UseDebian )
    if( DEBIAN_FOUND )
      ADD_DEBIAN_TARGETS( H3DAPI )
    endif()
  endif()
endif()
