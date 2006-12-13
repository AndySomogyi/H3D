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
/// \file GeometryBoundTreeOptions.h
/// \brief Header file for GeometryBoundTreeOptions.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __GEOMETRYBOUNDTREEOPTIONS_H__
#define __GEOMETRYBOUNDTREEOPTIONS_H__

#include <H3DOptionNode.h>
#include <SFString.h>
#include <SFInt32.h>

namespace H3D {

  /// \ingroup H3DNodes
  /// \class GeometryBoundTreeOptions
  /// \brief Specifies parameters to used for the tree of bounding primitives
  /// that are build for each X3DGeometryNode for collision detection purposes.
  class H3DAPI_API GeometryBoundTreeOptions : public H3DOptionNode {
  public:
    
    /// Constructor.
    GeometryBoundTreeOptions( Inst< SFNode   > _metadata = 0,
                              Inst< SFString > _boundType = 0,
                              Inst< SFInt32  > _maxTrianglesInLeaf  = 0 );
    
    /// The boundType field specifies what type of bound primitives to
    /// use in the bounding tree. 
    /// Valid values are: "SPHERE", "AABB"(axis aligned bounding box),
    /// and "OBB" (oriented bounding box)
    /// 
    ///
    /// <b>Default value: </b> "AABB" \n
    /// <b>Access type: </b> inputOutput \n
    /// <b>Valid values: </b> "SPHERE", "AABB", "OBB" \n
    auto_ptr< SFString > boundType;

    /// Specifies the maximum number of triangles that can be put in
    /// a leaf of the tree. A negative value means that the tree will
    /// be just a leaf with all triangles.
    ///
    /// <b>Default value: </b> 1 \n
    /// <b>Access type: </b> inputOutput \n
    auto_ptr< SFInt32 > maxTrianglesInLeaf;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  protected:
    
  };
}

#endif