# - Find VORBIS
# Find the native VORBIS headers and libraries.
#
#  VORBIS_INCLUDE_DIR -  where to find VORBIS.h, etc.
#  VORBIS_LIBRARIES    - List of libraries when using VORBIS.
#  VORBIS_FOUND        - True if VORBIS found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file.
FIND_PATH(VORBIS_INCLUDE_DIR NAMES vorbis/vorbisfile.h
                             PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                   $ENV{H3D_ROOT}/../External/include
                                   ../../External/include
                                   ${module_file_path}/../../../External/include
                             DOC "Path in which the file vorbis/vorbisfile.h is located." )
MARK_AS_ADVANCED(VORBIS_INCLUDE_DIR)

FIND_PATH(VORBIS_OGG_INCLUDE_DIR NAMES ogg/ogg.h
                          PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                $ENV{H3D_ROOT}/../External/include
                                ../../External/include
                                ${module_file_path}/../../../External/include
                          DOC "Path in which the file ogg/ogg.h is located." )
MARK_AS_ADVANCED(VORBIS_OGG_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(VORBIS_VORBISFILE_LIBRARY NAMES vorbisfile
                            PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                  ../../External/lib
                                  ${module_file_path}/../../../External/lib
                            DOC "Path to vorbisfile library." )
MARK_AS_ADVANCED(VORBIS_VORBISFILE_LIBRARY)

FIND_LIBRARY(VORBIS_LIBRARY NAMES vorbis
                            PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                  $ENV{H3D_ROOT}/../External/lib
                                  ../../External/lib
                                  ${module_file_path}/../../../External/lib
                            DOC "Path to vorbis library." )
MARK_AS_ADVANCED(VORBIS_LIBRARY)

FIND_LIBRARY(VORBIS_OGG_LIBRARY NAMES ogg 
                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                               $ENV{H3D_ROOT}/../External/lib
                               ../../External/lib
                               ${module_file_path}/../../../External/lib
                         DOC "Path to ogg library." )
MARK_AS_ADVANCED(VORBIS_OGG_LIBRARY)

IF( WIN32 AND PREFER_STATIC_LIBRARIES )
  FIND_LIBRARY( VORBIS_VORBISFILE_STATIC_LIBRARY NAMES vorbisfile_static
                                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                               $ENV{H3D_ROOT}/../External/lib
                                               ../../External/lib
                                               ${module_file_path}/../../../External/lib
                                         DOC "Path to vorbisfile static library." )
  MARK_AS_ADVANCED(VORBIS_VORBISFILE_STATIC_LIBRARY)
  
  FIND_LIBRARY( VORBIS_OGG_STATIC_LIBRARY NAMES ogg_static
                                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                               $ENV{H3D_ROOT}/../External/lib
                                               ../../External/lib
                                               ${module_file_path}/../../../External/lib
                                         DOC "Path to ogg static library." )
  MARK_AS_ADVANCED(VORBIS_OGG_STATIC_LIBRARY)
ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES )

IF( VORBIS_LIBRARY AND VORBIS_OGG_LIBRARY AND VORBIS_VORBISFILE_LIBRARY )
  SET( VORBIS_LIBRARIES_FOUND 1 )
ENDIF( VORBIS_LIBRARY AND VORBIS_OGG_LIBRARY AND VORBIS_VORBISFILE_LIBRARY )

IF( PREFER_STATIC_LIBRARIES AND VORBIS_VORBISFILE_STATIC_LIBRARY AND VORBIS_OGG_STATIC_LIBRARY )
  SET( VORBIS_LIBRARIES_FOUND 1 )
  SET( VORBIS_STATIC_LIBRARIES_FOUND 1 )
ENDIF( PREFER_STATIC_LIBRARIES AND VORBIS_VORBISFILE_STATIC_LIBRARY AND VORBIS_OGG_STATIC_LIBRARY )

# Copy the results to the output variables.
IF(VORBIS_INCLUDE_DIR AND VORBIS_OGG_INCLUDE_DIR AND VORBIS_LIBRARIES_FOUND)
  SET(VORBIS_FOUND 1)
  IF( VORBIS_STATIC_LIBRARIES_FOUND )
    SET(VORBIS_LIBRARIES ${VORBIS_VORBISFILE_STATIC_LIBRARY} ${VORBIS_OGG_STATIC_LIBRARY} )
  ELSE( VORBIS_STATIC_LIBRARIES_FOUND )
    SET(VORBIS_LIBRARIES ${VORBIS_VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${VORBIS_OGG_LIBRARY} )
  ENDIF( VORBIS_STATIC_LIBRARIES_FOUND )
  SET(VORBIS_INCLUDE_DIR ${VORBIS_INCLUDE_DIR} ${VORBIS_OGG_INCLUDE_DIR})
ELSE(VORBIS_INCLUDE_DIR AND VORBIS_OGG_INCLUDE_DIR AND VORBIS_LIBRARIES_FOUND)
  SET(VORBIS_FOUND 0)
  SET(VORBIS_LIBRARIES)
  SET(VORBIS_INCLUDE_DIR)
ENDIF(VORBIS_INCLUDE_DIR AND VORBIS_OGG_INCLUDE_DIR AND VORBIS_LIBRARIES_FOUND)

# Report the results.
IF(NOT VORBIS_FOUND)
  SET(VORBIS_DIR_MESSAGE
    "VORBIS was not found. Make sure cmake cache variables with prefix VORBIS are set
     to the locations of include and lib files for vorbis and ogg. If you do not have the library you will not be able to use ogg files as sound.")
  IF(Vorbis_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${VORBIS_DIR_MESSAGE}")
  ELSEIF(NOT Vorbis_FIND_QUIETLY)
    MESSAGE(STATUS "${VORBIS_DIR_MESSAGE}")
  ENDIF(Vorbis_FIND_REQUIRED)
ENDIF(NOT VORBIS_FOUND)
