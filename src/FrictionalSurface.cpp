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
/// \file FrictionalSurface.cpp
/// \brief cpp file for FrictionalSurface
///
//
//////////////////////////////////////////////////////////////////////////////
#include "H3DApi.h"
#ifdef USE_HAPTICS
#include "FrictionalSurface.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase FrictionalSurface::database( 
                                            "FrictionalSurface", 
                                            &(newInstance<FrictionalSurface>),
                                            typeid( FrictionalSurface ),
                                            &SmoothSurface::database
                                            );

namespace FritionalSurfaceInternals {
  FIELDDB_ELEMENT( FrictionalSurface, staticFriction, INPUT_OUTPUT );
  FIELDDB_ELEMENT( FrictionalSurface, dynamicFriction, INPUT_OUTPUT );
}
  
void FrictionalSurface::hlRender() {
#ifdef HAVE_OPENHAPTICS
  SmoothSurface::hlRender();
  hlMaterialf(HL_FRONT_AND_BACK, 
              HL_DYNAMIC_FRICTION, 
              dynamicFriction->getValue() );
  hlMaterialf(HL_FRONT_AND_BACK, 
              HL_STATIC_FRICTION, 
              staticFriction->getValue() );
#endif
}

void FrictionalSurface::chai3dMaterial( cMaterial &m ) {
  m.setStaticFriction( staticFriction->getValue() );
  m.setDynamicFriction( dynamicFriction->getValue() );
}

FrictionalSurface::FrictionalSurface( Inst< SFFloat >  _stiffness,
                                      Inst< SFFloat >  _damping,
                                      Inst< SFFloat >  _staticFriction,
                                      Inst< SFFloat >  _dynamicFriction ):
  SmoothSurface( _stiffness, _damping ),
  staticFriction( _staticFriction ),
  dynamicFriction( _dynamicFriction ),
  in_static_contact( true ) {
  type_name = "FrictionalSurface";
  database.initFields( this );
  
  staticFriction->setValue( 0.1f );
  dynamicFriction->setValue( 0.4f );
}

void FrictionalSurface::onContact( ContactInfo &contact ) {
  Vec3d local_probe = contact.localProbePosition();
  Vec3d force = local_probe * -stiffness->getValue();
  Vec2d force_t( force.x, force.z );

  if( in_static_contact ) {
   
    if( force_t.length() <= staticFriction->getValue() *  force.y ) {
      contact.setLocalForce( force );
      contact.proxy_movement_local = Vec2d( 0, 0 );
    } else {
      in_static_contact = false;
    }
  } 

  if( !in_static_contact ) {
    H3DDouble b = 1;
    H3DDouble dt = 1e-3;
    H3DDouble velocity = 
      ( force_t.length() - dynamicFriction->getValue() * force.y ) / b;

    contact.setLocalForce( force );
    if( velocity < Constants::f_epsilon ) {
      in_static_contact = true;
      velocity = 0;
      contact.proxy_movement_local = Vec2d( 0, 0 ); 
    } else {
      H3DDouble max_movement = velocity * 1e-3;
      Vec2d proxy_movement = Vec2d( local_probe.x, local_probe.z );
      H3DDouble l = proxy_movement.length();
      if( l > max_movement ) {
        proxy_movement *= max_movement / l; 
      }
      contact.proxy_movement_local = proxy_movement;
    }
  }
}

#endif

