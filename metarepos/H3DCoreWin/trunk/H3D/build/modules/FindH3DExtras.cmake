# - Find H3DExtras
# Find the native H3DExtras headers and libraries.
#
#  H3DEXTRAS_INCLUDE_DIR -  where to find H3DExtras.h, etc.
#  H3DEXTRAS_LIBRARIES    - List of libraries when using H3DExtras.
#  H3DEXTRAS_FOUND        - True if H3DExtras found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

# Look for the header file.
FIND_PATH( H3DEXTRAS_INCLUDE_DIR NAMES H3D/H3DExtras/H3DExtras.h
           PATHS $ENV{H3D_ROOT}/../H3DExtras/include
                 ../../../H3DExtras/include
                 ../../H3DExtras/include
                 ${module_file_path}/../../../H3DExtras/include )
MARK_AS_ADVANCED(H3DEXTRAS_INCLUDE_DIR)

# Look for the library.
IF( MSVC )
  SET( H3DExtras_NAME "H3DExtras" )
ELSE(MSVC)
  SET( H3DExtras_NAME h3dextras )
ENDIF( MSVC )

SET( DEFAULT_LIB_INSTALL "lib" )
IF( WIN32 )
  SET( DEFAULT_LIB_INSTALL "lib32" )
  IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( DEFAULT_LIB_INSTALL "lib64" )
  ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
ENDIF( WIN32 )

FIND_LIBRARY( H3DExtras_LIBRARY NAMES ${H3DExtras_NAME}
              PATHS $ENV{H3D_ROOT}/../H3DExtras/${DEFAULT_LIB_INSTALL}
                    ../../../${DEFAULT_LIB_INSTALL}
                    ${CMAKE_MODULE_PATH}/../../../H3DExtras/${DEFAULT_LIB_INSTALL}
                    $ENV{H3D_ROOT}/../${DEFAULT_LIB_INSTALL} )

FIND_LIBRARY( H3DExtras_DEBUG_LIBRARY NAMES ${H3DExtras_NAME}_d
              PATHS $ENV{H3D_ROOT}/../H3DExtras/${DEFAULT_LIB_INSTALL}
                    ../../../${DEFAULT_LIB_INSTALL}
                    ${CMAKE_MODULE_PATH}/../../../H3DExtras/${DEFAULT_LIB_INSTALL}
                    $ENV{H3D_ROOT}/../${DEFAULT_LIB_INSTALL} )
MARK_AS_ADVANCED(H3DExtras_LIBRARY)
MARK_AS_ADVANCED(H3DExtras_DEBUG_LIBRARY)

IF(H3DExtras_LIBRARY OR H3DExtras_DEBUG_LIBRARY)
  SET( HAVE_H3DExtras_LIBRARY 1 )
ELSE(H3DExtras_LIBRARY OR H3DExtras_DEBUG_LIBRARY)
  SET( HAVE_H3DExtras_LIBRARY 0 )
ENDIF(H3DExtras_LIBRARY OR H3DExtras_DEBUG_LIBRARY)

# Copy the results to the output variables.
IF(H3DEXTRAS_INCLUDE_DIR AND HAVE_H3DExtras_LIBRARY)

  SET(H3DEXTRAS_FOUND 1)
  IF(H3DExtras_LIBRARY)
    SET(H3DEXTRAS_LIBRARIES ${H3DEXTRAS_LIBRARIES} optimized ${H3DExtras_LIBRARY} )
  ELSE(H3DExtras_LIBRARY)
    SET(H3DEXTRAS_LIBRARIES ${H3DEXTRAS_LIBRARIES} optimized ${H3DExtras_NAME} )
    MESSAGE( STATUS "H3DExtras release libraries not found. Release build might not work." )
  ENDIF(H3DExtras_LIBRARY)

  IF(H3DExtras_DEBUG_LIBRARY)
    SET(H3DEXTRAS_LIBRARIES ${H3DEXTRAS_LIBRARIES} debug ${H3DExtras_DEBUG_LIBRARY} )
  ELSE(H3DExtras_DEBUG_LIBRARY)
    SET(H3DEXTRAS_LIBRARIES ${H3DEXTRAS_LIBRARIES} debug ${H3DExtras_NAME}_d )
    MESSAGE( STATUS "H3DExtras debug libraries not found. Debug build might not work." )
  ENDIF(H3DExtras_DEBUG_LIBRARY)
  
  SET(H3DEXTRAS_INCLUDE_DIR ${H3DEXTRAS_INCLUDE_DIR} )
  SET(H3DEXTRAS_LIBRARIES ${H3DEXTRAS_LIBRARIES} )
ELSE(H3DEXTRAS_INCLUDE_DIR AND HAVE_H3DExtras_LIBRARY)
  SET(H3DEXTRAS_FOUND 0)
  SET(H3DEXTRAS_LIBRARIES)
  SET(H3DEXTRAS_INCLUDE_DIR)
ENDIF(H3DEXTRAS_INCLUDE_DIR AND HAVE_H3DExtras_LIBRARY)

# Report the results.
IF(NOT H3DEXTRAS_FOUND)
  SET(H3DExtras_DIR_MESSAGE
    "H3DExtras was not found. Make sure H3DExtras_LIBRARY ( and/or H3DExtras_DEBUG_LIBRARY ) and H3DEXTRAS_INCLUDE_DIR are set.")
  IF(H3DExtras_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${H3DExtras_DIR_MESSAGE}")
  ELSEIF(NOT H3DExtras_FIND_QUIETLY)
    MESSAGE(STATUS "${H3DExtras_DIR_MESSAGE}")
  ENDIF(H3DExtras_FIND_REQUIRED)
ENDIF(NOT H3DEXTRAS_FOUND)
