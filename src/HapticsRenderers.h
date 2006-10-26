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
/// \file HapticsRenderers.h
/// \brief Header file for several subclasses of H3DHapticsRendererNode
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __HAPTICSRENDERERS_H__
#define __HAPTICSRENDERERS_H__

// H3DApi includes
#include <H3DHapticsRendererNode.h>
#include <SFFloat.h>
#include <SFString.h>
#include <SFBool.h>
#include <FieldTemplates.h>

// HAPI includes
#include <OpenHapticsRenderer.h>
#include <RuspiniRenderer.h>
#include <GodObjectRenderer.h>

namespace H3D {

  /// \ingroup H3DNodes
  /// \brief Haptics renderer using OpenHaptics to render objects
  class H3DAPI_API OpenHapticsRenderer: public H3DHapticsRendererNode {
  public:
    /// ShapeType specializes SFString to set the appropriate default
    /// OpenHaptics shape in the HAPI::OpenHapticsRenderer
    class ShapeType: public OnValueChangeSField< SFString > {
      virtual void onValueChange( const string &v );
    };

    /// AdaptiveViewport specializes SFBool to set the appropriate default
    /// values in the HAPI::OpenHapticsRenderer used.
    class AdaptiveViewport: public OnValueChangeSField< SFBool > {
      virtual void onValueChange( const bool &v );
    };

    
    /// CameraView specializes SFBool to set the appropriate default
    /// values in the HAPI::OpenHapticsRenderer used.
    class CameraView: public OnValueChangeSField< SFBool > {
      virtual void onValueChange( const bool &v );
    };

    /// Constructor.
    OpenHapticsRenderer( Inst< ShapeType     > _defaultShapeType = 0,
                         Inst< AdaptiveViewport >  _defaultAdaptiveViewport = 0,
                         Inst< CameraView  >  _defaultHapticCameraView = 0 ) :
      defaultShapeType( _defaultShapeType ),
      defaultAdaptiveViewport( _defaultAdaptiveViewport ),
      defaultHapticCameraView( _defaultHapticCameraView ) {
      renderer = new HAPI::OpenHapticsRenderer;
      type_name = "OpenHapticsRenderer";
      database.initFields( this );

      defaultShapeType->setValue( "FEEDBACK_BUFFER" );
      defaultAdaptiveViewport->setValue( true );
      defaultHapticCameraView->setValue( true );
    }

    /// Get the HAPI haptics renderer used for this node.
    inline virtual HAPI::OpenHapticsRenderer *getHapticsRenderer() {
      return static_cast< HAPI::OpenHapticsRenderer * >( renderer );
    }

    /// The default shape type to use if it has not been specified with
    /// the OpenHapticsOptions node. 
    /// 
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> "FEEDBACK_BUFFER" \n
    /// <b>Valid values: </b> "FEEDBACK_BUFFER" and "DEPTH_BUFFER"
    auto_ptr< SFString > defaultShapeType;

    /// The default value for use of adaptive viewport to use if it has not
    /// been specified with the OpenHapticsOptions node. 
    /// 
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> true \n
    auto_ptr< SFBool > defaultAdaptiveViewport;

    /// The default value for use of haptic camera view to use if it has not
    /// been specified with the OpenHapticsOptions node. 
    /// 
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> true \n
    auto_ptr< SFBool > defaultHapticCameraView;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };

  /// \ingroup H3DNodes
  /// \brief Haptics renderer using the god object algorithm to render
  /// objects.
  /// Uses the HAPI::GodObjectRenderer class.
  class H3DAPI_API GodObjectRenderer: public H3DHapticsRendererNode {
  public:

    /// Constructor.
    GodObjectRenderer() {
      renderer = new HAPI::GodObjectRenderer;
      type_name = "GodObjectRenderer";
      database.initFields( this );
    }

    /// Get the HAPI haptics renderer used for this node.
    inline virtual HAPI::GodObjectRenderer *getHapticsRenderer() {
      return static_cast< HAPI::GodObjectRenderer * >( renderer );
    }

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };

  /// \ingroup H3DNodes
  /// \brief Haptics renderer using a variant of the Ruspini algorithm
  /// allowing you to specify a radius on the proxy.
  /// Uses the HAPI::RuspiniRenderer class.
  class H3DAPI_API RuspiniRenderer: public H3DHapticsRendererNode {
  public:
    /// ProxyRadius specializes SFFloat to set the proxy radius
    /// in the HAPI::RuspiniRenderer used.
    class ProxyRadius: public OnValueChangeSField< SFFloat > {
      virtual void onValueChange( const H3DFloat &v );
    };

    /// Constructor.
    RuspiniRenderer( Inst< ProxyRadius > _proxyRadius    = 0);

    /// Get the HAPI haptics renderer used for this node.
    inline virtual HAPI::RuspiniRenderer *getHapticsRenderer() {
      return static_cast< HAPI::RuspiniRenderer * >( renderer );
    }

    /// The radius of the proxy in metres.
    /// 
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> 0.0025 \n
    auto_ptr< SFFloat > proxyRadius;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
