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
/// \file OpenHapticsSurface.cpp
/// \brief cpp file for OpenHapticsSurface
///
//
//////////////////////////////////////////////////////////////////////////////

#include "OpenHapticsSurface.h"

#ifdef HAVE_OPENHAPTICS

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase OpenHapticsSurface::database( 
                                        "OpenHapticsSurface", 
                                        &(newInstance<OpenHapticsSurface>), 
                                        typeid( OpenHapticsSurface ),
                                        &H3DSurfaceNode::database );

namespace OpenHapticsSurfaceInternals {
  FIELDDB_ELEMENT( OpenHapticsSurface, stiffness, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsSurface, damping, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsSurface, staticFriction, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsSurface, dynamicFriction, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsSurface, snapDistance, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsSurface, magnetic, INPUT_OUTPUT );
}


void OpenHapticsSurface::hlRender() {
#ifdef HAVE_OPENHAPTICS
  hlTouchModel( HL_CONTACT );
  hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, stiffness->getValue() );
  hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, damping->getValue() );
  hlMaterialf(HL_FRONT_AND_BACK, 
              HL_DYNAMIC_FRICTION, 
              dynamicFriction->getValue() );
  hlMaterialf(HL_FRONT_AND_BACK, 
              HL_STATIC_FRICTION, 
              staticFriction->getValue() );
  if( magnetic->getValue() ) hlTouchModel( HL_CONSTRAINT );
  else hlTouchModel( HL_CONTACT );
  // convert to millimeters
  hlTouchModelf( HL_SNAP_DISTANCE, 1000 * snapDistance->getValue() );

#endif
}

/// Constructor.
OpenHapticsSurface::OpenHapticsSurface( Inst< SFFloat > _stiffness,
                                        Inst< SFFloat > _damping,
                                        Inst< SFFloat > _staticFriction ,
                                        Inst< SFFloat > _dynamicFriction,
                                        Inst< SFBool  > _magnetic ,
                                        Inst< SFFloat > _snapDistance ) :
  stiffness( _stiffness ),
  damping( _damping ),
  staticFriction( _staticFriction ),
  dynamicFriction( _dynamicFriction ),
  magnetic( _magnetic ),
  snapDistance( _snapDistance ) {
  type_name = "OpenHapticsSurface";
  database.initFields( this );
  
  stiffness->setValue( (H3DFloat)0.5 );
  damping->setValue( 0 );
  staticFriction->setValue( (H3DFloat)0.1 );
  dynamicFriction->setValue( (H3DFloat)0.4 );
  magnetic->setValue( false );
  snapDistance->setValue( (H3DFloat) 0.01 );
}

void OpenHapticsSurface::onContact( ContactInfo &contact ) {
  //contact.setLocalForce( -local_probe/1000 * stiffness->getValue() * 700 );
  //contact.proxy_movement_local = Vec2d( local_probe.x, local_probe.z );
}

#endif
