if( COMMAND cmake_policy )
  if( POLICY CMP0003 )
    cmake_policy( SET CMP0003 OLD )
  endif()
endif()
if( WIN32 )
  cmake_minimum_required( VERSION 2.6.0 )
endif()
# The name of our project is "H3DViewer".  CMakeLists files in this project can
# refer to the root source directory of the project as ${H3DViewer_SOURCE_DIR} and
# to the root binary directory of the project as ${H3DViewer_BINARY_DIR}.
project( H3DViewer )

# set the install directory to the H3D directory on Windows
if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${H3DViewer_SOURCE_DIR}/../../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif()

# Add a cache variable ENABLE_SVN_REVISION to have the choice of using
# SubWCRev.exe to embed svn revision number in generated DLLs.
# Default is YES for Visual Studio and NMake generators, NO otherwise.
if( MSVC AND NOT DEFINED ENABLE_SVN_REVISION )
  set( ENABLE_SVN_REVISION_DEFAULT "NO" )
  if( CMAKE_GENERATOR MATCHES "Visual Studio|NMake" )
    set( ENABLE_SVN_REVISION_DEFAULT "YES" )
  endif()
  set( ENABLE_SVN_REVISION ${ENABLE_SVN_REVISION_DEFAULT} CACHE BOOL "Use SubWCRev.exe to embed svn revision number in generated DLLs." )
endif()

# use, i.e. don't skip the full RPATH for the build tree
set( CMAKE_SKIP_BUILD_RPATH FALSE )

# when building, don't use the install RPATH already
# (but later on when installing)
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE ) 

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# the RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir )
if( "${isSystemDir}" STREQUAL "-1" )
   set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif()

set( H3DViewer_MAJOR_VERSION "2" )
set( H3DViewer_MINOR_VERSION "3" )
set( H3DViewer_BUILD_VERSION "1" )
set( H3DVIEWER_APP_NAME "H3DViewer ${H3DViewer_MAJOR_VERSION}.${H3DViewer_MINOR_VERSION}.${H3DViewer_BUILD_VERSION}" )

# Add a cache variable USE_PYTHON_CONSOLE to have the choice of enabling the
# interactive Python console in H3DViewer. Default is NO.
if( NOT DEFINED USE_PYTHON_CONSOLE )
  set( USE_PYTHON_CONSOLE "NO" CACHE BOOL "Decides if the interactive Python console is enabled in H3DViewer." )
endif()

# Include directories.
include_directories( ../src )

include( "${H3DViewer_SOURCE_DIR}/H3DViewerSourceFiles.txt" )

