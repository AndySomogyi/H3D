//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
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

#include <OpenHapticsSurface.h>

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

void OpenHapticsSurface::initialize() {
  hapi_surface.reset(
    new FrictionalHLSurface( stiffness->getValue(),
                             damping->getValue(),
                             staticFriction->getValue(),
                             dynamicFriction->getValue(),
                             magnetic->getValue(),
                             snapDistance->getValue() * 1000 ) );
}

void OpenHapticsSurface::FrictionalHLSurface::hlRender() {
  HAPI::OpenHapticsRenderer::hlRenderRelative( 
                                  stiffness,
                                  damping,
                                  static_friction,
                                  dynamic_friction, 
                                  magnetic,
                                  snap_distance );
}

#endif
