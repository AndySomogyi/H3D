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
//
/// \file Node.h
/// \brief Node base class
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __NODE_H__
#define __NODE_H__

#include <H3D/Instantiate.h>
#include <H3D/H3DNodeDatabase.h>
#include <H3D/TraverseInfo.h>
#include <H3DUtil/RefCountedClass.h>
#include <H3DUtil/Console.h>

using namespace std;

namespace H3D {

  /// \defgroup Nodes Scene-graph nodes.
  ///
  /// \class Node
  /// \brief Node is the base class for all classes that can be part of
  /// the H3D scene-graph.
  class H3DAPI_API Node: public RefCountedClass {
  public:
 
    typedef HAPI::Collision::IntersectionInfo IntersectionInfo;

    /// Constructor.
    Node();
    
    /// Destructor.
    virtual ~Node() {};

    template< class N >
    static Node *newInstance() { return new N; };

    /// the render() function is used for the depth-first rendering traversal 
    /// of the scene-graph. 
    virtual void render()     { };

    /// traverseSG is called once per scenegraph loop on the scene in order to
    /// traverse the scenegraph. During this traversal things can be updated
    /// in the node and HapticObject instances to be rendered should be added 
    /// to the TraverseInfo object.
    /// \param ti The TraverseInfo object containing information about the
    /// traversal.
    /// 
    virtual void traverseSG( TraverseInfo &ti ) {}

    /// Used as input to intersection functions.
    /// If the intersection function succeeds this struct will contain
    /// the nodes that were intersected, the transform matrices from
    /// local to global for each intersected node and a vector of
    /// IntersectionInfo structs containing information about the 
    /// intersection for each intersected node geometry. The meaning of
    /// global coordinates in the comments regarding this struct refers to
    /// the coordinate system of the input to the function for which this
    /// struct contains collision information.
    struct H3DAPI_API NodeIntersectResult {
      // Constructor.
      NodeIntersectResult( void * _user_data = 0 ) :
        user_data( _user_data ) {
        current_matrix.push( Matrix4f() );
      }

      /// A vector of pointers to nodes. Usually the X3DGeometryNodes that
      /// caused the IntersectionInfo results.
      vector< Node * > theNodes;

      /// A vector of HAPI::IntersectionInfo that stores result of intersection
      /// such as point and normal. The point and normal will be in local
      /// coordinates of the intersected node. For convenience there is a
      /// function called transformResults() which changes the point and normal
      /// into global coordinates.
      vector< IntersectionInfo > result;

      /// Optional user_data in case someone want to do add some extra feature
      /// to lineIntersect and one or several custom made nodes.
      void * user_data;

      /// Adds the current transform from local coordinate space to global
      /// coordinate space to geometry_transforms.
      inline void addTransform() {
        geometry_transforms.push_back( getCurrentTransform() );
      }

      /// Get the info in geometry_transforms.
      inline const vector< Matrix4f > &getGeometryTransforms() {
        return geometry_transforms;
      }

      /// Get the current matrix that transforms from local coordinate space
      /// to global coordinate space.
      inline const Matrix4f &getCurrentTransform() {
        return current_matrix.top();
      }

      /// Push onto stack of current transforms. The top is the one that is
      /// considered the current transform in calls to getCurrentTransform().
      inline void pushTransform( const Matrix4f &matrix ) {
        current_matrix.push( current_matrix.top() * matrix );
      }

      /// Remove the top on the stack of current transforms.
      inline void popTransform() {
        current_matrix.pop();
      }

      /// Transforms point and normal in the IntersectionInfo vector from local
      /// to global space.
      inline void transformResult() {
        for( unsigned int i = 0; i < result.size(); i++ ) {
          result[i].point = geometry_transforms[i] * result[i].point;
          result[i].normal = geometry_transforms[i].getRotationPart() *
                             result[i].normal;
        }
      }

      /// Convenience function to add results to the struct.
      inline void addResults( IntersectionInfo &temp_result,
                              Node * the_node,
                              bool add_transform = true ) {
        result.push_back( temp_result );
        theNodes.push_back( the_node );
        if( add_transform )
          addTransform();
      }

    protected:
      /// A vector of matrices from the local
      /// coordinate space to global space for each node that the
      /// line intersects.
      vector< Matrix4f > geometry_transforms;

      /// The top of the stack is the current matrix that transforms from the
      /// local coordinate space where this Node resides in the scenegraph to
      /// global space.
      stack< Matrix4f > current_matrix;

    };