if( GENERATE_UNITY_BUILD )
  if( NOT DEFINED UNITY_BUILD_${PROJECT_NAME} )
    set(  UNITY_BUILD_${PROJECT_NAME} "YES" CACHE BOOL "Decides if a the generated project files should build through a unity build instead of a normal build. A unity builds packs all .cpp files into a UnityBuild.cpp file and then only include this in the project. This greatly reduces build times." )
  endif()

  if( UNITY_BUILD_${PROJECT_NAME} )
    # Generate a unity build, by creating the UnityBuild.cpp and only including the required 
    # source files.
    set( UNITYBUILD_INCLUDES "" )
    foreach( filename ${H3DViewer_SRCS} )
      set( UNITYBUILD_INCLUDES "${UNITYBUILD_INCLUDES}#include \"${filename}\"\n" )
    endforeach()

    # Using a cached variable with our string in it. Because cmake doesn't support multi-line strings we have to replace the newlines with a delimiter, so we arbitrarily use +=+.
    string( REPLACE "
" "+=+" UnitybuildIncludesConverted ${UNITYBUILD_INCLUDES} ) # Convert the file we're going to write to use our delimiter instead of newlines
    if( NOT ( UNITY_BUILD_CACHE_${PROJECT_NAME} ) OR NOT ( UnitybuildIncludesConverted STREQUAL UNITY_BUILD_CACHE_${PROJECT_NAME} )) # If we don't have the cache variable or if its contents don't match our new string then we write the unmodified new UnityBuild file and store the one with the swapped out delimiters in the cache variable
      message( STATUS "Updating UnityBuild.cpp for " ${PROJECT_NAME} )
      string( REPLACE "
" "+=+" unityBuildCacheNew ${UNITYBUILD_INCLUDES} )
      set( UNITY_BUILD_CACHE_${PROJECT_NAME} ${unityBuildCacheNew} CACHE INTERNAL "Used for determining if UnityBuild.cpp should be updated or not." )
      file( WRITE UnityBuild.cpp ${UNITYBUILD_INCLUDES} )
    else()
      message( STATUS "Unitybuild.cpp for ${PROJECT_NAME} already up to date" )
    endif()
    # overwrite the source files to only include the generated unity build file.
    set( H3DViewer_SRCS UnityBuild.cpp ) 
  endif()
endif()

set( PRECOMPILED_HEADERS_${PROJECT_NAME} "YES" CACHE BOOL "Decides if a the generated project files should use precompiled headers. This greatly reduces build times after first build." )

# set up precompiled headers  
if( MSVC AND PRECOMPILED_HEADERS_${PROJECT_NAME} )
  list( APPEND H3DViewer_HEADERS ${H3DViewer_SOURCE_DIR}/../src/StdAfx.h )
  list( APPEND H3DViewer_SRCS ${H3DViewer_SOURCE_DIR}/../src/StdAfx.cpp )
  if( ${MSVC_VERSION} LESS 1900 )
    set_source_files_properties( ${H3DViewer_SRCS}
                                 PROPERTIES COMPILE_FLAGS "/Zm900 /FIStdAfx.h /YuStdAfx.h" )
    set_source_files_properties( ${H3DViewer_SOURCE_DIR}/../src/StdAfx.cpp 
                                 PROPERTIES COMPILE_FLAGS "/Zm900 /YcStdAfx.h" )
  else()
    set_source_files_properties( ${H3DViewer_SRCS}
                                 PROPERTIES COMPILE_FLAGS "/FIStdAfx.h /YuStdAfx.h" )
    set_source_files_properties( ${H3DViewer_SOURCE_DIR}/../src/StdAfx.cpp 
                                 PROPERTIES COMPILE_FLAGS "/YcStdAfx.h" )
  endif()
endif()

# Add the H3DViewer.rc resource file if Visual Studio
if( MSVC )
  set( H3DViewer_SRCS ${H3DViewer_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc
                                        ${H3DViewer_SOURCE_DIR}/H3DViewer.ico )
endif()

# Add the directory to INCLUDE_DIRECTORIES before any other statement to make sure that this header file is found first.first.
# This assumes that we always use "#include <path/include_header.h>" in our code even if the file is actually found in
# the same directory as the file that contains the include statement.
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include )

# Add required libs to this variable.
set( required_libs )

# Where to find cmake modules used to find packages.
set( CMAKE_MODULE_PATH "${H3DViewer_SOURCE_DIR}/modules" "${H3DViewer_SOURCE_DIR}/../../build/modules/sharedModules" )
if( COMMAND cmake_policy )
  if( POLICY CMP0011 )
    cmake_policy( SET CMP0011 NEW )
  endif()
endif()
include( StripAndAddLibraryDirectories )

if( H3D_USE_DEPENDENCIES_ONLY )
  # The variables set here must be set by the CMakeLists.txt that sets H3D_USE_DEPENDENCIES_ONLY to true.
  include_directories( ${EXTERNAL_INCLUDE_DIR} ) 
endif()

if( TARGET H3DUtil )
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} H3DUtil )
else()
  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUTIL_FOUND )
    include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
    set( required_libs ${required_libs} ${H3DUTIL_LIBRARIES} )
  endif()
endif()

if( TARGET HAPI )
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} HAPI )
else()
  #HAPI
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIR} ) 
    set( required_libs ${required_libs} ${HAPI_LIBRARIES} )
  endif()
endif()

if( TARGET H3DAPI )
  include_directories( ${H3DAPI_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} H3DAPI )
else()
  #H3DAPI
  find_package( H3DAPI REQUIRED )

  if( H3DAPI_FOUND )
    include_directories( ${H3DAPI_INCLUDE_DIR} ) 
    set( required_libs ${required_libs} ${H3DAPI_LIBRARIES} )
  endif()
