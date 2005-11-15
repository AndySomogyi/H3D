
from H3D import *
from H3DInterface import *

class LinSpring( SFVec3f ):
  def __init__( self, konstant ):
    SFVec3f.__init__( self )
    self.konstant=konstant
  def update( self, event ):
    dist = event.getValue()
    return -dist * self.konstant
linSpring = LinSpring( 100 )

class LinDamper( SFVec3f ):
  def __init__( self, konstant ):
    SFVec3f.__init__( self )
    self.konstant=konstant
  def update( self, event ):
    vel = event.getValue()
    return -vel * self.konstant

linDamper = LinDamper( 10 )

class SumForces( SFVec3f ):
  def update( self, event ):
    f = Vec3f( 0, 0, 0 );
    routes = getRoutesIn( self )
    for r in routes:
      f = f + r.getValue();
    return f

class InvertForce( TypedField( SFVec3f, MFVec3f ) ):
  def update( self, event ):
    try:
      return event.getValue()[0] * -1;
    except:
      return Vec3f(0,0,0)

sumForces = SumForces()
invertForce = InvertForce()
linSpring.route( sumForces )
linDamper.route( sumForces )
invertForce.route( sumForces )

class AngSpring( TypedField( SFVec3f, SFRotation ) ):
  def __init__( self, konstant ):
    SFVec3f.__init__( self )
    self.konstant = konstant
  def update( self, event ):
    orn = -event.getValue()
    diff = Rotation(1,0,0,0) * orn
    p = Vec3f( diff.x, diff.y, diff.z ) * diff.a * self.konstant
    return p
angSpring = AngSpring( 0.1 ) #1

#
# RotDamper dampens the angular motion of the dynamic. We route from
# the dynamic's angularVelocity field to the dynamic's torque.
#
class AngDamper( SFVec3f ):
  def __init__( self, konstant ):
    SFVec3f.__init__( self )
    self.konstant = konstant
  def update( self, event ):
    angVel = event.getValue()
    return -angVel*self.konstant
angDamper = AngDamper( 0.1 )

class ApplyTorque( TypedField( SFVec3f, ( SFMatrix4f, MFVec3f, MFVec3f ) ) ):
  def update( self, event ):
    try:

      matrix, force, points = self.getRoutesIn()
      mypoints = points.getValue()
      to_global = matrix.getValue()

      fulcrum = to_global * Vec3f( 0, 0, 0 )
      touch_point = to_global * mypoints[0]
      torque = (fulcrum - touch_point) % ( to_global.getRotationPart() *force.getValue()[0] )
      return torque

    except:
      return Vec3f( 0, 0, 0 )

class SumTorques( SFVec3f ):
  def update( self, event ):
    f = Vec3f( 0, 0, 0 );
    routes_in = getRoutesIn( self )
    
    for r in routes_in:
      f = f + r.getValue();
#    if( abs( f.y ) < 0.0001 ): f.y = 0
    return f

sumTorques = SumTorques()
applyTorque = ApplyTorque()
angSpring.route( sumTorques )
angDamper.route( sumTorques )
applyTorque.route( sumTorques )


