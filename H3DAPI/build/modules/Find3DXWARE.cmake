# - Find 3dxware (www.3dconnexion.com)
#
#  3DXWARE_INCLUDE_DIR -  where to find si.h, siapp.h for Windows and xdrvlib.h, Xlib.h, Xutil.h, Xos.h, Xatom.h and keysym.h for LINUX.
#  3DXWARE_LIBRARIES    - List of libraries when using 3dxware.
#  3DXWARE_FOUND        - True if 3dxware is found.


# Look for the header file.
FIND_PATH( 3DXWARE_INCLUDE_DIR NAMES si.h siapp.h H3D/xdrvlib.h X11/Xlib.h X11/Xutil.h X11/Xos.h X11/Xatom.h X11/keysym.h
           PATHS  /usr/local/include 
                 $ENV{H3D_EXTERNAL_ROOT}/include  
                 $ENV{H3D_EXTERNAL_ROOT}/include/3dconnexion/inc
                 $ENV{H3D_ROOT}/../External/include  
                 $ENV{H3D_ROOT}/../External/include/3dconnexion/inc
                 ../../External/include
                 ../../External/include/3dconnexion/inc
                 ${CMAKE_MODULE_PATH}/../../../External/include
                 ${CMAKE_MODULE_PATH}/../../../External/include/3dconnexion/inc )
MARK_AS_ADVANCED(3DXWARE_INCLUDE_DIR)

# Look for the library siapp.
# TODO: Does this work on UNIX systems? (LINUX) I strongly doubt it. What are the libraries to find on linux?
FIND_LIBRARY( 3DXWARESIAPP_LIBRARY NAMES siapp
              PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                    $ENV{H3D_ROOT}/../External/lib
                    ../../External/lib
                    ${CMAKE_MODULE_PATH}/../../../External/lib )
MARK_AS_ADVANCED(3DXWARESIAPP_LIBRARY)

# Look for the library spwmath.
# Does this work on UNIX systems? (LINUX)
FIND_LIBRARY( 3DXWARESPWMATH_LIBRARY NAMES spwmath
              PATHS $ENV{H3D_EXTERNAL_ROOT}/lib
                    $ENV{H3D_ROOT}/../External/lib
                    ../../External/lib
                    ${CMAKE_MODULE_PATH}/../../../External/lib )
MARK_AS_ADVANCED(3DXWARESPWMATH_LIBRARY)

# Copy the results to the output variables.
IF(3DXWARE_INCLUDE_DIR AND 3DXWARESIAPP_LIBRARY AND 3DXWARESPWMATH)
  SET(3DXWARE_FOUND 1)
  SET(3DXWARE_LIBRARIES ${3DXWARESIAPP_LIBRARY} ${3DXWARESPWMATH_LIBRARY})
  SET(3DXWARE_INCLUDE_DIR ${3DXWARE_INCLUDE_DIR})
ELSE(3DXWARE_INCLUDE_DIR AND 3DXWARESIAPP_LIBRARY AND 3DXWARESPWMATH)
  SET(3DXWARE_FOUND 0)
  SET(3DXWARE_LIBRARIES)
  SET(3DXWARE_INCLUDE_DIR)
ENDIF(3DXWARE_INCLUDE_DIR AND 3DXWARESIAPP_LIBRARY AND 3DXWARESPWMATH)

# Report the results.
IF(NOT 3DXWARE_FOUND)
  SET(3DXWARE_DIR_MESSAGE
    "3dxware (ww.3dconnexion) was not found. Make sure 3DXWARE_LIBRARY and 3DXWARE_INCLUDE_DIR are set.")
  IF(3DXWARE_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${3DXWARE_DIR_MESSAGE}")
  ELSEIF(NOT 3DXWARE_FIND_QUIETLY)
    MESSAGE(STATUS "${3DXWARE_DIR_MESSAGE}")
  ENDIF(3DXWARE_FIND_REQUIRED)
ENDIF(NOT 3DXWARE_FOUND)