    /// Used as input to lineIntersect functions.
    /// It is basically like NodeIntersectResult with the additional feature of
    /// handling X3DPointingDeviceNode features as per X3D specification.
    struct H3DAPI_API LineIntersectResult : public NodeIntersectResult {
      // Constructor.
      LineIntersectResult( bool _override_no_collision = false,
                           bool _detect_pt_device = false,
                           void *_user_data = 0 ) :
        NodeIntersectResult( _user_data ),
        override_no_collision( _override_no_collision ),
        detect_pt_device( _detect_pt_device ) {}

      /// Flag used to know if lineintersect should be called for the children
      /// in the Collision Node regardless if it is enabled or not.
      bool override_no_collision;

      /// Flag used to know if lineintersect should bother with keeping track
      /// of X3DPointingDeviceSensorNodes.
      bool detect_pt_device;

      /// Contains a list of Nodes (X3DPointingDeviceNodes) and a
      /// transformation matrix from global space to local space of the
      /// nodes. It is assumed that all the Nodes in the list have the same
      /// transformation matrix.
      struct H3DAPI_API PointingDeviceResultStruct {
        list< Node * > x3dptd;
        list< Node * > lowest_enabled;
        Matrix4f global_to_local;
      };

      /// Push struct to current_pt_device stack.
      inline void pushCurrentPtDevice( PointingDeviceResultStruct &tmp ) {
        PointingDeviceResultStruct temp_struct;
        if( !current_pt_device.empty() )
          temp_struct = current_pt_device.top();
        current_pt_device.push( tmp );
        list< Node * > &tmp_list = current_pt_device.top().x3dptd;
        for( list< Node * >::iterator i = tmp_list.begin();
          i != tmp_list.end(); i++ ) {
          current_pt_device.top().lowest_enabled.push_back( *i );
        }
        tmp_list.insert( tmp_list.end(),
                         temp_struct.x3dptd.begin(),
                         temp_struct.x3dptd.end() );
      }

      /// Pop struct from current_pt_device stack
      inline void popCurrentPtDevice() {
        current_pt_device.pop();
      }

      /// Add current pointing device struct to geom_ptd_map.
      inline void addPtDevMap() {
        if( detect_pt_device && !current_pt_device.empty() ) {
          geom_ptd_map[ (unsigned int) result.size() - 1 ] = current_pt_device.top();
        }
      }

      typedef map< unsigned int,
                   PointingDeviceResultStruct > GeomX3DPtdMap;

      /// Used in order to keep track of DEF/USE feature of X3D. Maps indexes
      /// in theNodes vector to PointingDeviceResultStruct which contains
      /// info about the X3DPointingDeviceNodes.
      GeomX3DPtdMap geom_ptd_map;

    protected:
      /// The top of the stack contains the X3DPointingDeviceNodes that is to
      /// be considered.
      stack< PointingDeviceResultStruct > current_pt_device;
    };

    /// Detect intersection between a line segment and the Node.
    /// \param from The start of the line segment.
    /// \param to The end of the line segment.
    /// \param result Contains info about the closest intersection for every
    /// object that intersects the line.
    /// \returns true if intersected, false otherwise.
    virtual bool lineIntersect( 
      const Vec3f &from, 
      const Vec3f &to,    
      LineIntersectResult &result ) {
      return false;
    }

    /// Find closest point on Node to p.
    /// \param p The point to find the closest point to.
    /// \param result A struct containing various results of closest
    /// points such as which geometries the closest points where
    /// detected on.
    virtual void closestPoint( const Vec3f &p,
                               NodeIntersectResult &result ){}

    /// Detect collision between a moving sphere and the Node.
    /// Only nodes to which collision is possible will return true
    /// \param radius The radius of the sphere
    /// \param from The start position of the sphere
    /// \param to The end position of the sphere.
    /// \param result A struct containing various results of intersections
    /// such as which geometries intersected the moving sphere.
    /// \returns true if intersected, false otherwise.
    virtual bool movingSphereIntersect( H3DFloat radius,
                                        const Vec3f &from, 
                                        const Vec3f &to,
                                        NodeIntersectResult &result ){
      return false;
    }
    
    /// Returns the default xml containerField attribute value.
    /// For this node it is "children".
    ///
    virtual string defaultXMLContainerField() {
      return "children";
    }
    
    /// return a pointer to the field specified by name within this instance 
    virtual Field *getField( const string &name );
    
    friend class Field;
    template <class Type>
    friend class SField;
    template <class Type>
    friend class MField;
    template<class RefClass, class BaseField>
    friend class RefCountSField;

  protected:
    static H3DNodeDatabase database;
    int id;
    static int nr_nodes_created;
  };

};

#endif