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
/// \file PlanarReflector.h
/// \brief Header file for PlanarReflector, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __PLANARREFLECTOR_H__
#define __PLANARREFLECTOR_H__

#include <H3D/X3DGroupingNode.h>
#include <H3D/X3DBoundedObject.h>
#include <H3D/X3DGeometryNode.h>
#include <H3D/X3DViewpointNode.h>
#include <H3D/DependentNodeFields.h>
#include <H3D/H3DDisplayListObject.h>

namespace H3D {
  /// \ingroup AbstractNodes
  /// \class PlanarReflector
  ///
  /// \par Internal routes:
  /// \dotfile PlanarReflector.dot
  class H3DAPI_API PlanarReflector : 
    public X3DChildNode,
    public X3DBoundedObject,
    public H3DDisplayListObject {

  public:
    /// SFGeometryNode is dependent on the displayList field of its
    /// encapsulated X3DGeometryNode node, i.e. an event from that
    /// field will trigger an event from the SFGeometryNode as
    /// well. Also we ovverride onAdd and onRemove in order to uphold
    /// a route between the encapsulated X3DGeometryNode's bound field
    /// to the bound field of the PlanarReflector the field resides in.
    ///
    class SFGeometryNode: 
      public DependentSFNode< X3DGeometryNode, 
      FieldRef< H3DDisplayListObject,
      H3DDisplayListObject::DisplayList,
      &H3DDisplayListObject::displayList >, true > {
        typedef DependentSFNode< X3DGeometryNode, 
        FieldRef< H3DDisplayListObject,
                  H3DDisplayListObject::DisplayList,
                  &H3DDisplayListObject::displayList >, true > BaseField;
    public:
    
      /// Constructor.
      SFGeometryNode() {}

      /// Destructor. Sets the value to NULL in order to get the correct
      /// onAdd and onRemove functions to be called. 
      ~SFGeometryNode() {
        value = NULL;
      }
      
      /// This function will be called when the value of RefCountField
      /// changes. As soon as the value changes onRemove will
      /// be called on the old value and onAdd will be called
      /// on the new value. 
      /// \param n The new value.
      ///
      virtual void onAdd( Node *n ) {
        PlanarReflector *pr = static_cast< PlanarReflector * >( getOwner() );
        BaseField::onAdd( n );
        if( n && pr->use_geometry_bound ) {
          X3DGeometryNode *g = static_cast< X3DGeometryNode * >( n );
          g->bound->route( pr->bound );
        }
      }
      
      /// This function will be called when the value of RefCountField
      /// changes. As soon as the value changes onRemove will
      /// be called on the old value and onAdd will be called
      /// on the new value. 
      /// \param n The old value.
      ///
      virtual void onRemove( Node *n ) {
        PlanarReflector *pr = static_cast< PlanarReflector * >( getOwner() );
        BaseField::onRemove( n );
        if( n && pr->use_geometry_bound ) {
          X3DGeometryNode *g = static_cast< X3DGeometryNode * >( n );
          g->bound->unroute( pr->bound );
        }
      }
      
    };

    /// Constructor.
    PlanarReflector( Inst< SFNode           > _metadata       = 0,
                     Inst< SFBound          > _bound          = 0,
                     Inst< SFVec3f          > _bboxCenter     = 0,
                     Inst< SFVec3f          > _bboxSize       = 0,
                     Inst< DisplayList      > _displayList    = 0,
                     Inst< SFGeometryNode   > _geometry       = 0
                     );

    ~PlanarReflector() {
      instances.remove( this );
    }

    /// Sets up the bound field using the bboxCenter and bboxSize fields.
    /// If bboxSize is (-1, -1, -1) the bound will be the bound of the
    /// geometry field. Otherwise it will be a BoxBound with center
    /// and origin determined by the bboxCenter and bboxOrigin fields.
    virtual void initialize() {
      const Vec3f &size = bboxSize->getValue();
      if( size.x == -1 && size.y == -1 && size.z == -1 ) {
        use_geometry_bound = true;
        X3DGeometryNode *g = 
          static_cast< X3DGeometryNode * >( geometry->getValue() );
        if( g ) {
          g->bound->route( this->bound );
        }
      } else {
        BoxBound *bb = new BoxBound();
        bb->center->setValue( bboxCenter->getValue() );
        bb->size->setValue( bboxSize->getValue() );
        bound->setValue( bb );
      }
      X3DChildNode::initialize();
    }

    static void updateAll( X3DChildNode *n,
                           X3DViewpointNode *vp,
                           const Matrix4f &vp_transform ) { 
      for( list< PlanarReflector * >::iterator i = instances.begin();
           i != instances.end(); i++ ) {
        (*i)->update( n, vp, vp_transform );
      }
    }

    void update ( X3DChildNode *n,
                  X3DViewpointNode *vp,
                  const Matrix4f &vp_transform );

    /// Traverse the scenegraph. Calls traverseSG on appeance and geometry.
    virtual void traverseSG( TraverseInfo &ti );

    /// Detect intersection between a line segment and the X3DGeometryNode
    /// in the field geometry.
    /// \param from The start of the line segment.
    /// \param to The end of the line segment.
    /// \param result Contains info about the closest intersection for every
    /// object that intersects the line
    /// \param theNodes A vector of pairs of pointer and index to
    /// differ between different places in the scene graph for the same Node.
    /// This can happen due to the DEF/USE feature of X3D.
    /// \param current_matrix The current matrix that transforms from the local
    /// coordinate space where this Node resides in the scenegraph to 
    /// global space.
    /// \param geometry_transforms A vector of matrices from the local
    /// coordinate space to global space for each node that the
    /// line intersects.
    /// \param pt_device_affect Flag telling a node if it is affected by a
    /// X3DPointingDeviceSensorNode. Needed to allow for correct behaviour
    /// when using the DEF/USE feature of X3D.
    /// \returns true if intersected, false otherwise.
    virtual bool lineIntersect(
      const Vec3f &from,
      const Vec3f &to,
      vector< IntersectionInfo > &result,
      vector< pair< Node *, H3DInt32 > > &theNodes,
      const Matrix4f &current_matrix,
      vector< Matrix4f > &geometry_transforms,
      bool pt_device_affect = false );

    /// Find closest point on the node in the field geometry.
    /// \param p The point to find the closest point to.
    /// \param closest_point Return parameter for each closest point
    /// \param normal Return parameter for normal at each closest point.
    /// \param tex_coord Return paramater for each texture coordinate at
    /// closest point
    virtual void closestPoint( const Vec3f &p,
                               vector< Vec3f > &closest_point,
                               vector< Vec3f > &normal,
                               vector< Vec3f > &tex_coord );

    /// Detect collision between a moving sphere and the geometry
    /// in the geometry field.
    /// \param The radius of the sphere
    /// \param from The start position of the sphere
    /// \param to The end position of the sphere.
    /// \returns true if intersected, false otherwise.
    virtual bool movingSphereIntersect( H3DFloat radius,
                                        const Vec3f &from, 
                                        const Vec3f &to );

    /// Resets flags used to get correct behaviour for lineIntersect
    /// when using the DEF/USE feature and X3DPointingDeviceSensorNode.
    virtual void resetNodeDefUseId();

    /// Increase an integer used to get correct behaviour for lineIntersect
    /// when using the DEF/USE feature and X3DPointingDeviceSensorNode.
    /// \param pt_device_affect A flag which is true if the node is affected
    /// by a X3DPointingDeviceSensorNode.
    virtual void incrNodeDefUseId( bool pt_device_affect );

    /// Contains the X3DGeometryNode to be rendered.
    /// 
    /// <b>Access type:</b> inputOutput
    /// 
    /// \dotfile PlanarReflector_geometry.dot
    auto_ptr< SFGeometryNode  >  geometry;

    // if true a route will be set up between the bound field of the
    // geometry node in this field and the bound field of the shape. 
    bool use_geometry_bound;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

    // All instances of GeneratedCubeMapTexture that has been created.
    static std::list< PlanarReflector * > instances;

    Matrix4f local_to_global;
  };
}

#endif
