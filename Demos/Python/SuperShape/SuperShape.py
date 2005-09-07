import math
from H3D import *
from H3DInterface import *

m=0
class MyFloat( TypedField( SFFloat, SFTime ) ):
  def update( self, event ):
    global m
    m=m+0.02
    return m

class MyVec2f( TypedField( SFVec3f, SFFloat ) ):
  def update(self, event):
    f=event.getValue()
    return Vec3f( math.cos(f),math.sin(f), 0 )

morph=MyFloat()
time.route( morph )

pos=MyVec2f()
morph.route( pos )
