# - Find FTGL
# Find the native FTGL headers and libraries.
#
#  FTGL_INCLUDE_DIR -  where to find FTGL.h, etc.
#  FTGL_LIBRARIES    - List of libraries when using FTGL.
#  FTGL_FOUND        - True if FTGL found.


# Look for the header file.
FIND_PATH(FTGL_INCLUDE_DIR NAMES FTGL/FTGL.h 
                           PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                 $ENV{H3D_ROOT}/../External/include
                                 ../../External/include
                                 ${CMAKE_MODULE_PATH}/../../../External/include)
MARK_AS_ADVANCED(FTGL_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(FTGL_LIBRARY NAMES ftgl ftgl_dynamic_MTD
                          PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                $ENV{H3D_ROOT}/../External/lib
                                ../../External/lib
                                ${CMAKE_MODULE_PATH}/../../../External/lib)
MARK_AS_ADVANCED(FTGL_LIBRARY)

IF( WIN32 AND PREFER_STATIC_LIBRARIES )
  SET( FTGL_STATIC_LIBRARY_NAME ftgl_static_MTD )
  IF( MSVC80 )
    SET( FTGL_STATIC_LIBRARY_NAME ftgl_static_MTD_vc8 )
  ELSEIF( MSVC90 )
    SET( FTGL_STATIC_LIBRARY_NAME ftgl_static_MTD_vc9 )
  ENDIF( MSVC80 )
  
  FIND_LIBRARY( FTGL_STATIC_LIBRARY NAMES ${FTGL_STATIC_LIBRARY_NAME}
                                         PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                         $ENV{H3D_ROOT}/../External/lib
                                         ../../External/lib
                                         ${CMAKE_MODULE_PATH}/../../../External/lib )
  MARK_AS_ADVANCED(FTGL_STATIC_LIBRARY)
  
  FIND_LIBRARY( FTGL_STATIC_DEBUG_LIBRARY NAMES ${FTGL_STATIC_LIBRARY_NAME}_d
                                                PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                                $ENV{H3D_ROOT}/../External/lib
                                                ../../External/lib
                                                ${CMAKE_MODULE_PATH}/../../../External/lib )
  MARK_AS_ADVANCED(FTGL_STATIC_DEBUG_LIBRARY)
  
  IF( FTGL_STATIC_LIBRARY OR FTGL_STATIC_DEBUG_LIBRARY )
    SET( FTGL_STATIC_LIBRARIES_FOUND 1 )
  ENDIF( FTGL_STATIC_LIBRARY OR FTGL_STATIC_DEBUG_LIBRARY )
ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES )

IF( FTGL_LIBRARY OR FTGL_STATIC_LIBRARIES_FOUND )
  SET( FTGL_LIBRARIES_FOUND 1 )
ENDIF( FTGL_LIBRARY OR FTGL_STATIC_LIBRARIES_FOUND )

# Copy the results to the output variables.
IF(FTGL_INCLUDE_DIR AND FTGL_LIBRARIES_FOUND)
  SET(FTGL_FOUND 1)
  
  IF( WIN32 AND PREFER_STATIC_LIBRARIES AND FTGL_STATIC_LIBRARIES_FOUND )
    IF(FTGL_STATIC_LIBRARY)
      SET(FTGL_LIBRARIES optimized ${FTGL_STATIC_LIBRARY} )
    ELSE(FTGL_STATIC_LIBRARY)
      SET(FTGL_LIBRARIES optimized ${FTGL_STATIC_LIBRARY_NAME} )
      MESSAGE( STATUS, "FTGL static release libraries not found. Release build might not work." )
    ENDIF(FTGL_STATIC_LIBRARY)

    IF(FTGL_STATIC_DEBUG_LIBRARY)
      SET(FTGL_LIBRARIES ${FTGL_LIBRARIES} debug ${FTGL_STATIC_DEBUG_LIBRARY} )
    ELSE(FTGL_STATIC_DEBUG_LIBRARY)
      SET(FTGL_LIBRARIES ${FTGL_LIBRARIES} debug ${FTGL_STATIC_LIBRARY_NAME}_d )
      MESSAGE( STATUS, "FTGL static debug libraries not found. Debug build might not work." )
    ENDIF(FTGL_STATIC_DEBUG_LIBRARY)
    
    SET( FTGL_LIBRARY_STATIC 1 )
  ELSE( WIN32 AND PREFER_STATIC_LIBRARIES AND FTGL_STATIC_LIBRARIES_FOUND )
    SET(FTGL_LIBRARIES ${FTGL_LIBRARY})
  ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES AND FTGL_STATIC_LIBRARIES_FOUND )

  SET(FTGL_INCLUDE_DIR ${FTGL_INCLUDE_DIR})
ELSE(FTGL_INCLUDE_DIR AND FTGL_LIBRARIES_FOUND)
  SET(FTGL_FOUND 0)
  SET(FTGL_LIBRARIES)
  SET(FTGL_INCLUDE_DIR)
ENDIF(FTGL_INCLUDE_DIR AND FTGL_LIBRARIES_FOUND)

# Report the results.
IF(NOT FTGL_FOUND)
  SET(FTGL_DIR_MESSAGE
    "FTGL was not found. Make sure FTGL_LIBRARY and FTGL_INCLUDE_DIR are set to the directories containing the include and lib files for FTGL. If you do not have the library you will not be able to use the Text node.")
  IF(FTGL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "${FTGL_DIR_MESSAGE}")
  ELSEIF(NOT FTGL_FIND_QUIETLY)
    MESSAGE(STATUS "${FTGL_DIR_MESSAGE}")
  ELSE(NOT FTGL_FIND_QUIETLY)
  ENDIF(FTGL_FIND_REQUIRED)
ENDIF(NOT FTGL_FOUND)