endif()

# WxWidgets.
set( WXWINDOWS_USE_GL 1 )
# find wx library propgrid
# The propgrid code does not work properly on Ubuntu 11.10 with the installed CMake and wx (2.8).
# It returns true even though propgrid does not exist, it probably ignores USE_LIBS that does not exist.
# Since there is no wxWidgets_VERSION variable or something like that there is code to check the end of
# include path. I have no idea if this would work on all other UNIX systems as well. NOTE, report
# this to CMAKE unless it is reported already.
find_package( wxWidgets QUIET COMPONENTS propgrid )
if( wxWidgets_FOUND )
  if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    set( wxwidgets_version_above_28 FALSE )
    foreach( TMP_INCLUDE_DIR ${wxWidgets_INCLUDE_DIRS} )
      string( REGEX MATCH "wx[-][0-9][.][0-9]+$" wx_version_match ${TMP_INCLUDE_DIR} )
      if( NOT ${wx_version_match} STREQUAL "" )
        string( REGEX MATCH "[0-9][.][0-9]+$" wx_version_match_version ${wx_version_match} )
        if( ${wx_version_match_version} VERSION_GREATER "2.8" )
          set( wxwidgets_version_above_28 TRUE )
        endif()
      endif()
    endforeach()
    if( NOT wxwidgets_version_above_28 )
      set( wxWidgets_FOUND 0 )
    endif()
  endif()
endif()

set( wx_components )
if( wxWidgets_FOUND )
  set( HAVE_WXPROPGRID 1 )
  set( wx_components base core gl adv richtext html propgrid qa )  
else()
  message( STATUS "wxWidgets propgrid not found. Needed for better tree view and program setting choice in H3DViewer. Part of wxWidgets 2.9." )
  set( wx_components base core gl adv richtext html qa )
endif()

# reset the wxWidgets_FOUND variable before running FIND_PACKAGE again.
set( wxWidgets_FOUND 0 )

find_package( wxWidgets REQUIRED COMPONENTS ${wx_components} )
if( wxWidgets_FOUND )
  if( WIN32 )
    include_directories( ${wxWidgets_INCLUDE_DIR} ) 
  else()
    include( ${wxWidgets_USE_FILE} )
  endif()
  set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
endif()

