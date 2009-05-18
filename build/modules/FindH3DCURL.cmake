# - Find curl
# Find the native curl headers and libraries.
# If none is found for Windows use the distributed one in External
#
#  CURL_INCLUDE_DIR -  where to find curl.h, etc.
#  CURL_LIBRARIES    - List of libraries when using curl.
#  CURL_FOUND        - True if curlfound.

IF(H3DCURL_FIND_REQUIRED)
  FIND_PACKAGE(CURL REQUIRED)
ELSE(H3DCURL_FIND_REQUIRED)
  FIND_PACKAGE(CURL)
ENDIF(H3DCURL_FIND_REQUIRED)

IF( WIN32 )
IF(NOT CURL_FOUND OR PREFER_STATIC_LIBRARIES)
  GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )
  # Look for the header file.
  FIND_PATH( CURL_INCLUDE_DIR NAMES curl/curl.h
             PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                   $ENV{H3D_ROOT}/../External/include
                   ../../External/include
                   ${module_file_path}/../../../External/include )
  
  # Look for the library.
  FIND_LIBRARY( CURL_LIBRARY NAMES libcurl 
                PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                      $ENV{H3D_ROOT}/../External/lib
                      ../../External/lib
                      ${module_file_path}/../../../External/lib )

  IF( PREFER_STATIC_LIBRARIES )
    FIND_LIBRARY( CURL_STATIC_LIBRARY NAMES libcurl_static
                                            PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                                            $ENV{H3D_ROOT}/../External/lib
                                            ../../External/lib
                                            ${module_file_path}/../../../External/lib )
    MARK_AS_ADVANCED(CURL_STATIC_LIBRARY)
  ENDIF( PREFER_STATIC_LIBRARIES )

  IF( CURL_LIBRARY OR CURL_STATIC_LIBRARY )
    SET( CURL_LIBRARIES_FOUND 1 )
  ENDIF( CURL_LIBRARY OR CURL_STATIC_LIBRARY )
  
  IF(CURL_INCLUDE_DIR AND CURL_LIBRARIES_FOUND)
    SET(CURL_FOUND 1)

    IF( WIN32 AND PREFER_STATIC_LIBRARIES AND CURL_STATIC_LIBRARY )
      SET(CURL_LIBRARIES ${CURL_STATIC_LIBRARY})
      SET( CURL_STATICLIB 1 )
    ELSE( WIN32 AND PREFER_STATIC_LIBRARIES AND CURL_STATIC_LIBRARY )
      SET(CURL_LIBRARIES ${CURL_LIBRARY})
    ENDIF( WIN32 AND PREFER_STATIC_LIBRARIES AND CURL_STATIC_LIBRARY )

    SET(CURL_INCLUDE_DIR ${CURL_INCLUDE_DIR})
  ENDIF(CURL_INCLUDE_DIR AND CURL_LIBRARIES_FOUND)
ENDIF(NOT CURL_FOUND OR PREFER_STATIC_LIBRARIES)
ENDIF( WIN32 )

# Report the results.
IF(NOT CURL_FOUND)
  SET(CURL_DIR_MESSAGE
    "Curl was not found. Make sure CURL_LIBRARY and CURL_INCLUDE_DIR are set if compressed files support is desired.")
  IF(H3DCURL_FIND_REQUIRED)
    SET(CURL_DIR_MESSAGE
        "Curl was not found. Make sure CURL_LIBRARY and CURL_INCLUDE_DIR are set. CURL is required to build.")
    MESSAGE(FATAL_ERROR "${CURL_DIR_MESSAGE}")
  ELSEIF(NOT H3DCURL_FIND_QUIETLY)
    MESSAGE(STATUS "${CURL_DIR_MESSAGE}")
  ENDIF(H3DCURL_FIND_REQUIRED)
ENDIF(NOT CURL_FOUND)

MARK_AS_ADVANCED(CURL_INCLUDE_DIR CURL_LIBRARY)