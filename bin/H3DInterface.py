print "BEGIN: H3DInterface.py"

import H3D


###################################################################
#
# Base types; Vec2f, Vec3f, Vec2d, Vec2d, etc, etc, etc
#
###################################################################

class Vec2f:
  def __init__( self, x=0, y=0 ):
	self.x = float( x )
	self.y = float( y )
  # String cast operator:
  def __repr( self ):
	return `self.x` + ", " + `self.y`


class Vec2d( Vec2f ):
  pass

class Vec3f:
  def __init__( self, x=0, y=0, z=0 ):
	self.x = float( x )
	self.y = float( y )
	self.z = float( z )
  # String cast operator:
  def __repr__( self ):
	print `self.x` + ", " + `self.y` + ", " + `self.z`
	return `self.x` + " " + `self.y` + " " + `self.z`


class Vec3d( Vec3f ):
  pass


class RGB:
  def __init__( self, r=0, g=0, b=0 ):
	self.r = float( r )
	self.g = float( g )
	self.b = float( b )


class RGBA:
  def __init__( self, r=0, g=0, b=0, a=0 ):
	self.r = float( r )
	self.g = float( g )
	self.b = float( b )
	self.a = float( a )


class Rotation:
  def __init__( self, x=1, y=0, z=0, a=0 ):
	self.x = float( x )
	self.y = float( y )
	self.z = float( z )
	self.a = float( a )

class Matrix4f:
  def __init__( self, \
				m = [ [ 1.0, 0.0, 0.0, 0.0 ], \
					  [ 0.0, 1.0, 0.0, 0.0 ], \
					  [ 0.0, 0.0, 1.0, 0.0 ], \
					  [ 0.0, 0.0, 0.0, 1.0 ] ] ):
	print "M4F"

	self.m = \
		[ [ float(m[0][0]), float(m[0][1]), float(m[0][2]), float(m[0][3]) ], 
		  [ float(m[1][0]), float(m[1][1]), float(m[1][2]), float(m[1][3]) ], 
		  [ float(m[2][0]), float(m[2][1]), float(m[2][2]), float(m[2][3]) ], 
		  [ float(m[3][0]), float(m[3][1]), float(m[3][2]), float(m[3][3]) ] ]








###################################################################
#
# Field base class and SF* MF* class definitions
#
###################################################################
class Field:
  type = H3D.UNKNOWN_X3D_TYPE
  def __init__( self ):
    H3D.createField( self )

  def setValue( self, value ):
	print "setValue"
	H3D.setFieldValue( self, value )

  def getValue( self ):
    return H3D.getFieldValue( self )

  def route( self, dest ):
    return H3D.routeField( self, dest )


# Install all built-in Field types:
types = [ 
  ( H3D.SFFLOAT,  "SFFloat" ),
  ( H3D.MFFLOAT,  "MFFloat" ),
  ( H3D.SFDOUBLE, "SFDouble" ),
  ( H3D.MFDOUBLE, "MFDouble" ),
  ( H3D.SFINT32,  "SFInt32" ),
  ( H3D.MFINT32,  "MFInt32" ),
  ( H3D.SFVEC2F,  "SFVec2f" ),
  ( H3D.MFVEC2F,  "MFVec2f" ),
  ( H3D.SFVEC2D,  "SFVec2f" ),
  ( H3D.MFVEC2D,  "MFVec2d" ),
  ( H3D.SFVEC3F,  "SFVec3f" ),
  ( H3D.MFVEC3F,  "MFVec3f" ),
  ( H3D.SFVEC3D,  "SFVec3d" ),
  ( H3D.MFVEC3D,  "MFVec3d" ),
  ( H3D.SFBOOL,   "SFBool"  ),
  ( H3D.MFBOOL,   "MFBool"  ),
  ( H3D.SFSTRING, "SFString" ),
  ( H3D.MFSTRING, "MFString" ),
  ( H3D.SFRGB,    "SFColor" ),
  ( H3D.MFRGB,    "MFColor" ),
  ( H3D.SFRGBA,   "SFColorRGBA" ),
  ( H3D.MFRGBA,   "MFColorRGBA" ),
  ( H3D.SFROTATION, "SFRotation" ),
  ( H3D.MFROTATION, "MFRotation" ),
  ( H3D.SFMATRIX3F, "SFMatrix3f" ),
  ( H3D.MFMATRIX3F, "MFMatrix3f" ),
  ( H3D.SFMATRIX4F, "SFMatrix4f" ),
  ( H3D.MFMATRIX4F, "MFMatrix4f" ) ]

for t in types:
  exec """
class %s( Field ):
  type = %s
""" % (t[1], t[0] )