find_package( DirectX )
if( DirectX_FOUND )
  set( HAVE_DX9 1 )
  include_directories( ${DirectX_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${DirectX_LIBRARIES} )
endif()

# Create build files.
if( GENERATE_H3DVIEWER_CPACK_PROJECT )
  # if generating CPACK project using BUNDLE we should not create the bundle here instead
  # it is built with CPack 
  add_executable( H3DViewer WIN32 ${H3DViewer_SRCS} ${H3DViewer_HEADERS} )
else()
  add_executable( H3DViewer WIN32 MACOSX_BUNDLE ${H3DViewer_SRCS} ${H3DViewer_HEADERS} ${H3DViewer_SOURCE_DIR}/H3DViewer.icns )
endif()

# make sure that the H3DViewer.rc contains the correct svn-version
if( MSVC )

  set( H3DViewer_SVN_VERSION "0" )
  if( ENABLE_SVN_REVISION )
    # Find SubWCRev.exe
    find_file( SubWCRev
               NAMES "SubWCRev.exe"
               DOC   "Set to SubWCRev.exe that comes with TortoiseSVN. Used to find svn revision number." )
    mark_as_advanced( SubWCRev )
  endif()

  set( H3DViewer_is_working_copy 10 )
  if( ENABLE_SVN_REVISION AND SubWCRev )
    execute_process( COMMAND ${SubWCRev} ${H3DViewer_SOURCE_DIR}/../
                     RESULT_VARIABLE H3DViewer_is_working_copy )
    if( ${H3DViewer_is_working_copy} EQUAL 0 )
      set( H3DViewer_SVN_VERSION "$WCREV$" )
    endif()
  endif()

  # autogenerate H3DViewer.rc depending on the version
  configure_file( H3DViewer.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc )

  if( ENABLE_SVN_REVISION )
    add_custom_command( TARGET H3DViewer
                        PRE_BUILD
                        COMMAND ${H3DViewer_SOURCE_DIR}/../../build/UpdateResourceFile 
                        ARGS H3DViewer ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc ${H3DViewer_SOURCE_DIR}/H3DViewer.rc.cmake
                        ${H3DViewer_MAJOR_VERSION} ${H3DViewer_MINOR_VERSION}
                        ${H3DViewer_BUILD_VERSION} "${H3DViewer_SVN_VERSION}"
                        "H3DViewer_SOURCE_DIR"
                        ${H3DViewer_SOURCE_DIR}
                        "H3DViewer_Output_Name"
                        "$(TargetFileName)" )
  endif()

  if( ENABLE_SVN_REVISION AND SubWCRev AND ${H3DViewer_is_working_copy} EQUAL 0 )
    # Update SVN revision in file.
    execute_process( COMMAND ${SubWCRev} ${H3DViewer_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc )
    
    add_custom_command( TARGET H3DViewer 
                        PRE_BUILD 
                        COMMAND ${SubWCRev} 
                        ARGS ${H3DViewer_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer.rc )
  endif()
endif()


if( EXISTS ${H3DViewer_SOURCE_DIR}/../src/H3DViewerConfig.h )
  file( REMOVE ${H3DViewer_SOURCE_DIR}/../src/H3DViewerConfig.h )
endif()
configure_file( ${H3DViewer_SOURCE_DIR}/../src/H3DViewerConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/include/H3DViewerConfig.h )

if( NOT ( GENERATE_CPACK_PROJECT OR GENERATE_H3DVIEWER_CPACK_PROJECT ) )
  # Install header files on non-windows system (e.g. Unix).
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/include/H3DViewerConfig.h
           DESTINATION ${H3DViewer_SOURCE_DIR}/../src )
endif()

# Link against required_libs.
target_link_libraries( H3DViewer ${required_libs} )

if( COMMAND cmake_policy )
  if( POLICY CMP0054 )
    cmake_policy( SET CMP0054 OLD )
  endif()
endif()
foreach( h3dutil_include_dir_tmp ${H3DUTIL_INCLUDE_DIR} )
  if( EXISTS ${h3dutil_include_dir_tmp}/H3DUtil/H3DUtil.h )
    file( STRINGS ${h3dutil_include_dir_tmp}/H3DUtil/H3DUtil.h list_of_defines REGEX "#define THREAD_LOCK_DEBUG" )
    list( LENGTH list_of_defines list_of_defines_length )
    if( list_of_defines_length )
      if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" )
        execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
        if( GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7 )
          add_definitions( "-std=gnu++11" )
        elseif( GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3 )
          add_definitions( "-std=gnu++0x" )
        else()
          message( FATAL_ERROR "Enabling ENABLE_THREAD_LOCK_DEBUG requires C++11 support. This compiler lacks such support." )
        endif()
      elseif( ${MSVC_VERSION} LESS 1600 )
        message( FATAL_ERROR "Enabling ENABLE_THREAD_LOCK_DEBUG requires C++11 support. This compiler lacks such support." )
      endif()
    endif()
  endif()
endforeach()


# Set postfix for debug version of H3DViewer.
set_target_properties( H3DViewer PROPERTIES DEBUG_POSTFIX "_d" )

set_target_properties( H3DViewer
    PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME H3DViewer
    MACOSX_BUNDLE_ICON_FILE H3DViewer.icns
)

if( MSVC )

  set( H3DViewer_COMPILE_FLAGS "-DWXUSINGDLL" )
  if( GENERATE_H3DVIEWER_CPACK_PROJECT )
    set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} -DH3DVIEWER_STANDALONE" )
  endif()

  # Treat wchar_t as built in type for all visual studio versions.
  # This is default for every version above 7 (so far) but we still set it for all.
  set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} /Zc:wchar_t" )

  if( MSVC80 )
    # This might be useful for visual studio 2005 users that often recompile the api.
    if( NOT DEFINED USE_VC8_MP_FLAG )
      set( USE_VC8_MP_FLAG "NO" CACHE BOOL "In visual studio 8 the MP flag exists but is not documented. Maybe it is unsafe to use. If you want to use it then set this flag to yes." )
    endif()

    if( USE_VC8_MP_FLAG )
      set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} /MP" )
    endif()
  endif()

  if( ${MSVC_VERSION} GREATER 1399 )
    # Remove compiler warnings about deprecation for visual studio versions 8 and above.
    set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} -D_CRT_SECURE_NO_DEPRECATE" )
  endif()

  if( ${MSVC_VERSION} GREATER 1499 )
    # Build using several threads for visual studio versions 9 and above.
    set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} /MP" )
  endif()

  if( ${MSVC_VERSION} LESS 1600 )
    # Turn off optimization for visual studio versions 8 and below.
    # Old setting which I (Markus) have no idea why it was added.
    set( H3DViewer_COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS} /Od" )
  endif()

  # H3D API is now to big to link incrementally with vc. And remove link warning in debug build.
  set_target_properties( H3DViewer PROPERTIES LINK_FLAGS_DEBUG "/INCREMENTAL:NO /NODEFAULTLIB:msvcrt" )
  set_target_properties( H3DViewer PROPERTIES LINK_FLAGS "/INCREMENTAL:NO" )

  set_target_properties( H3DViewer PROPERTIES COMPILE_FLAGS "${H3DViewer_COMPILE_FLAGS}" )
  set_target_properties( H3DViewer PROPERTIES FOLDER "H3D" )
