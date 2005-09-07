
from H3D import *

###################################################################
#
# Base types; Vec2f, Vec3f, Vec2d, Vec2d, etc, etc, etc
#
###################################################################



###################################################################
#
# Field base class and SF* MF* class definitions
#
###################################################################
class Field:
  type = UNKNOWN_X3D_TYPE
  def __init__( self, auto_update = 0 ):
    createField( self, auto_update )

  def setValue( self, value ):
    setFieldValue( self, value )

  def getValue( self ):
    return getFieldValue( self )
  
  def route( self, dest ):
    return routeField( self, dest )

  def routeNoEvent( self, dest ):
    return routeFieldNoEvent( self, dest )

  def unroute( self, dest ):
    return unrouteField( self, dest )

#  def replaceRoute( self, dest, i ):
#    return unrouteField( self, dest )

#  def replaceRouteNoEvent( self, dest, i ):
#    return unrouteField( self, dest, i )

  def touch( self ):
    return touchField( self )

  def getRoutesIn( self ):
    return getRoutesIn( self )

  def getRoutesOut( self ):
    return getRoutesOut( self )





# Install all built-in Field types:
types = [ 
  ( SFFLOAT,    "SFFloat" ),
  ( MFFLOAT,    "MFFloat" ),
  ( SFDOUBLE,   "SFDouble" ),
  ( MFDOUBLE,   "MFDouble" ),
  ( SFTIME,     "SFTime" ),
  ( MFTIME,     "MFTime" ),
  ( SFINT32,    "SFInt32" ),
  ( MFINT32,    "MFInt32" ),
  ( SFVEC2F,    "SFVec2f" ),
  ( MFVEC2F,    "MFVec2f" ),
  ( SFVEC2D,    "SFVec2d" ),
  ( MFVEC2D,    "MFVec2d" ),
  ( SFVEC3F,    "SFVec3f" ),
  ( MFVEC3F,    "MFVec3f" ),
  ( SFVEC3D,    "SFVec3d" ),
  ( MFVEC3D,    "MFVec3d" ),
  ( SFBOOL,     "SFBool"  ),
  ( MFBOOL,     "MFBool"  ),
  ( SFSTRING,   "SFString" ),
  ( MFSTRING,   "MFString" ),
  ( SFCOLOR,    "SFColor" ),
  ( MFCOLOR,    "MFColor" ),
  ( SFCOLORRGBA,"SFColorRGBA" ),
  ( MFCOLORRGBA,"MFColorRGBA" ),
  ( SFROTATION, "SFRotation" ),
  ( MFROTATION, "MFRotation" ),
  ( SFMATRIX3F, "SFMatrix3f" ),
  ( MFMATRIX3F, "MFMatrix3f" ),
  ( SFMATRIX4F, "SFMatrix4f" ),
  ( MFMATRIX4F, "MFMatrix4f" ),
  ( SFNODE    , "SFNode"     ),
  ( MFNODE    , "MFNode"     ) ]

for t in types:
  exec """
class %s( Field ):
  type = %s
""" % (t[1], t[0] )


def TypedField( base_class, type_info = None ):
  class TypedBase( base_class ):
    pass
  if type_info == None:
    TypedBase.__type_info__ = ()
  elif type( type_info ) != type(()):
    TypedBase.__type_info__ = ( type_info, )
  else:
    TypedBase.__type_info__ = type_info
  return TypedBase


auto_update_classes = {}

# AutoUpdate "template" as in C++
def AutoUpdate( base_class ):
  class AutoUpdateBase( base_class ):
    def __init__( self ):
      base_class.__init__( self, 1 )

  global auto_update_classes
  if( auto_update_classes.has_key( base_class ) ):
    return auto_update_classes[base_class]
  else:
    auto_update_classes[base_class] = AutoUpdateBase
    return AutoUpdateBase




