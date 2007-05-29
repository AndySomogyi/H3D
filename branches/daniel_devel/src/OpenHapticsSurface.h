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
/// \file OpenHapticsSurface.h
/// \brief Header file for OpenHapticsSurface
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __OPENHAPTICSSURFACE_H__
#define __OPENHAPTICSSURFACE_H__

#include <H3DSurfaceNode.h>
#include <OpenHapticsRenderer.h>
#include <SFFloat.h>
#include <SFBool.h>

#ifdef HAVE_OPENHAPTICS

namespace H3D {

  /// \ingroup H3DNodes
  /// \class OpenHapticsSurface
  /// \brief Surface type for setting OpenHaptics surface parameters directly.
  /// Only works when using the OpenHapticsRenderer.
  class H3DAPI_API OpenHapticsSurface: public H3DSurfaceNode, 
                                       public HAPI::OpenHapticsRenderer::HLSurface {
  public:

    /// Constructor.
    OpenHapticsSurface( Inst< SFFloat > _stiffness = 0,
                        Inst< SFFloat > _damping   = 0,
                        Inst< SFFloat > _staticFriction  = 0,
                        Inst< SFFloat > _dynamicFriction = 0,
                        Inst< SFBool  > _magnetic  = 0,
                        Inst< SFFloat > _snapDistance = 0 );
  
    /// Renders the surface using hlMaterialf calls
    virtual void hlRender();

    virtual void onContact( ContactInfo &contact );

    /// The stiffness of the surface. Should be a value between 0 and 1
    /// where 1 is the maximum stiffness the haptics device can handle.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> 0.5 \n
    /// <b>Value range: </b> [0-1]
    auto_ptr< SFFloat > stiffness;

    /// The velocity based damping of the surface. Should be a value between
    /// 0 and 1 where 1 is the maximum damping the haptics device can handle.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> 0 \n
    /// <b>Value range: </b> [0-1]
    auto_ptr< SFFloat > damping;

    /// The friction that is experienced upon initial movement when resting on 
    /// the surface.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> 0.1 \n
    /// <b>Value range: </b> [0-1]
    auto_ptr< SFFloat > staticFriction;

    /// The friction that is experienced when moving along the surface 
    /// the surface.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> 0.4 \n
    /// <b>Value range: </b> [0-1]
    auto_ptr< SFFloat > dynamicFriction;

    /// Specifies if the surface should be magnetic or not.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> false \n
    auto_ptr< SFBool > magnetic;

    /// The distance from the surface within which forces are generated
    /// to pull the proxy towards the surface. If the device is pulled
    /// outside this distance from the surface it will be freed from the
    /// magnetic attraction.
    ///
    /// <b>Access type: </b> inputOutput \n
    /// <b>Default value: </b> 0.01 \n
    auto_ptr< SFFloat > snapDistance;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
#endif
