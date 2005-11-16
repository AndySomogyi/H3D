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
/// \file HapticShape.h
/// \brief Header file for HapticShape
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __HAPTICSHAPE_H__
#define __HAPTICSHAPE_H__

#include "HapticObject.h"
#include "RefCountedClass.h"
#include "Types/H3DTypeOperators.h"

//#include "Surface.h"
//#include "X3DGeometryNode.h"

namespace H3D {

  class H3DSurfaceNode;
  class X3DGeometryNode;

  /// Base class for haptic shapes, i.e. geometrical objects that are rendered
  /// by letting their surfaces constrain the proxy. A HapticShape has a Surface
  /// object associated to it that defines the properties of the surface, e.g.
  /// stiffness and friction properties.
  ///
  class H3DAPI_API HapticShape: public HapticObject, public RefCountedClass {
  public:
    /// Constructor.
    HapticShape( X3DGeometryNode *_geometry,
                 H3DSurfaceNode *_surface,
                 const H3D::ArithmeticTypes::Matrix4f & _transform ):
      HapticObject( _transform ),
      geometry( _geometry ),
      surface( _surface ) {}
    
    /// The X3DGeometryNode instance that created this object.
    X3DGeometryNode *geometry;

    /// The Surface object describing the properties of the surface of the 
    /// HapticShape.
    H3DSurfaceNode *surface;
      
  };
}

#endif