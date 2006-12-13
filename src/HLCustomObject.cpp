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
/// \file HLCustomObject.h
/// \brief cpp file for HLCustomObject
///
//
//////////////////////////////////////////////////////////////////////////////
#include "H3DApi.h"
#ifdef USE_HAPTICS
#include "HLCustomObject.h"

#include <HAPIHapticShape.h>

using namespace H3D;

void HLCustomObject::hlRender( HAPI::HAPIHapticsDevice *hd,
                               HLuint hl_shape_id ) {
#ifdef HAVE_OPENHAPTICS
  HAPI::HAPIHapticShape *hs = dynamic_cast< HAPI::HAPIHapticShape * >( this );
/*  if( hs && !closeEnoughToBound( hd->proxyPosition->getValue(), 
                                 hd->getPreviousProxyPosition(),
                                 (Matrix4f)hs->transform.inverse(), 
                                 (X3DGeometryNode * )hs->userdata ) ) {
    return;
  }*/
  
  hlBeginShape(HL_SHAPE_CALLBACK, hl_shape_id );
  hlCallback(HL_SHAPE_INTERSECT_LS, 
             (HLcallbackProc) intersectCallback, this);
  hlCallback(HL_SHAPE_CLOSEST_FEATURES, 
             (HLcallbackProc) closestFeaturesCallback, this);
  hlEndShape();
#endif
};
#endif