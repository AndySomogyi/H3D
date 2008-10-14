# - Find FREEIMAGE
# Find the native FREEIMAGE headers and libraries.
#
#  FREEIMAGE_INCLUDE_DIR -  where to find FREEIMAGE.h, etc.
#  FREEIMAGE_LIBRARIES    - List of libraries when using FREEIMAGE.
#  FREEIMAGE_FOUND        - True if FREEIMAGE found.


# Look for the header file.
FIND_PATH(FREEIMAGE_INCLUDE_DIR NAMES FreeImage.h
                                PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                      $ENV{H3D_EXTERNAL_ROOT}/include/FreeImage/Dist
                                      $ENV{H3D_ROOT}/../External/include
                                      $ENV{H3D_ROOT}/../External/include/FreeImage/Dist
                                      ../../External/include
                                      ../../External/include/FreeImage/Dist
                                      ${CMAKE_MODULE_PATH}/../../../External/include
                                      ${CMAKE_MODULE_PATH}/../../../External/include/FreeImage/Dist)

MARK_AS_ADVANCED(FREEIMAGE_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(FREEIMAGE_LIBRARY NAMES freeimage
                               PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                     $ENV{H3D_ROOT}/../External/lib
                                     ../../External/lib
                                     ${CMAKE_MODULE_PATH}/../../../External/lib )
MARK_AS_ADVANCED(FREEIMAGE_LIBRARY)

IF( WIN32 AND PREFER_STATIC_LIBRARIES )
  FIND_LIBRARY( FREEIMAGE_STATIC_LIBRARY NAMES FreeImage_static
                                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                         $ENV{H3D_ROOT}/../External/lib
                                         ../../External/lib
                                         ${CMAKE_MODULE_PATH}/../../../External/lib )
  MARK_AS_ADVANCED(FREEIMAGE_STATIC_LIBRARY)
ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES )

IF( FREEIMAGE_LIBRARY OR FREEIMAGE_STATIC_LIBRARY )
  SET( FREEIMAGE_LIBRARIES_FOUND 1 )
ENDIF( FREEIMAGE_LIBRARY OR FREEIMAGE_STATIC_LIBRARY )

# Copy the results to the output variables.
IF(FREEIMAGE_INCLUDE_DIR AND FREEIMAGE_LIBRARIES_FOUND)
  SET(FREEIMAGE_FOUND 1)

  IF( WIN32 AND PREFER_STATIC_LIBRARIES AND FREEIMAGE_STATIC_LIBRARY )
    SET(FREEIMAGE_LIBRARIES ${FREEIMAGE_STATIC_LIBRARY})
    SET( FREEIMAGE_LIB 1 )
  ELSE( WIN32 AND PREFER_STATIC_LIBRARIES AND FREEIMAGE_STATIC_LIBRARY )
    SET(FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY})
  ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES AND FREEIMAGE_STATIC_LIBRARY )

  SET(FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INCLUDE_DIR})
ELSE(FREEIMAGE_INCLUDE_DIR AND FREEIMAGE_LIBRARIES_FOUND)
  SET(FREEIMAGE_FOUND 0)
  SET(FREEIMAGE_LIBRARIES)
  SET(FREEIMAGE_INCLUDE_DIR)
ENDIF(FREEIMAGE_INCLUDE_DIR AND FREEIMAGE_LIBRARIES_FOUND)

# Report the results.
IF(NOT FREEIMAGE_FOUND)
  SET(FREEIMAGE_DIR_MESSAGE
    "FREEIMAGE was not found. Make sure FREEIMAGE_LIBRARY and FREEIMAGE_INCLUDE_DIR are set to the directory of your FreeImage installation. If you do not have it many image formats will not be available to use as textures")
  IF(FreeImage_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${FREEIMAGE_DIR_MESSAGE}")
  ELSEIF(NOT FreeImage_FIND_QUIETLY)
    MESSAGE(STATUS "${FREEIMAGE_DIR_MESSAGE}")
  ENDIF(FreeImage_FIND_REQUIRED)
ENDIF(NOT FREEIMAGE_FOUND)
