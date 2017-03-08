if( COMMAND cmake_policy )
  if( POLICY CMP0003 )
    cmake_policy( SET CMP0003 OLD )
  endif()
endif()

if( WIN32 )
  cmake_minimum_required( VERSION 2.6.0 )
endif()

project( H3DAPI )

# set the install directory to the H3D directory on Windows
if( WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET )
  set( CMAKE_INSTALL_PREFIX ${H3DAPI_SOURCE_DIR}/../.. CACHE PATH "Install path prefix, prepended onto install directories." FORCE )
  set( H3D_CMAKE_INSTALL_PREFIX_ALREADY_SET TRUE )
endif()

# Add the directory to INCLUDE_DIRECTORIES before any other statement to make sure that this header file is found first.
# This assumes that we always use "#include <path/include_header.h>" in our code even if the file is actually found in
# the same directory as the file that contains the include statement.
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include )
set( H3DAPI_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include ${H3DAPI_SOURCE_DIR}/../include CACHE INTERNAL "Set to internal to propagate change" )

# Make include files in paths like /usr/local/include have preference before
# files in OSX frameworks. 
set( CMAKE_FIND_FRAMEWORK "LAST" )

# make sure symbols are exported.
set( H3DAPI_COMPILE_FLAGS "-DH3DAPI_EXPORTS" )
set( H3DAPI_LINK_FLAGS "" )
set( H3DAPI_LINK_FLAGS_RELEASE "" )
set( H3DAPI_LINK_FLAGS_DEBUG "" )

# Add a cache variable GENERATE_CPACK_PROJECT to have the choice of generating a project
# for packaging H3DAPI. Default is NO since most people will not use this.
if( NOT DEFINED GENERATE_CPACK_PROJECT )
  set( GENERATE_CPACK_PROJECT "NO" CACHE BOOL "Decides if a cpack project should be generated. The project in the first loaded CMakeLists will configure CPack." )
  mark_as_advanced( GENERATE_CPACK_PROJECT )
endif()

# Add a cache variable GENERATE_UNITY_BUILD to have the choice of selecting
# a unity build project. Default is NO.
if( NOT DEFINED GENERATE_UNITY_BUILD )
  set( GENERATE_UNITY_BUILD "NO" CACHE BOOL "Decides if a the generated project files should build through a unity build instead of a normal build. A unity builds packs all .cpp files into a UnityBuild.cpp file and then only include this in the project. This greatly reduces build times." )
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

