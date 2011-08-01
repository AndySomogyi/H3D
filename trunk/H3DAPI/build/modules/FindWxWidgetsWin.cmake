# - Find wxWidgets
# Find the native wxWidgets headers and libraries.
#
#  wxWidgets_INCLUDE_DIR -  where to find WxWidgets headers
#  wxWidgets_LIBRARIES    - List of libraries when using WxWidgets.
#  wxWidgets_FOUND        - True if WxWidgets found.

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

IF( CMAKE_CL_64 )
  SET( LIB "lib64" )
ELSE( CMAKE_CL_64 )
  SET( LIB "lib32" )
ENDIF( CMAKE_CL_64 )


# Look for the header file.
FIND_PATH(wxWidgets_INCLUDE_DIR NAMES wx/wx.h 
                                PATHS $ENV{H3D_EXTERNAL_ROOT}/include
                                      $ENV{H3D_ROOT}/../External/include
                                      ../../External/include
                                      ${module_file_path}/../../../External/include
                                DOC "Path in which the file wx/wx.h is located." )
MARK_AS_ADVANCED(wxWidgets_INCLUDE_DIR)

IF(wxWidgets_USE_LIBS)
  SET(wxlibs ${wxWidgets_USE_LIBS})
ELSE(wxWidgets_USE_LIBS)
  SET( wxlibs core adv aui html media xrc dbgrid gl qa richtext )
ENDIF(wxWidgets_USE_LIBS)


FOREACH(WXLIB ${wxlibs} )
      FIND_LIBRARY(wxWidgets_${WXLIB}${_DBG}_LIBRARY
        NAMES
        wxmsw29u${_DBG}_${WXLIB}
        wxmsw28${_UCD}${_DBG}_${WXLIB}
        PATHS 
  	$ENV{H3D_EXTERNAL_ROOT}/${LIB}
        $ENV{H3D_ROOT}/../External/${LIB}
        ../../External/${LIB}
        ${module_file_path}/../../../External/${LIB}
        DOC 
	"Path to wx ${WXLIB} library."
        )
        MARK_AS_ADVANCED(wxWidgets_${WXLIB}_LIBRARY)
ENDFOREACH( WXLIB )


FIND_LIBRARY(wxWidgets_base_LIBRARY NAMES wxbase29u wxbase28 
                                      PATHS $ENV{H3D_EXTERNAL_ROOT}/${LIB}
                                            $ENV{H3D_ROOT}/../External/${LIB}
                                            ../../External/${LIB}
                                            ${module_file_path}/../../../External/${LIB}
                                      DOC "Path to wx base library." )

IF( wxWidgets_base_LIBRARY )
  SET( wxWidgets_FOUND 1 )
ELSE( wxWidgets_base_LIBRARY )
  SET( wxWidgets_FOUND 0 )
ENDIF( wxWidgets_base_LIBRARY )

FOREACH(WXLIB ${wxlibs} )
  IF( NOT wxWidgets_${WXLIB}${_DBG}_LIBRARY )
    SET( wxWidgets_FOUND 0 )
  ENDIF( NOT wxWidgets_${WXLIB}${_DBG}_LIBRARY )
ENDFOREACH( WXLIB )

# Copy the results to the output variables.
IF(wxWidgets_INCLUDE_DIR AND wxWidgets_base_LIBRARY AND wxWidgets_FOUND )
  SET( wxWidgets_FOUND 1)
  SET( wxWidgets_LIBRARIES ${wxWidgets_base_LIBRARY} comctl32 Rpcrt4 )
  FOREACH( WXLIB ${wxlibs} )
    SET( wxWidgets_LIBRARIES ${wxWidgets_LIBRARIES} ${wxWidgets_${WXLIB}${_DBG}_LIBRARY} )
  ENDFOREACH( WXLIB ${wxlibs} )
  SET( wxWidgets_INCLUDE_DIR ${wxWidgets_INCLUDE_DIR})
ELSE(wxWidgets_INCLUDE_DIR AND wxWidgets_base_LIBRARY AND wxWidgets_FOUND )
  SET(wxWidgets_FOUND 0)
  SET(wxWidgets_LIBRARIES)
  SET(wxWidgets_INCLUDE_DIR)
ENDIF(wxWidgets_INCLUDE_DIR AND wxWidgets_base_LIBRARY AND wxWidgets_FOUND )

# Report the results.
IF(NOT wxWidgets_FOUND)
  SET(wxWidgets_DIR_MESSAGE
    "WxWidgets was not found. Make sure wxWidgets_core_LIBRARY, wxWidgets_base_LIBRARY")
   SET( wxWidgets_DIR_MESSAGE "${wxWidgets_DIR_MESSAGE} and wxWidgets_INCLUDE_DIR are set and other requested libs are set.")
  IF(wxWidgets_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "${wxWidgets_DIR_MESSAGE}")
  ELSEIF(NOT wxWidgets_FIND_QUIETLY)
    MESSAGE(STATUS "${wxWidgets_DIR_MESSAGE}")
  ENDIF(wxWidgets_FIND_REQUIRED)
ENDIF(NOT wxWidgets_FOUND)
