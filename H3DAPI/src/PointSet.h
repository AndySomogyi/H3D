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
/// \file PointSet.h
/// \brief Header file for PointSet, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __POINTSET_H__
#define __POINTSET_H__

#include "X3DGeometryNode.h"
#include "DependentNodeFields.h"
#include "X3DCoordinateNode.h"
#include "X3DColorNode.h"
#include "CoordBoundField.h"

namespace H3D {

  /// \ingroup Geometries
  /// \class PointSet
  /// \brief The PointSet node specifies a set of 3D points, in the local
  /// coordinate system, with associated colours at each point.
  ///
  /// The coord field specifies a X3DCoordinateNode. PointSet uses the
  /// coordinates in order. If the coord field is NULL, the point set 
  /// is considered empty.
  ///
  /// PointSet nodes are not lit, not texture-mapped, nor do they
  /// participate in collision detection. The size of each point is
  /// implementation-dependent. 
  ///
  /// If the color field is not NULL, it shall specify a Color node that
  /// contains at least the number of points contained in the coord
  /// node. Colours  are applied to each point in order. The
  /// results are undefined if the number of values in the Color node is
  /// less than the number of values specified in the Coordinate node. 
  ///
  /// If the color field is NULL and there is a Material node defined for the
  /// Appearance node affecting this PointSet node, the emissiveColor of the
  /// Material node shall be used to draw the points. 
  ///
  /// \par Internal routes:
  /// \dotfile PointSet.dot
  class H3DAPI_API PointSet : public X3DGeometryNode {
  public:

    /// Thrown when the number of colors in the color field are less
    /// than the number of points in the coord field.
    H3D_VALUE_EXCEPTION( unsigned int, NotEnoughColors );
    
    /// SFCoordinateNode is dependent on coordChanged field in 
    /// X3DCoordinateNode.
    typedef DependentSFNode< X3DCoordinateNode,
                             FieldRef< X3DGeometricPropertyNode,
                                       Field,
                                       &X3DCoordinateNode::propertyChanged > > 
    SFCoordinateNode;

    /// SFColorNode is dependent on colorChanged field in 
    /// X3DColorNode.
    typedef DependentSFNode< X3DColorNode,
                             FieldRef< X3DGeometricPropertyNode,
                                       Field,
                                       &X3DColorNode::propertyChanged > > 
    SFColorNode;

    /// The bound field for PointSet is a CoordBoundField.
    typedef CoordBoundField SFBound;

    /// Constructor.
    PointSet( Inst< SFNode           > _metadata       = 0,
              Inst< SFBound          > _bound          = 0,
              Inst< DisplayList      > _displayList    = 0,
              Inst< SFColorNode      > _color          = 0,
              Inst< SFCoordinateNode > _coord          = 0 );

    /// Traverse the scenegraph. A HLFeedbackShape is added for haptic
    /// rendering if haptics is enabled.
    virtual void traverseSG( TraverseInfo &ti ); 
    
    /// Render the LineSet with OpenGL
    virtual void render();
    
    /// If the color field is not NULL, it shall specify a Color node that
    /// contains at least the number of points contained in the coord
    /// node. Colours  are applied to each point in order.   
    /// If the color field is NULL and there is a Material node defined for the
    /// Appearance node affecting this PointSet node, the emissiveColor of the
    /// Material node shall be used to draw the points. 
    ///
    /// <b>Access type:</b> inputOutput 
    ///
    /// \dotfile PointSet_color.dot 
    auto_ptr< SFColorNode >  color;

    /// The coord field specifies a X3DCoordinateNode. PointSet uses the
    /// coordinates in order. If the coord field is NULL, the point set 
    /// is considered empty.
    ///
    /// <b>Access type:</b> inputOutput 
    ///
    /// \dotfile PointSet_coord.dot 
    auto_ptr< SFCoordinateNode >  coord;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif