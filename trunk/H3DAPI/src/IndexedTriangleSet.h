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
/// \file IndexedTriangleSet.h
/// \brief Header file for IndexedTriangleSet, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __INDEXEDTRIANGLESET_H__
#define __INDEXEDTRIANGLESET_H__

#include <X3DComposedGeometryNode.h>
#include <DependentNodeFields.h>
#include <X3DCoordinateNode.h>
#include <X3DColorNode.h>
#include <CoordBoundField.h>
#include <MFInt32.h>
#include <SFInt32.h>
#include <HapticTriangleTree.h>

namespace H3D {

  /// \ingroup X3DNodes
  /// \class IndexedTriangleSet
  /// \brief The IndexedTriangleSet node represents a 3D shape composed of a
  /// collection of individual triangles.
  ///
  /// IndexedTriangleSet uses the indices in its index field to
  /// specify the vertices of each triangle from the coord field. Each
  /// triangle is formed from a set of three vertices of the Coordinate
  /// node identified by three consecutive indices from the index field. If
  /// the index field does not contain a multiple of three coordinate
  /// values, the remaining vertices shall be ignored. 
  ///
  /// The IndexedTriangleSet node is specified in the local coordinate
  /// system and is affected by the transformations of its
  /// ancestors.  If values are provided for the
  /// color, normal and texCoord fields, the values are applied in the same
  /// manner as the values from the coord field and there shall be at least
  /// as many values as are present in the coord field. The value of the
  /// colorPerVertex field is ignored and always treated as TRUE. If the
  /// normal field is not supplied, normals shall be generated as follows:
  /// 
  /// - If normalPerVertex is TRUE, the normal at each vertex shall be the
  /// average of the normals for all triangles that share that vertex. 
  /// 
  /// - If normalPerVertex is FALSE, the normal at each vertex shall be
  /// perpendicular to the face for that triangle. 
  ///
  /// \par Internal routes:
  /// \dotfile IndexedTriangleSet.dot
  class H3DAPI_API IndexedTriangleSet : public X3DComposedGeometryNode {
  public:
    
    /// Thrown if the number of colors in the color field is less than
    /// the number coordinates in the coord field.
    H3D_VALUE_EXCEPTION( int, NotEnoughColors );

    /// Thrown if the number of texture coordinates in the color field is less 
    /// than the number coordinates in the coord field.
     H3D_VALUE_EXCEPTION( int, NotEnoughTextureCoordinates );

    /// The bound field for IndexedFaceSet is a CoordBoundField.
    typedef CoordBoundField SFBound;

    /// Specialized field for automatically generating normals from
    /// coordinates.
    /// routes_in[0] is the normalPerVertex field.
    /// routes_in[1] is the coord field.
    /// routes_in[2] is the index field.
    /// routes_in[3] is the ccw field.
    
    class H3DAPI_API AutoNormal: 
      public TypedField< SFNormalNode,
                         Types< SFBool, SFCoordinateNode, MFInt32, SFBool > > {
      /// Calls generateNormalsPerVertex() if routes_in[0] is true, 
      /// otherwise generateNormalsPerFace() is called.
      virtual void update();

      /// Create a new X3DNormalNode from the arguments given
      /// with one normal for each vertex specified.
      ///
      /// \param coord Node with the coordinates.
      /// \param index The indices in coord for the vertices.
      /// \param ccw Defines the ordering of the vertex coordinates of the 
      /// geometry with respect to generated normal vectors used in the 
      /// lighting model equations. If ccw is TRUE, the normals shall 
      /// follow the right hand rule; the orientation of each normal with
      /// respect to the vertices (taken in order) shall be such that the
      /// vertices appear to be oriented in a counterclockwise order when 
      /// the vertices are viewed (in the local coordinate system of the Shape)
      /// from the opposite direction as the normal.
      /// \returns A new Normal node with a normal for each
      /// vertex.
      ///
      virtual X3DNormalNode *generateNormalsPerVertex( 
                                                      X3DCoordinateNode *coord,
                                                      const vector< int > &index,
                                                      bool ccw );
    

      /// Create a new X3DNormalNode from the arguments given
      /// with one normal for each face specified.
      ///
      /// \param coord Node with the coordinates.
      /// \param index The indices in coord for the vertices.
      /// \param ccw Defines the ordering of the vertex coordinates of the 
      /// geometry with respect to generated normal vectors used in the 
      /// lighting model equations. If ccw is TRUE, the normals shall 
      /// follow the right hand rule; the orientation of each normal with
      /// respect to the vertices (taken in order) shall be such that the
      /// vertices appear to be oriented in a counterclockwise order when 
      /// the vertices are viewed (in the local coordinate system of the Shape)
      /// from the opposite direction as the normal.
      /// \returns A new Normal node with a normal for each
      /// vertex.
      ///
      virtual X3DNormalNode *generateNormalsPerFace( 
                                                    X3DCoordinateNode *coord,
                                                    const vector< int > &index,
                                                    bool ccw );

    };

    /// Constructor.
    IndexedTriangleSet( Inst< SFNode           > _metadata        = 0,
                        Inst< SFBound          > _bound           = 0,
                        Inst< DisplayList      > _displayList     = 0,
                        Inst< SFColorNode      > _color           = 0,
                        Inst< SFCoordinateNode > _coord           = 0,
                        Inst< SFNormalNode     > _normal          = 0,
                        Inst< SFTextureCoordinateNode > _texCoord = 0,
                        Inst< SFBool           > _ccw             = 0,
                        Inst< SFBool           > _colorPerVertex  = 0,
                        Inst< SFBool           > _normalPerVertex = 0,
                        Inst< SFBool           > _solid           = 0,
			                  Inst< MFVertexAttributeNode > _attrib     = 0,
                        Inst< AutoNormal       > _autoNormal      = 0,
                        Inst< MFInt32          > _set_index       = 0,
                        Inst< MFInt32          > _index           = 0,
                        Inst< SFFogCoordinate  > _fogCoord        =0 );
 
    /// Traverse the scenegraph. A HLFeedbackShape is added for haptic
    /// rendering if haptics is enabled.
    virtual void traverseSG( TraverseInfo &ti );  

    /// Renders the IndexedTriangleSet with OpenGL.
    virtual void render();

    /// The number of triangles renderered in this geometry.
    virtual int nrTriangles() {
      return index->size() / 3;
    }

    /// Auto-generated normals that are used if the normal field is NULL.
    /// Only accessable in C++.
    ///
    /// \dotfile IndexedFaceSet_autoNormal.dot 
    auto_ptr< AutoNormal  >  autoNormal;

    /// Field for setting the value of the index field.
    /// <b>Access type:</b> inputOnly 
    ///
    /// \dotfile IndexedTriangleSet_set_Index.dot 
    auto_ptr< MFInt32 >  set_index;

    /// The index field is used to specify the vertices of each triangle
    /// from the coord field. Each triangle is formed from a set of three 
    /// vertices of the Coordinate node identified by three consecutive 
    /// indices from the index field. If the index field does not contain
    /// a multiple of three coordinate values, the remaining vertices shall
    ///be ignored.
    ///
    /// <b>Access type:</b> inputOnly 
    ///
    /// \dotfile IndexedTriangleSet_index.dot 
    auto_ptr< MFInt32 >  index;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

     auto_ptr< SFInt32 > depth;
  };
}

#endif