# Add a cache variable PREFER_STATIC_LIBRARIES to have the choice of generating a project
# linking against static libraries if they exist. Default is NO since most people will not use this.
if( NOT DEFINED PREFER_STATIC_LIBRARIES )
  set( PREFER_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries when both exist. To link FreeGLUT static see the PREFER_FREEGLUT_STATIC_LIBRARIES option." )
  mark_as_advanced( PREFER_STATIC_LIBRARIES )
endif()

if( WIN32 AND NOT DEFINED PREFER_FREEGLUT_STATIC_LIBRARIES )
  # Add a cache variable PREFER_FREEGLUT_STATIC_LIBRARIES to have the choice of generating a project
  # linking statically against glut. Default is NO since most people will not use this.
  set( PREFER_FREEGLUT_STATIC_LIBRARIES "NO" CACHE BOOL "Decides if CMake should prefer static libraries to dynamic libraries for FreeGLUT when both exist." )
  mark_as_advanced( PREFER_FREEGLUT_STATIC_LIBRARIES )
endif()


if( PREFER_STATIC_LIBRARIES )
  set( CMAKE_FIND_LIBRARY_SUFFIXES .a;${CMAKE_FIND_LIBRARY_SUFFIXES} )
endif()

# Add a cache variable GENERATE_NODEROUTESTODOTFILE_BUILD that should be
# set to yes if NodeRoutesToDotFile project should be run against this
# build of H3DAPI.
if( NOT DEFINED GENERATE_NODEROUTESTODOTFILE_BUILD )
  set( GENERATE_NODEROUTESTODOTFILE_BUILD "NO" CACHE BOOL "Breaks H3DAPI for normal use but this flag must be set to yes when using the NodeRoutesToDotFile project, located in ${H3DAPI_SOURCE_DIR}/../Util/NodeRoutesToDotFile." )
  mark_as_advanced( GENERATE_NODEROUTESTODOTFILE_BUILD )
endif()

# Include HAPI in the build.
if( WIN32 AND EXISTS ${H3DAPI_SOURCE_DIR}/../../HAPI/build )
  message( STATUS "Including HAPI" )
  add_subdirectory( ${H3DAPI_SOURCE_DIR}/../../HAPI/build
                    ${CMAKE_CURRENT_BINARY_DIR}/HAPI )

  get_directory_property( HAPI_INCLUDE_DIR_TMP DIRECTORY ${H3DAPI_SOURCE_DIR}/../../HAPI/build INCLUDE_DIRECTORIES )
  set( HAPI_INCLUDE_DIR ${HAPI_INCLUDE_DIR_TMP} CACHE INTERNAL "Set to internal so others can change it." )

  # Check if Haptik was found.
  if( Haptik_INCLUDE_DIR AND Haptik_LIBRARY )
    set( Haptik_FOUND 1 )
  endif()
endif()

# Include H3D header dir.
include_directories( ../include )

# Version of H3D API.
set( H3DAPI_MAJOR_VERSION 2 )
set( H3DAPI_MINOR_VERSION 3 )
set( H3DAPI_BUILD_VERSION 1 )


# Set here already in case a special NSIS template needs to be used by cpack.
set( CMAKE_MODULE_PATH ${H3DAPI_SOURCE_DIR}/modules ${H3DAPI_SOURCE_DIR}/modules/sharedModules )
if( COMMAND cmake_policy )
  if( POLICY CMP0011 )
    cmake_policy( SET CMP0011 NEW )
  endif()
endif()
include( StripAndAddLibraryDirectories )

# Version of project.
set( H3DAPI_FULL_VERSION
${H3DAPI_MAJOR_VERSION}.${H3DAPI_MINOR_VERSION}.${H3DAPI_BUILD_VERSION} ) 

# Add all sources, they are added to a variable called H3DAPI_SRCS
# defined in the included file. All header files are added to a variable called
# H3DAPI_HEADERS.
include( ${H3DAPI_SOURCE_DIR}/H3DAPISourceFiles.txt )

if( GENERATE_UNITY_BUILD )
  if( NOT DEFINED UNITY_BUILD_${PROJECT_NAME} )
    set(  UNITY_BUILD_${PROJECT_NAME} "YES" CACHE BOOL "Decides if a the generated project files should build through a unity build instead of a normal build. A unity builds packs all .cpp files into a UnityBuild.cpp file and then only include this in the project. This greatly reduces build times." )
  endif()

  if( UNITY_BUILD_${PROJECT_NAME} )  
    # Generate a unity build, by creating the UnityBuild.cpp and only including the required 
    # source files.
    set( UNITYBUILD_INCLUDES "" )

    foreach( filename ${NON_UNITYBUILD_FILES} )
      set( UNITYBUILD_INCLUDES "${UNITYBUILD_INCLUDES}\n #include \"${H3DAPI_SOURCE_DIR}/../src/${filename}\"" )
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
    if( MSVC )
      set( H3DAPI_COMPILE_FLAGS "${H3DAPI_COMPILE_FLAGS} /bigobj" )
    endif()

    set( H3DAPI_SRCS ${H3DAPI_SRCS_UNITYBUILD} )
  endif()
endif()

set( PRECOMPILED_HEADERS_${PROJECT_NAME} "YES" CACHE BOOL "Decides if a the generated project files should use precompiled headers. This greatly reduces build times after first build." )

# set up precompiled headers  
if( MSVC AND PRECOMPILED_HEADERS_${PROJECT_NAME} )
  list( APPEND H3DAPI_HEADERS ${H3DAPI_SOURCE_DIR}/../include/H3D/StdAfx.h )
  list( APPEND H3DAPI_SRCS ${H3DAPI_SOURCE_DIR}/../src/StdAfx.cpp )
  if( ${MSVC_VERSION} LESS 1900 )
    set_source_files_properties( ${H3DAPI_SRCS}
                                 PROPERTIES COMPILE_FLAGS "/Zm900 /FIH3D/StdAfx.h /YuH3D/StdAfx.h" )
    set_source_files_properties( ${H3DAPI_SOURCE_DIR}/../src/StdAfx.cpp 
                                 PROPERTIES COMPILE_FLAGS "/Zm900 /YcH3D/StdAfx.h" )
  else( ${MSVC_VERSION} LESS 1900 )
    set_source_files_properties( ${H3DAPI_SRCS}
                                 PROPERTIES COMPILE_FLAGS "/FIH3D/StdAfx.h /YuH3D/StdAfx.h" )
    set_source_files_properties( ${H3DAPI_SOURCE_DIR}/../src/StdAfx.cpp 
                                 PROPERTIES COMPILE_FLAGS "/YcH3D/StdAfx.h" )
  endif()
endif()


# Add the H3DAPI.rc resource file if Visual Studio
if( MSVC )
  set( H3DAPI_SRCS ${H3DAPI_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc )
endif()

# Add all optional libraries to this variable
set( optional_libs )

# Add all required libraries to this variable
set( required_libs )

if( NOT DEFINED DISABLE_H3D_DISPLAYLIST )
  set( DISABLE_H3D_DISPLAYLIST "OFF" CACHE BOOL "force disable h3d displaylist" )
  mark_as_advanced( DISABLE_H3D_DISPLAYLIST )
endif()

if( NOT DEFINED DISABLE_FIELD_ACCESS_TYPE_CHECK )
  set( DISABLE_FIELD_ACCESS_TYPE_CHECK "OFF" CACHE BOOL "force disable field access type check" )
  mark_as_advanced( DISABLE_FIELD_ACCESS_TYPE_CHECK )
endif()

if( NOT DEFINED DISABLE_PROTOTYPENODE )
  set( DISABLE_PROTOTYPENODE "OFF" CACHE BOOL "force disable usage of x3dprototypenode for optimization, do not turn on if you use x3dprototypenode" )
  mark_as_advanced( DISABLE_PROTOTYPENODE )
endif()

if( NOT DEFINED DEPRECATE_DISPLAYLIST_EVENT_COLLECT )
  set( DEPRECATE_DISPLAYLIST_EVENT_COLLECT "OFF" CACHE BOOL "force disable event collection for h3ddisplaylistobject to save cpu time. Note: its hasCausedEvent will no long report correctly " )
  mark_as_advanced( DEPRECATE_DISPLAYLIST_EVENT_COLLECT )
endif()

if( NOT DEFINED USE_APPLICATION_MANAGED_ACTIVE_TEXTURE_UNIT )
  set( USE_APPLICATION_MANAGED_ACTIVE_TEXTURE_UNIT "OFF" CACHE BOOL " Let H3DAPI to manage active texture value, instead of relying on opengl get function. H3DAPI rely on setTextureUnit and getTextureUnit to simply manage it " )
  mark_as_advanced( USE_APPLICATION_MANAGED_ACTIVE_TEXTURE_UNIT )
endif()




if( TARGET H3DUtil )
  # Target was created by ADD_SUBDIRECTORY command. There might not be anything in
  # H3DUTIL_INCLUDE_DIR but HAPI_INCLUDE_DIR covers that in this case.
  include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} H3DUtil )
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} H3DUtil )
else( TARGET H3DUtil )
  #H3DUtil
  find_package( H3DUtil REQUIRED )

  if( H3DUTIL_FOUND )
    include_directories( ${H3DUTIL_INCLUDE_DIR} ) 
    set( required_libs ${required_libs} ${H3DUTIL_LIBRARIES} )
  endif()
