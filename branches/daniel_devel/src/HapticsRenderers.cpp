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
/// \file Material.cpp
/// \brief CPP file for Material, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "HapticsRenderers.h"

using namespace H3D;

H3DNodeDatabase OpenHapticsRenderer::database( 
                           "OpenHapticsRenderer", 
                           &(newInstance<OpenHapticsRenderer>), 
                           typeid( OpenHapticsRenderer ),
                           &H3DHapticsRendererNode::database );

H3DNodeDatabase GodObjectRenderer::database( 
                           "GodObjectRenderer", 
                           &(newInstance<GodObjectRenderer>), 
                           typeid( GodObjectRenderer ),
                           &H3DHapticsRendererNode::database );

H3DNodeDatabase RuspiniRenderer::database( 
                           "RuspiniRenderer", 
                           &(newInstance<RuspiniRenderer>), 
                           typeid( RuspiniRenderer ),
                           &H3DHapticsRendererNode::database );

namespace HapticsRendererInternals {
  FIELDDB_ELEMENT( RuspiniRenderer, proxyRadius, INPUT_OUTPUT );

  FIELDDB_ELEMENT( OpenHapticsRenderer, defaultShapeType, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsRenderer, defaultAdaptiveViewport, INPUT_OUTPUT );
  FIELDDB_ELEMENT( OpenHapticsRenderer, defaultHapticCameraView, INPUT_OUTPUT );
}

RuspiniRenderer::RuspiniRenderer( Inst< ProxyRadius > _proxyRadius ) :
  proxyRadius( _proxyRadius ) {
  renderer = new HAPI::RuspiniRenderer;
  
  type_name = "RuspiniRenderer";
  database.initFields( this );
  
  proxyRadius->setValue( (H3DFloat) 0.0025 );
}
  
void RuspiniRenderer::ProxyRadius::onValueChange( const H3DFloat &v ) {
  RuspiniRenderer *ruspini_node = 
    static_cast< RuspiniRenderer * >( getOwner() );
  HAPI::RuspiniRenderer *r = ruspini_node->getHapticsRenderer();
  // convert to millimeters
  r->setProxyRadius( v * 1000 );
}

void OpenHapticsRenderer::ShapeType::onValueChange( const string &v ) {
  OpenHapticsRenderer *oh_node = 
    static_cast< OpenHapticsRenderer * >( getOwner() );
  HAPI::OpenHapticsRenderer *r = oh_node->getHapticsRenderer();
  
  if( v == "FEEDBACK_BUFFER" ) {
    r->setDefaultShapeType( HAPI::OpenHapticsRenderer::OpenHapticsOptions::FEEDBACK_BUFFER );
  } else if( v == "DEPTH_BUFFER" ) {
    r->setDefaultShapeType( HAPI::OpenHapticsRenderer::OpenHapticsOptions::DEPTH_BUFFER );    
  } else {
    Console(4) << "Warning: Invalid OpenHaptics shape type: "
               << v 
               << ". Must be \"FEEDBACK_BUFFER\" or \"DEPTH_BUFFER\" "
               << "(in \"" << getName() << "\")" << endl;
    r->setDefaultShapeType( HAPI::OpenHapticsRenderer::OpenHapticsOptions::FEEDBACK_BUFFER );
  }
}

void OpenHapticsRenderer::AdaptiveViewport::onValueChange( const bool &v ) {
  OpenHapticsRenderer *oh_node = 
    static_cast< OpenHapticsRenderer * >( getOwner() );
  HAPI::OpenHapticsRenderer *r = oh_node->getHapticsRenderer();
  r->setDefaultAdaptiveViewport( v );
}

void OpenHapticsRenderer::CameraView::onValueChange( const bool &v ) {
  OpenHapticsRenderer *oh_node = 
    static_cast< OpenHapticsRenderer * >( getOwner() );
  HAPI::OpenHapticsRenderer *r = oh_node->getHapticsRenderer();
  r->setDefaultHapticCameraView( v );
}
