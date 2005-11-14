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
/// \file HapticCylinder.h
/// \brief Header file for HapticCylinder
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __HAPTICCYLINDER_H__
#define __HAPTICCYLINDER_H__

#include "X3DGeometryNode.h"
#include "H3DSurfaceNode.h"
#include "HapticShape.h"
#include "HLCustomObject.h"

namespace H3D {

  /// Class for rendering a haptic cylinder.
  class H3DAPI_API HapticCylinder: public HapticShape, public HLCustomObject  {
  public:
    /// Constructor.
    HapticCylinder( H3DFloat _height,
                    H3DFloat _radius,
                    X3DGeometryNode *_geometry,
                    H3DSurfaceNode *_surface,
                    const Matrix4f &_transform ):
      HapticShape( _geometry, _surface, _transform ),
      height( _height ),
      radius( _radius ) {}
#ifdef HAVE_OPENHAPTICS
    /// Intersect the line segment from start_point to end_point with
    /// the object.  
    /// This is used by the callback functions of the HLCustomObject.
    /// \param start_point The start point of the line segment.
    /// \param end_point The end point of the line segment.
    /// \param intersection_point Return parameter that should be set to the 
    /// point of intersection between the line segment and object.
    /// \param intersection_normal Return parameter that should be set to the 
    /// normal of the surface of the objet at the intersection point.
    /// \param face Return parameter that should be set to HL_FRONT if the front
    /// of the surface is touched and to HL_BACK if the backside of the surface
    /// is touched. 
    /// \returns true if there is an intersection, false otherwise.
    ///
    virtual bool intersectSurface( const Vec3f &start_point, 
                                   const Vec3f &end_point,
                                   Vec3f &intersection_point, 
                                   Vec3f &intersection_normal,
                                   HLenum &face );

    /// Find the closest point to query_point on the surface of the
    /// object. 
    /// This is used by the callback functions of the HLCustomObject.
    /// \param query_point The point to find the closest point to.
    /// \param geom Return parameter to return a hlLocalFeature
    /// \param closest_point Return parameter that should be set to the closest
    /// point to the surface to query_point.
    /// the surface at the closest point.
    /// 
    virtual bool closestFeature( const Vec3f &query_point, 
                                 const Vec3f &target_point,
                                 HLgeom *geom,
                                 Vec3f &closest_point );    

    /// hlRender uses glutCylinder for now to render the cylinder.
    virtual void hlRender( HLHapticsDevice *hd );
#endif
    H3DFloat height, radius;

  };
}

#endif
