//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file HapticSpring.h
/// \brief Header file for HapticSpring.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __HAPTICSPRING_H__
#define __HAPTICSPRING_H__

#include "HapticForceEffect.h" 

namespace H3D {

  /// Generates a spring force, 
  /// i.e. force = ( position - device_position ) * spring_constant.
  /// 
  class H3DAPI_API HapticSpring: public HapticForceEffect {
  public:
    /// Constructor
    HapticSpring ( const H3D::ArithmeticTypes::Matrix4f & _transform,
                   bool _interpolate ):
      HapticForceEffect( _transform, _interpolate ),
      position( Vec3f( 0, 0, 0 ) ),
      velocity( Vec3f( 0, 0, 0 ) ),
    spring_constant( 0 ) { }
    
    /// Constructor
    HapticSpring( const H3D::ArithmeticTypes::Matrix4f & _transform,
                  const Vec3f &_position,
                  const Vec3f &_velocity,
                  H3DFloat _spring_constant,
                  bool _interpolate ):
      HapticForceEffect( _transform, _interpolate ),
      position( _position ),
      velocity( _velocity ),
      spring_constant( _spring_constant ) {}
    
    /// The force of the EffectOutput will be the force of the force field. 
    EffectOutput virtual calculateForces( const EffectInput &input ) {
      //position = position + velocity*input.deltaT;
      Vec3f f = ( position - input.position ) * spring_constant;
      force += (double)((TimeStamp)input.deltaT) * f;
      //cerr << "input.deltaT " << input.deltaT << endl;
      return EffectOutput( f );
    }

    // set position
    virtual void setPosition( const Vec3f &_position ) { 
      position = _position;
    }

    // set velocity
    virtual void setVelocity( const Vec3f &_velocity ) {
      velocity = _velocity;
    }

    // set velocity
    virtual void setSpringConstant( const H3DFloat &_sc ) { spring_constant = _sc; }
    
    // get and reset force
    virtual Vec3f getAndResetForce() {
      //Vec3f local_pos = transform.inverse() * input.position;
      // force in local coordinate space
      //Vec3f local_force = ( position - local_pos ) * spring_constant;
      // transform force into global coordinate space before returning.
      //return EffectOutput( transform.getRotationPart() * local_force );
      Vec3f f = force;
      force = Vec3f( 0, 0, 0 );
      return f;
    }
    
  protected:
    Vec3f position;
    Vec3f velocity;
    Vec3f force;
    H3DFloat spring_constant;
  };
}

#endif
