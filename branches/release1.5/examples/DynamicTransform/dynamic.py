from H3DInterface import *

# The LinSpring class generates a spring force that tries to keep the
# nodes in the origin.
#
# routes_in[0] is the position of the DynamicTransform node.
class LinSpring( SFVec3f ):
  def __init__( self, constant ):
    SFVec3f.__init__( self )
    self.constant=constant
  def update( self, event ):
    dist = event.getValue()
    return -dist * self.constant

linSpring = LinSpring( 100 )

# The LinDamper class generates a force to dampen the linear motion
# of a DynamicTransform.
#
# routes_in[0] is the velocity of the DynamicTransform node.
class LinDamper( SFVec3f ):
  def __init__( self, constant ):
    SFVec3f.__init__( self )
    self.constant=constant
  def update( self, event ):
    vel = event.getValue()
    return -vel * self.constant

linDamper = LinDamper( 10 )

# The SumForces class just sums all SFVec3f fields that are routed to it.
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

# The AngSpring class generates a torque that tries to keep the
# DynamicTransform in the original position..
#
# routes_in[0] is the orientation of the DynamicTransform node.

class AngSpring( TypedField( SFVec3f, SFRotation ) ):
  def __init__( self, constant ):
    SFVec3f.__init__( self )
    self.constant = constant
  def update( self, event ):
    orn = -event.getValue()
    diff = Rotation(1,0,0,0) * orn
    p = Vec3f( diff.x, diff.y, diff.z ) * diff.a * self.constant
    return p
angSpring = AngSpring( 0.1 ) #1

#
# RotDamper dampens the angular motion of the dynamic. We route from
# the dynamic's angularVelocity field to the dynamic's torque.
#
class AngDamper( SFVec3f ):
  def __init__( self, constant ):
    SFVec3f.__init__( self )
    self.constant = constant
  def update( self, event ):
    angVel = event.getValue()
    return -angVel*self.constant
angDamper = AngDamper( 0.1 )

# The ApplyTorque field calulates the torque applied to the Box with the 
# haptics device.
#
# routes_in[0] is the matrix field of the DynamicTransform
# routes_in[1] is the force field of the Box node
# routes_in[2] is the contactPoint field of the box node.

class ApplyTorque( TypedField( SFVec3f, ( SFMatrix4f, MFVec3f, MFVec3f ) ) ):
  def update( self, event ):
    try:
      matrix, force, points = self.getRoutesIn()
      mypoints = points.getValue()
      to_global = matrix.getValue()

      fulcrum = to_global * Vec3f( 0, 0, 0 )
      touch_point = to_global * mypoints[0]
      torque = (fulcrum - touch_point) % ( to_global.getRotationPart() *
                                           force.getValue()[0] )
      return torque
    except:
      return Vec3f( 0, 0, 0 )


# The SumTorques class just sums all SFVec3f fields that are routed to it.
class SumTorques( SFVec3f ):
  def update( self, event ):
    f = Vec3f( 0, 0, 0 );
    routes_in = getRoutesIn( self )
    
    for r in routes_in:
      f = f + r.getValue();
    return f

sumTorques = SumTorques()
applyTorque = ApplyTorque()
angSpring.route( sumTorques )
angDamper.route( sumTorques )
applyTorque.route( sumTorques )


