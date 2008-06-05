# - Find H3DAPI
# Find the native H3DAPI headers and libraries.
#
#  H3DAPI_INCLUDE_DIR -  where to find H3DAPI.h, etc.
#  H3DAPI_LIBRARIES    - List of libraries when using H3DAPI.
#  H3DAPI_FOUND        - True if H3DAPI found.


# Look for the header file.
FIND_PATH( H3DAPI_INCLUDE_DIR NAMES H3D/H3DApi.h
           PATHS $ENV{H3D_ROOT}/include
                 ../../include )
MARK_AS_ADVANCED(H3DAPI_INCLUDE_DIR)

# Look for the library.
IF(MSVC70 OR MSVC71)
  SET( H3DAPI_NAME H3DAPI_vc7 )
ELSEIF(MSVC80)
  SET( H3DAPI_NAME H3DAPI_vc8 )
ELSEIF(MSVC90)
  SET( H3DAPI_NAME H3DAPI_vc9 )
ELSE(MSVC90)
  SET( H3DAPI_NAME H3DAPI )
ENDIF(MSVC70 OR MSVC71)

FIND_LIBRARY( H3DAPI_LIBRARY NAMES ${H3DAPI_NAME}
              PATHS $ENV{H3D_ROOT}/../lib
                    ../../lib )

FIND_LIBRARY( H3DAPI_DEBUG_LIBRARY NAMES ${H3DAPI_NAME}_d
              PATHS $ENV{H3D_ROOT}/../lib
                    ../../lib )
MARK_AS_ADVANCED(H3DAPI_LIBRARY)
MARK_AS_ADVANCED(H3DAPI_DEBUG_LIBRARY)

IF(H3DAPI_LIBRARY OR H3DAPI_DEBUG_LIBRARY)
  SET( HAVE_H3DAPI_LIBRARY 1 )
ELSE(H3DAPI_LIBRARY OR H3DAPI_DEBUG_LIBRARY)
  SET( HAVE_H3DAPI_LIBRARY 0 )
ENDIF(H3DAPI_LIBRARY OR H3DAPI_DEBUG_LIBRARY)

# Copy the results to the output variables.
IF(H3DAPI_INCLUDE_DIR AND HAVE_H3DAPI_LIBRARY)
  
  # OpenGL is required for using the H3DAPI library.
  FIND_PACKAGE(OpenGL REQUIRED)
  IF(OPENGL_FOUND)
    SET( H3DAPI_INCLUDE_DIR ${H3DAPI_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} )
    SET( H3DAPI_LIBRARIES ${OPENGL_LIBRARIES} )
  ENDIF(OPENGL_FOUND)
  
  # Glew is required for using the H3DAPI library
  # On windows this will also find opengl header includes.
  FIND_PACKAGE(GLEW REQUIRED)
  IF(GLEW_FOUND)
    SET( H3DAPI_INCLUDE_DIR ${H3DAPI_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} )
    SET( H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} ${GLEW_LIBRARIES} )
  ENDIF(GLEW_FOUND)

  SET(H3DAPI_FOUND 1)
  IF(H3DAPI_LIBRARY)
    SET(H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} optimized ${H3DAPI_LIBRARY} )
  ELSE(H3DAPI_LIBRARY)
    SET(H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} optimized ${H3DAPI_NAME} )
    MESSAGE( STATUS, "H3DAPI release libraries not found. Release build might not work." )
  ENDIF(H3DAPI_LIBRARY)

  IF(H3DAPI_DEBUG_LIBRARY)
    SET(H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} debug ${H3DAPI_DEBUG_LIBRARY} )
  ELSE(H3DAPI_DEBUG_LIBRARY)
    SET(H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} debug ${H3DAPI_NAME}_d )
    MESSAGE( STATUS, "H3DAPI debug libraries not found. Debug build might not work." )
  ENDIF(H3DAPI_DEBUG_LIBRARY)
  
  SET(H3DAPI_INCLUDE_DIR ${H3DAPI_INCLUDE_DIR} )
  SET(H3DAPI_LIBRARIES ${H3DAPI_LIBRARIES} )
ELSE(H3DAPI_INCLUDE_DIR AND HAVE_H3DAPI_LIBRARY)
  SET(H3DAPI_FOUND 0)
  SET(H3DAPI_LIBRARIES)
  SET(H3DAPI_INCLUDE_DIR)
ENDIF(H3DAPI_INCLUDE_DIR AND HAVE_H3DAPI_LIBRARY)

# Report the results.
IF(NOT H3DAPI_FOUND)
  SET(H3DAPI_DIR_MESSAGE
    "H3DAPI was not found. Make sure H3DAPI_LIBRARY ( and/or H3DAPI_DEBUG_LIBRARY ) and H3DAPI_INCLUDE_DIR are set.")
  IF(H3DAPI_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${H3DAPI_DIR_MESSAGE}")
  ELSEIF(NOT H3DAPI_FIND_QUIETLY)
    MESSAGE(STATUS "${H3DAPI_DIR_MESSAGE}")
  ENDIF(H3DAPI_FIND_REQUIRED)
ENDIF(NOT H3DAPI_FOUND)