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
/// \file SmoothSurface.cpp
/// \brief cpp file for SmoothSurface
///
//
//////////////////////////////////////////////////////////////////////////////
#include <H3DApi.h>
#include <SmoothSurface.h>
#include <FrictionSurface.h>

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
SmoothSurface::SmoothSurface( Inst< UpdateStiffness >  _stiffness,
                              Inst< UpdateDamping >  _damping ):
  stiffness( _stiffness ),
  damping( _damping ),
  conversion_to_HAPI( 0.7 ) {
  type_name = "SmoothSurface";
  database.initFields( this );
  
  stiffness->setValue( 0.5 );
  damping->setValue( 0 );
}

void SmoothSurface::initialize() {
  hapi_surface.reset(
    new HAPI::FrictionSurface( stiffness->getValue() * conversion_to_HAPI,
                               damping->getValue() * conversion_to_HAPI ) );
}

void SmoothSurface::UpdateStiffness::setValue( const H3DFloat &f, int id ){
  SFFloat::setValue( f, id );
  SmoothSurface *ss = 
    static_cast< SmoothSurface * >( getOwner() );
  if( ss->hapi_surface.get() ) {
    static_cast< HAPI::FrictionSurface * >( ss->hapi_surface.get() )
      ->stiffness = f * ss->conversion_to_HAPI;
  }
}

void SmoothSurface::UpdateStiffness::update() {
  SFFloat::update();
  SmoothSurface *ss = 
    static_cast< SmoothSurface * >( getOwner() );
  if( ss->hapi_surface.get() ) {
    static_cast< HAPI::FrictionSurface * >( ss->hapi_surface.get() )
      ->stiffness = value * ss->conversion_to_HAPI;
  }
}

void SmoothSurface::UpdateDamping::setValue( const H3DFloat &f, int id ){
  SFFloat::setValue( f, id );
  SmoothSurface *ss = 
    static_cast< SmoothSurface * >( getOwner() );
  if( ss->hapi_surface.get() ) {
    static_cast< HAPI::FrictionSurface * >( ss->hapi_surface.get() )
      ->damping = f * ss->conversion_to_HAPI;
  }
}

void SmoothSurface::UpdateDamping::update() {
  SFFloat::update();
  SmoothSurface *ss = 
    static_cast< SmoothSurface * >( getOwner() );
  if( ss->hapi_surface.get() ) {
    static_cast< HAPI::FrictionSurface * >( ss->hapi_surface.get() )
      ->damping = value * ss->conversion_to_HAPI;
  }
}