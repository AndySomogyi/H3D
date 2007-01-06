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
/// \file SmoothSurface.cpp
/// \brief cpp file for SmoothSurface
///
//
//////////////////////////////////////////////////////////////////////////////
#include "H3DApi.h"
#ifdef USE_HAPTICS
#include "SmoothSurface.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase SmoothSurface::database( 
                                        "SmoothSurface", 
                                        &(newInstance<SmoothSurface>), 
                                        typeid( SmoothSurface ),
                                        &H3DSurfaceNode::database );

namespace SmoothSurfaceInternals {
  FIELDDB_ELEMENT( SmoothSurface, stiffness, INPUT_OUTPUT );
  FIELDDB_ELEMENT( SmoothSurface, damping, INPUT_OUTPUT );
}


/// Constructor.
SmoothSurface::SmoothSurface( Inst< SFFloat >  _stiffness,
                              Inst< SFFloat >  _damping ):
  stiffness( _stiffness ),
  damping( _damping ) {
  type_name = "SmoothSurface";
  database.initFields( this );
  
  stiffness->setValue( 0.5 );
  damping->setValue( 0 );
}

void SmoothSurface::onContact( ContactInfo &contact ) {
  Vec3d local_probe = contact.localProbePosition();
  // TODO: fix realtime field access
  //contact.setLocalForce( Vec3d( 0, -local_probe.y/1000 * stiffness->getValue() * 700, 0 ) );
  contact.setLocalForce( -local_probe/1000 * stiffness->getValue() * 700 );
  contact.proxy_movement_local = Vec2d( local_probe.x, local_probe.z );
}

#endif

