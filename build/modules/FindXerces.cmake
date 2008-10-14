# - Find XERCES
# Find the native XERCES headers and libraries.
#
#  XERCES_INCLUDE_DIR -  where to find XERCES.h, etc.
#  XERCES_LIBRARIES    - List of libraries when using XERCES.
#  XERCES_FOUND        - True if XERCES found.


# Look for the header file.
FIND_PATH(XERCES_INCLUDE_DIR NAMES xercesc/sax2/Attributes.hpp
                             PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                   $ENV{H3D_ROOT}/../External/include
                                   ../../External/include
                                   ${CMAKE_MODULE_PATH}/../../../External/include )
MARK_AS_ADVANCED(XERCES_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(XERCES_LIBRARY NAMES xerces-c xerces-c_2  
                            PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                  $ENV{H3D_ROOT}/../External/lib
                                  ../../External/lib
                                  ${CMAKE_MODULE_PATH}/../../../External/lib )
MARK_AS_ADVANCED(XERCES_LIBRARY)

SET( XERCES_LIBRARIES_FOUND 0 )
SET( XERCES_STATIC_LIBRARIES_FOUND 0 )

IF( WIN32 AND PREFER_STATIC_LIBRARIES )
  FIND_LIBRARY( XERCES_STATIC_LIBRARY NAMES xerces-c_static_2
                                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                         $ENV{H3D_ROOT}/../External/lib
                                         ../../External/lib
                                         ${CMAKE_MODULE_PATH}/../../../External/lib )
  MARK_AS_ADVANCED(XERCES_STATIC_LIBRARY)

ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES )

IF( XERCES_STATIC_LIBRARY OR XERCES_LIBRARY )
  SET( XERCES_LIBRARIES_FOUND 1 )
ENDIF( XERCES_STATIC_LIBRARY OR XERCES_LIBRARY )

# Copy the results to the output variables.
IF(XERCES_INCLUDE_DIR AND XERCES_LIBRARIES_FOUND)
  SET(XERCES_FOUND 1)
  
  IF( XERCES_STATIC_LIBRARY  )
    SET(XERCES_LIBRARIES ${XERCES_STATIC_LIBRARY} ws2_32.lib )
    SET( XML_LIBRARY 1 )
  ELSE( XERCES_STATIC_LIBRARY )
    SET(XERCES_LIBRARIES ${XERCES_LIBRARY})
  ENDIF( XERCES_STATIC_LIBRARY )

  SET(XERCES_INCLUDE_DIR ${XERCES_INCLUDE_DIR})
ELSE(XERCES_INCLUDE_DIR AND XERCES_LIBRARIES_FOUND)
  SET(XERCES_FOUND 0)
  SET(XERCES_LIBRARIES)
  SET(XERCES_INCLUDE_DIR)
ENDIF(XERCES_INCLUDE_DIR AND XERCES_LIBRARIES_FOUND)

# Report the results.
IF(NOT XERCES_FOUND)
  SET(XERCES_DIR_MESSAGE
    "Xerces-c was not found. Make sure XERCES_LIBRARY and
    XERCES_INCLUDE_DIR are set to the directory of your xerces lib and
    include files. If you do not have Xerces x3d/xml files cannot be parsed.")
  IF(Xerces_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${XERCES_DIR_MESSAGE}")
  ELSEIF(NOT Xerces_FIND_QUIETLY)
    MESSAGE(STATUS "${XERCES_DIR_MESSAGE}")
  ENDIF(Xerces_FIND_REQUIRED)
ENDIF(NOT XERCES_FOUND)
