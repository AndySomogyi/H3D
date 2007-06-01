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
/// \file MagneticSurface.h
/// \brief Header file for MagneticSurface
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __MAGNETICSURFACE_H__
#define __MAGNETICSURFACE_H__

#include <FrictionalSurface.h>
#include <OpenHapticsRenderer.h>

namespace H3D {

  /// \ingroup H3DNodes
  /// \class MagneticSurface
  /// The MagneticSurface is a surface where the proxy is attracted to
  /// the surface, and forces are generated in order to keep the proxy 
  /// on the surface.
  ///
  /// The snapDistance field defines a distance in meters from the surface
  /// within which forces are generated to pull the proxy towards the
  /// surface. If the device is pulled outside this distance from the
  /// surface it will be freed from the magnetic attraction.
  class H3DAPI_API MagneticSurface:  public FrictionalSurface {
  public:

    /// Constructor.
    MagneticSurface( Inst< UpdateStiffness > _stiffness       = 0,
                     Inst< UpdateDamping > _damping         = 0,
                     Inst< UpdateStaticFriction > _staticFriction  = 0,
                     Inst< UpdateDynamicFriction > _dynamicFriction = 0,
                     Inst< SFFloat > _snapDistance    = 0 );

    void initialize();

#ifdef HAVE_OPENHAPTICS
    // temporary class until HAPI has a MagneticSurface
    class H3DAPI_API FrictionalHLSurface: public HAPI::HAPISurfaceObject,
      public HAPI::OpenHapticsRenderer::HLSurface
    {
    public:
      FrictionalHLSurface( H3DDouble _stiffness,
                           H3DDouble _damping,
                           H3DDouble _static_friction,
                           H3DDouble _dynamic_friction,
                           H3DDouble _magnetic,
                           H3DDouble _snap_distance ):
        stiffness( _stiffness ),
        damping( _damping ),
        static_friction( _static_friction ),
        dynamic_friction( _dynamic_friction ),
        magnetic( _magnetic ),
        snap_distance( _snap_distance )
      {
      }
      /// Renders the surface using hlMaterialf calls
      virtual void hlRender();

    protected:
      H3DDouble stiffness, damping, static_friction,
                dynamic_friction, magnetic, snap_distance;
    };
#endif

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