endif()

if( TARGET HAPI )
  # Target was created by ADD_SUBDIRECTORY command.
  include_directories( ${HAPI_INCLUDE_DIR} ) 
  set( required_libs ${required_libs} HAPI )
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} HAPI )
else( TARGET HAPI )
  #HAPI
  set( HAPI_DECIDES_RENDERER_SUPPORT 1 )
  find_package( HAPI REQUIRED )

  if( HAPI_FOUND )
    include_directories( ${HAPI_INCLUDE_DIR} ) 
    set( required_libs ${required_libs} ${HAPI_LIBRARIES} )
  endif()
endif()

if( TARGET Chai3DRenderer )
  set( required_libs ${required_libs} Chai3DRenderer )
  if( MSVC80 )
    set( required_libs ${required_libs} optimized atls.lib debug atlsd.lib )
  endif()
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} Chai3DRenderer )
endif()
  
if( TARGET OpenHapticsRenderer )
  set( required_libs ${required_libs} OpenHapticsRenderer )
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} OpenHapticsRenderer )
endif()

# Extra libs to link against.
if( WIN32 )
  # TODO: Only for MSVC?
  set( required_libs ${required_libs} winmm.lib DelayImp.lib )
endif()

# Find OpenGL library. Required to compile.
find_package( OpenGL REQUIRED )
if( OPENGL_FOUND )
  set( HAVE_OPENGL 1 )
  include_directories( ${OPENGL_INCLUDE_DIR} )
  set( required_libs ${required_libs} ${OPENGL_LIBRARIES} )
endif()