endif()

set( default_bin_install "bin" )
set( default_lib_install "lib" )

if( WIN32 )
  set( default_bin_install "bin32" )
  set( default_lib_install "lib32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set( default_bin_install "bin64" )
    set( default_lib_install "lib64" )
  endif()
endif()

# Install in these folders.
if( APPLE )
  if( NOT DEFINED OSX_BUNDLE_INSTALL_DESTINATION )
    set( OSX_BUNDLE_INSTALL_DESTINATION /Applications CACHE PATH "Directory into which to install the H3DViewer.app bundle." )
  endif()


  set_source_files_properties( ${H3DViewer_SOURCE_DIR}/H3DViewer.icns
                               PROPERTIES
                               MACOSX_PACKAGE_LOCATION Resources )

  install( TARGETS H3DViewer
           LIBRARY DESTINATION ${default_lib_install}
           RUNTIME DESTINATION ${default_bin_install}
           BUNDLE DESTINATION ${OSX_BUNDLE_INSTALL_DESTINATION}
           COMPONENT H3DAPI_cpack_runtime )
else()
  set( H3DViewer_package_install_dir "" )
  if( WIN32 AND GENERATE_H3DVIEWER_CPACK_PROJECT )
    set( H3DViewer_package_install_dir "H3DViewer/" )
  endif()

  set( H3DVIEWER_INSTALL_COMPONENT_NAME "H3DViewer_cpack_runtime" )
  if( GENERATE_CPACK_PROJECT )
    set( H3DVIEWER_INSTALL_COMPONENT_NAME "H3DAPI_cpack_runtime" )
  endif()

  install( TARGETS H3DViewer
           LIBRARY DESTINATION ${default_lib_install}
           RUNTIME DESTINATION ${H3DViewer_package_install_dir}${default_bin_install} COMPONENT ${H3DVIEWER_INSTALL_COMPONENT_NAME} )
endif()

# Add a cache variable GENERATE_H3DVIEWER_CPACK_PROJECT to have the choice of generating a project
# for packaging H3DViewer. Default is NO since most people will not use this.
if( NOT DEFINED GENERATE_H3DVIEWER_CPACK_PROJECT )
  set( GENERATE_H3DVIEWER_CPACK_PROJECT "NO" CACHE BOOL "Decides if a cpack project should be generated. The project in the first loaded CMakeLists will configure CPack." )
  mark_as_advanced( GENERATE_H3DVIEWER_CPACK_PROJECT )
endif()

if( GENERATE_H3DVIEWER_CPACK_PROJECT )
  include( ${H3DViewer_SOURCE_DIR}/H3DViewerCPack.cmake )
endif()