# Find Glew library. Required to compile.
find_package( GLEW REQUIRED )
if( GLEW_FOUND )
  include_directories( ${GLEW_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${GLEW_LIBRARIES} )
endif()

# Optional extra library. Makes is possible to use GLUT windows handling with H3D API.
find_package( GLUT )

if( GLUT_FOUND )
  set( HAVE_GLUT 1 )
  include_directories( ${GLUT_INCLUDE_DIR} )
  if( WIN32 )
    if( MINGW )
      set( required_libs ${required_libs} ${GLUT_LIBRARIES} )
    else( MINGW )
      # This is used in order for the compiler to find freeglut.lib.
      # If freeglut.lib is in any other place than glut32 on your system
      # either this CMakeLists.txt or FindGLUT needs to be changed
      # to allow for choice of extra library directory.
      STRIP_AND_ADD_LIBRARY_DIRECTORIES( ${GLUT_LIBRARIES} )
    endif()
  else( WIN32 )
    set( required_libs ${required_libs} ${GLUT_LIBRARIES} )
  endif()
endif()

# Optional extra library. Without this H3D API can not load scenes defined with X3D syntax.
find_package( XercesC )
if( XercesC_FOUND )
  set( HAVE_XERCES 1 )
  include_directories( ${XercesC_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${XercesC_LIBRARIES} )
endif()

# Optional extra library. Used in H3D API to resolved urls.
find_package( CURL )
if( CURL_FOUND )
  set( HAVE_LIBCURL 1 )
  include_directories( ${CURL_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${CURL_LIBRARIES} )
endif()

# Optional extra library. Needed to support shading using the CG shader language from NVidia.
find_package( NvidiaCG )
if( NvidiaCG_FOUND )
  set( HAVE_CG 1 )
  include_directories( ${NvidiaCG_INCLUDE_DIRS} ) 
  set( optional_libs ${optional_libs} ${NvidiaCG_LIBRARIES} )
endif()

# Optional extra library. Used to read compressed files.
find_package( ZLIB )
if( ZLIB_FOUND )
  include_directories( ${ZLIB_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${ZLIB_LIBRARIES} )
endif()

# Optional extra library. Add python bindings to H3D API, allow for extended prototyping without using C++.
find_package( PythonLibs )
if( PYTHON_INCLUDE_PATH AND PYTHON_LIBRARIES )
  set( PYTHON_FOUND 1 )
else( PYTHON_INCLUDE_PATH AND PYTHON_LIBRARIES )
  set( PYTHON_FOUND 0 )
endif()

# Report the results.
if( NOT PYTHON_FOUND )
  set( PYTHON_DIR_MESSAGE
    "PYTHON was not found. Make sure PYTHON_LIBRARY and PYTHON_INCLUDE_PATH are set." )
  if( NOT PYTHON_FIND_QUIETLY )
    message( STATUS "${PYTHON_DIR_MESSAGE}" )
  else( NOT PYTHON_FIND_QUIETLY )
    if( PYTHON_FIND_REQUIRED ) 
      message( FATAL_ERROR "${PYTHON_DIR_MESSAGE}" )
    endif()
  endif()
endif()

if( PYTHON_FOUND )
  set( HAVE_PYTHON 1 )
  include_directories( ${PYTHON_INCLUDE_PATH} ) 
  if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    # pyconfig.h is put in different directory from Ubuntu 13.04 (raring) 
    # and CMake FindPythonLibs module is not updated for this yet. 
    # Adding it explicitly here in the mean time.
    include_directories( /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/python2.7 ) 
  endif()
  set( optional_libs ${optional_libs} ${PYTHON_LIBRARIES} )
endif()

# Optional extra library. Used for sound in H3D API.
find_package( OpenAL )
if( OPENAL_FOUND )
  set( HAVE_OPENAL 1 )
  include_directories( ${OPENAL_INCLUDE_DIR} )
  set( optional_libs ${optional_libs} ${OPENAL_LIBRARIES} )
endif()

# Optional extra library. Used to read sound files.
find_package( Vorbis )
if( Vorbis_FOUND )
  set( HAVE_LIBVORBIS 1 )
  include_directories( ${Vorbis_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${Vorbis_LIBRARIES} )
endif()

# Optional extra library. Used for Oculus Rift support.
find_package( LibOVR )
if( LibOVR_FOUND )
  set( HAVE_LIBOVR 1 )
  include_directories( ${LibOVR_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${LibOVR_LIBRARIES} )
endif()

# Optional extra library. Used to read sound files.
find_package( Audiofile )
if( Audiofile_FOUND )
  set( HAVE_LIBAUDIOFILE 1 )
  include_directories( ${Audiofile_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${Audiofile_LIBRARIES} )
endif()

# Optional extra library. Used to handle fonts in H3D API.
find_package( Freetype )
if( FREETYPE_FOUND )
  set( HAVE_FREETYPE 1 )
  include_directories( ${FREETYPE_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${FREETYPE_LIBRARIES} )
endif()

# Optional extra library. Adds support to use devices by 3dxware. See www.3dconnexion.com.
find_package( 3DXWARE )
if( 3DXWARE_FOUND )
  set( HAVE_3DXWARE 1 )
  include_directories( ${3DXWARE_INCLUDE_DIRS} ) 
  set( optional_libs ${optional_libs} ${3DXWARE_LIBRARIES} )
endif()

# Optional extra library. Used for RazerHydraSensor
find_package( SixenseSDK )
if( SixenseSDK_FOUND )
  set( HAVE_SIXENSE 1 )
  include_directories( ${SixenseSDK_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${SixenseSDK_LIBRARIES} )
endif()

# Optional extra library. Used to render fonts in OpenGL.
find_package( FTGL )
if( FTGL_FOUND )
  set( HAVE_FTGL 1 )
  set( FTGL_INCLUDE_IS_UPPER ${FTGL_INCLUDE_IS_UPPER} )
  include_directories( ${FTGL_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${FTGL_LIBRARIES} )
endif()

# Optional extra library. Used to read images.
find_package( FreeImage )
if( FreeImage_FOUND )
  include_directories( ${FreeImage_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${FreeImage_LIBRARIES} )
endif()

if( Haptik_FOUND )
  set( optional_libs ${optional_libs} ${Haptik_LIBRARY} )
endif()

if( WIN32 )
  # Windows specific libraries

  # Optional extra library. Used to render movies as textures.
  find_package( DirectShow )
  if( DirectShow_FOUND )
    set( HAVE_DSHOW 1 )
    include_directories( ${DirectShow_INCLUDE_DIRS} )
    set( optional_libs ${optional_libs} ${DirectShow_LIBRARIES} )
  endif()
endif()

if( NOT WIN32 )
  # Linux specific libraries

  find_package( FontConfig )
  if( FONTCONFIG_FOUND )
    set( HAVE_FONTCONFIG 1 )
    include_directories( ${FONTCONFIG_INCLUDE_DIRS} )
    set( optional_libs ${optional_libs} ${FONTCONFIG_LIBRARIES} )
  endif()

  if( NOT APPLE )
    # Optional extra library. Used to render movies as textures.
    find_package( FFmpeg )
    if( FFMPEG_FOUND )
      set( HAVE_FFMPEG 1 )
      include_directories( ${FFMPEG_INCLUDE_DIRS} )
      list( APPEND optional_libs ${FFMPEG_LIBRARIES} )
    endif()  
  endif()
endif()

find_package( DCMTK )
if( DCMTK_FOUND )
  include_directories( ${DCMTK_INCLUDE_DIRS} ) 
  set( required_libs ${required_libs} ${DCMTK_LIBRARIES} )
endif()

find_package( Teem )
if( Teem_FOUND )
  include_directories( ${Teem_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${Teem_LIBRARIES} )
endif()

# Optional extra library. Used to handle OpenEXR HDR image formats.
find_package( OpenEXR )
if( OpenEXR_FOUND )
  set( HAVE_OPENEXR 1 )
  include_directories( ${OpenEXR_INCLUDE_DIRS} )
  set( optional_libs ${optional_libs} ${OpenEXR_LIBRARIES} )
endif()

# We do not use V8 anymore, only SpiderMonkey
#find_package( V8 )
#if( V8_FOUND )
#  set( HAVE_V8 1 )
#  include_directories( ${V8_INCLUDE_DIRS} )
#  set( required_libs ${required_libs} ${V8_LIBRARIES} )
#endif()

find_package( SpiderMonkey )
if( SpiderMonkey_FOUND )
  set( HAVE_SPIDERMONKEY 1 )
  include_directories( ${SpiderMonkey_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${SpiderMonkey_LIBRARIES} )
endif()

find_package( VirtualHand )
if( VirtualHand_FOUND )
  set( HAVE_VIRTUAL_HAND_SDK 1 )
  include_directories( ${VirtualHand_INCLUDE_DIRS} )
  set( required_libs ${required_libs} ${VirtualHand_LIBRARIES} )
endif()

# H3DAPI shared library definition
add_library( H3DAPI SHARED ${H3DAPI_SRCS} ${H3DAPI_HEADERS} )

# make sure that the H3DAPI.rc contains the correct svn-version
if( MSVC )
  set( H3DAPI_SVN_VERSION "0" )
  if( ENABLE_SVN_REVISION )
    # Find SubWCRev.exe
    find_file( SubWCRev
               NAMES "SubWCRev.exe"
               DOC   "Set to SubWCRev.exe that comes with TortoiseSVN. Used to find svn revision number." )
    mark_as_advanced( SubWCRev )
  endif()

  set( H3DAPI_is_working_copy 10 )
  if( ENABLE_SVN_REVISION AND SubWCRev )
    execute_process( COMMAND ${SubWCRev} ${H3DAPI_SOURCE_DIR}/../
                     RESULT_VARIABLE H3DAPI_is_working_copy )
    if( ${H3DAPI_is_working_copy} EQUAL 0 )
      set( H3DAPI_SVN_VERSION "$WCREV$" )
    endif()
  endif()

  # autogenerate H3DAPI.rc depending on the version
  configure_file( H3DAPI.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc )

  if( ENABLE_SVN_REVISION )
    add_custom_command( TARGET H3DAPI
                        PRE_BUILD
                        COMMAND ${H3DAPI_SOURCE_DIR}/UpdateResourceFile 
                        ARGS H3DAPI ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc ${H3DAPI_SOURCE_DIR}/H3DAPI.rc.cmake
                        ${H3DAPI_MAJOR_VERSION} ${H3DAPI_MINOR_VERSION}
                        ${H3DAPI_BUILD_VERSION} "${H3DAPI_SVN_VERSION}"
                        "H3DAPI_Output_Name"
                        "$(TargetFileName)" )
  endif()

  if( ENABLE_SVN_REVISION AND SubWCRev AND ${H3DAPI_is_working_copy} EQUAL 0 )
    # Update SVN revision in file.
    execute_process( COMMAND ${SubWCRev} ${H3DAPI_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc )

    add_custom_command( TARGET H3DAPI 
                        PRE_BUILD 
                        COMMAND ${SubWCRev} 
                        ARGS ${H3DAPI_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc ${CMAKE_CURRENT_BINARY_DIR}/H3DAPI.rc )
  endif()
endif()

# add the libraries needed for linking
target_link_libraries( H3DAPI ${required_libs} ${optional_libs} )

if( COMMAND cmake_policy )
  if( POLICY CMP0054 )
    cmake_policy( SET CMP0054 OLD )
  endif()
endif()
foreach( H3DUTIL_INCLUDE_DIR_TMP ${H3DUTIL_INCLUDE_DIR} )
  if( EXISTS ${H3DUTIL_INCLUDE_DIR_TMP}/H3DUtil/H3DUtil.h )
    file( STRINGS ${H3DUTIL_INCLUDE_DIR_TMP}/H3DUtil/H3DUtil.h list_of_defines REGEX "#define THREAD_LOCK_DEBUG" )
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


# make the name of debug libraries end in _d.
set_target_properties( H3DAPI PROPERTIES DEBUG_POSTFIX "_d" )

# set the version of the library
set_target_properties( H3DAPI PROPERTIES VERSION ${H3DAPI_FULL_VERSION} )

if( APPLE )
  set_target_properties( H3DAPI
                         PROPERTIES
#                        BUILD_WITH_INSTALL_RPATH 1
                         INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" )
endif()

if( UNIX )
  set_target_properties( H3DAPI PROPERTIES OUTPUT_NAME h3dapi )
endif()

if( MSVC )
  set_target_properties( H3DAPI PROPERTIES FOLDER "H3D" )
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
  # Temporary variables used later to set LINK_FLAGS.
  set( temp_output_postfix "" )

  # if we are using 64 bit compiler some object files get too big so we need
  # to add the /bigobj flag
  if( CMAKE_CL_64 )
    set( H3DAPI_COMPILE_FLAGS "${H3DAPI_COMPILE_FLAGS} /bigobj" )
  endif()
  
  if( MSVC )
    include( H3DCommonFunctions )
    # change the name depending on compiler to be able to tell them apart
    # since they are not compatible with each other. 
    setH3DMSVCOutputName( H3DAPI "H3DAPI" temp_output_postfix )
    # Add common H3D compile flags for microsoft visual studio
    addCommonH3DMSVCCompileFlags( H3DAPI_COMPILE_FLAGS )

    # H3D API is now to big to link incrementally with vc8.
    set( H3DAPI_LINK_FLAGS "${H3DAPI_LINK_FLAGS} /INCREMENTAL:NO" )
    set( H3DAPI_LINK_FLAGS_DEBUG "${H3DAPI_LINK_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt" )
  endif()

  # Set delayload properties of H3D API.
  if( MSVC )
    set( H3DAPI_LINK_FLAGS "${H3DAPI_LINK_FLAGS} /DELAYLOAD:\"cg.dll\" /DELAYLOAD:\"cgGL.dll\" /DELAYLOAD:\"vorbisfile.dll\" /DELAYLOAD:\"VirtualHandDevice.dll\" /DELAYLOAD:\"VirtualHandCore.dll\"" )
  endif()

  if( Haptik_FOUND AND MSVC )
    set( H3DAPI_LINK_FLAGS "${H3DAPI_LINK_FLAGS} /DELAYLOAD:\"Haptik.Library.dll\"" )
  endif()

  if( SIXENSE_FOUND AND MSVC )
    set( H3DAPI_LINK_FLAGS "${H3DAPI_LINK_FLAGS} /DELAYLOAD:\"sixense.dll\" /DELAYLOAD:\"sixense_utils.dll\" /DELAYLOAD:\"sixense_x64.dll\" /DELAYLOAD:\"sixense_utils_x64.dll\"" )
  endif()
  
  if( TARGET Chai3DRenderer AND MSVC )
    set( H3DAPI_LINK_FLAGS_DEBUG "${H3DAPI_LINK_FLAGS_DEBUG} /DELAYLOAD:\"Chai3DRenderer${temp_output_postfix}_d.dll\"" )
    set( H3DAPI_LINK_FLAGS_RELEASE "${H3DAPI_LINK_FLAGS_RELEASE} /DELAYLOAD:\"Chai3DRenderer${temp_output_postfix}.dll\"" )
  endif()
  
  if( TARGET OpenHapticsRenderer AND MSVC )
    set( H3DAPI_LINK_FLAGS_DEBUG "${H3DAPI_LINK_FLAGS_DEBUG} /DELAYLOAD:\"OpenHapticsRenderer${temp_output_postfix}_d.dll\"" )
    set( H3DAPI_LINK_FLAGS_RELEASE "${H3DAPI_LINK_FLAGS_RELEASE} /DELAYLOAD:\"OpenHapticsRenderer${temp_output_postfix}.dll\"" )
  endif()


endif()

if( GENERATE_NODEROUTESTODOTFILE_BUILD )
  set( H3DAPI_COMPILE_FLAGS "${H3DAPI_COMPILE_FLAGS} -DH3D_GENERATE_DOTROUTE_FILES" )
endif()

# Set compile and linker flags for H3DAPI.
  set_target_properties( H3DAPI
                         PROPERTIES  
                         COMPILE_FLAGS "${H3DAPI_COMPILE_FLAGS}"
                         LINK_FLAGS "${H3DAPI_LINK_FLAGS}"
                         LINK_FLAGS_DEBUG "${H3DAPI_LINK_FLAGS_DEBUG}"
                         LINK_FLAGS_RELEASE "${H3DAPI_LINK_FLAGS_RELEASE}"
                         LINK_FLAGS_RELWITHDEBINFO "${H3DAPI_LINK_FLAGS_RELEASE}"
                         LINK_FLAGS_MINSIZEREL "${H3DAPI_LINK_FLAGS_RELEASE}" )


# autogenerate H3DAPI.h depending on the libraries available.
if( EXISTS ${H3DAPI_SOURCE_DIR}/../include/H3D/H3DApi.h )
  file( REMOVE ${H3DAPI_SOURCE_DIR}/../include/H3D/H3DApi.h )
endif()
configure_file( ${H3DAPI_SOURCE_DIR}/../include/H3D/H3DApi.cmake ${CMAKE_CURRENT_BINARY_DIR}/include/H3D/H3DApi.h )

if( NOT GENERATE_CPACK_PROJECT )
  # Install header files on non-windows system (e.g. Unix).
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/include/H3D/H3DApi.h
           DESTINATION ${H3DAPI_SOURCE_DIR}/../include/H3D )
endif()

# Intall directories.
install( TARGETS H3DAPI
         LIBRARY DESTINATION ${default_lib_install} COMPONENT H3DAPI_cpack_runtime
         RUNTIME DESTINATION ${default_bin_install} COMPONENT H3DAPI_cpack_runtime
         ARCHIVE DESTINATION ${default_lib_install} COMPONENT H3DAPI_cpack_libraries )

if( NOT ( WIN32 OR GENERATE_CPACK_PROJECT ) )
  # Install include files.
  install( FILES ${H3DAPI_HEADERS}
           DESTINATION include/H3D )
endif()

if( NOT DEFINED H3DAPI_LOADER_PROJECTS )
  set( H3DAPI_LOADER_PROJECTS_DEFAULT "NO" )
  if( H3D_USE_DEPENDENCIES_ONLY )
    set( H3DAPI_LOADER_PROJECTS_DEFAULT "YES" )
  endif()
  set( H3DAPI_LOADER_PROJECTS "${H3DAPI_LOADER_PROJECTS_DEFAULT}" CACHE BOOL "If set to YES H3DLoad and H3DViewer will be included in the build." )
endif()

if( NOT DEFINED H3DAPI_H3DLOAD_PROJECTS )
  set( H3DAPI_H3DLOAD_PROJECTS_DEFAULT "YES" )
  set( H3DAPI_H3DLOAD_PROJECTS "${H3DAPI_H3DLOAD_PROJECTS_DEFAULT}" CACHE BOOL "If set to YES H3DLoad will be included in the build." )
endif()

if( H3DAPI_LOADER_PROJECTS )
  if( H3DAPI_H3DLOAD_PROJECTS )
    if( GLUT_FOUND )
      message( STATUS "Including H3DLoad" )
      add_subdirectory( ${H3DAPI_SOURCE_DIR}/../H3DLoad/build
                        ${CMAKE_CURRENT_BINARY_DIR}/H3DLoad )
      set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} H3DLoad )
    else( GLUT_FOUND )
      message( STATUS "GLUT was not found. H3DLoad will not be generated." )
    endif()
  endif()

  if( H3D_USE_DEPENDENCIES_ONLY )
    if( WIN32 )
      find_package( wxWidgetsWin )
      if( wxWidgets_FOUND )
        include_directories( ${wxWidgets_INCLUDE_DIR} ) 
        set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
      endif()
    endif()

    if( wxWidgets_FOUND )
    else( wxWidgets_FOUND )
      set( wxWidgets_USE_LIBS base core )
      find_package( wxWidgets )
      if( wxWidgets_FOUND )
        include( ${wxWidgets_USE_FILE} )
        set( required_libs ${required_libs} ${wxWidgets_LIBRARIES} )
      endif()
    endif()
  endif()

  if( ( NOT H3D_USE_DEPENDENCIES_ONLY ) OR wxWidgets_FOUND )
    message( STATUS "Including H3DViewer" )
    add_subdirectory( ${H3DAPI_SOURCE_DIR}/../H3DViewer/build
                      ${CMAKE_CURRENT_BINARY_DIR}/H3DViewer )
    set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} H3DViewer )
  else( ( NOT H3D_USE_DEPENDENCIES_ONLY ) OR wxWidgets_FOUND )
    message( WARNING "wxWidgets was not found. H3DViewer will not be generated." )
  endif()
endif()

if( NOT DEFINED H3DAPI_EXAMPLE_PROJECTS )
  set( H3DAPI_EXAMPLE_PROJECTS "NO" CACHE BOOL "If set to YES various example projects build on H3DAPI will be included in the build." )
endif()

if( H3DAPI_EXAMPLE_PROJECTS )
  if( GLUT_FOUND )
    message( STATUS "Including H3DAPI manual Examples" )
    add_subdirectory( ${H3DAPI_SOURCE_DIR}/../examples/manualExamples/C++
                      ${CMAKE_CURRENT_BINARY_DIR}/examples/manualExamples )
  endif()
  
  if( EXISTS ${H3DAPI_SOURCE_DIR}/../examples/Particledemo )
    if( ( NOT H3D_USE_DEPENDENCIES_ONLY ) OR wxWidgets_FOUND )
      message( STATUS "Including ParticleDemo" )
      add_subdirectory( ${H3DAPI_SOURCE_DIR}/../examples/Particledemo/build
                        ${CMAKE_CURRENT_BINARY_DIR}/examples/Particledemo )
      set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} ParticleDemo )
    else( ( NOT H3D_USE_DEPENDENCIES_ONLY ) OR wxWidgets_FOUND )
      message( WARNING "wxWidgets was not found. ParticleDemo will not be generated." )
    endif()
  endif()

  message( STATUS "Including H3DAPI ThreadExample" )
  add_subdirectory( ${H3DAPI_SOURCE_DIR}/../examples/ThreadExample
                    ${CMAKE_CURRENT_BINARY_DIR}/examples/ThreadExample )
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} ThreadExample Sphere_X3D Spheres_X3D )
endif()

if( H3D_USE_DEPENDENCIES_ONLY )
  set( INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES ${INSTALL_RUNTIME_AND_LIBRARIES_ONLY_DEPENDENCIES} PARENT_SCOPE )
else( H3D_USE_DEPENDENCIES_ONLY )
  include( ${H3DAPI_SOURCE_DIR}/H3DAPICPack.cmake )
endif()
