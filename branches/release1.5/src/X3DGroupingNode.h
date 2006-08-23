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
/// \file X3DGroupingNode.h
/// \brief Header file for X3DGroupingNode, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __X3DGROUPINGNODE_H__
#define __X3DGROUPINGNODE_H__

#include "X3DChildNode.h"
#include "X3DBoundedObject.h"
#include "DependentNodeFields.h"
#include "H3DDisplayListObject.h"

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DGroupingNode
  /// This abstract node type indicates that concrete node types derived
  /// from it contain children nodes and is the basis for all
  /// aggregation. 
  class H3DAPI_API X3DGroupingNode : 
    public X3DChildNode,
    public X3DBoundedObject,
    public H3DDisplayListObject{
  public:
    
    /// MFChild sets up a routes from the Nodes that are added and
    /// removed from the field to the owner of the field. 
    /// The displayList field of the Nodes are routed to the displayList
    /// field and the bound fields are routed to the bound field.
    /// 
    class H3DAPI_API MFChild :
      public DependentMFNode< X3DChildNode,
        FieldRef< H3DDisplayListObject,
                  H3DDisplayListObject::DisplayList,
                  &H3DDisplayListObject::displayList >,
                              true >  {
      public:
      /// The base class of MFChild.
      typedef DependentMFNode< 
        X3DChildNode,
        FieldRef< H3DDisplayListObject,
                  H3DDisplayListObject::DisplayList,
                  &H3DDisplayListObject::displayList >,
        true >  MFChildBase;

      /// Destructor. Clears the MFNode in order to get the correct onRemove 
      /// function to be called upon destruction. Must be done in all subclasses
      /// that overrides onRemove ().
      ~MFChild() {
        clear();
      }

      /// Set up the routes we want to maintain.
      virtual void onAdd( Node *n );

      /// Remove the routes we want to maintain.
      virtual void onRemove( Node *n );

    private:
      friend class X3DGroupingNode;
    };
    friend class MFChild;
    
    /// SFBound is specialized to update from the SFBound fields 
    /// routed to it. The resulting Bound object is the union of 
    /// the Bound objects routed to it. If the bboxSize of the 
    /// X3DGrouping node containing the SFBound field is ( -1, -1, -1 )
    /// bound fields of all Nodes in the children field of the containing
    /// Node that are instances of X3DBoundedObject are routed to it.
    /// Otherwise the bound will be a BoxBound with center and
    /// radius specified with the bboxCenter and bboxSize fields.
    ///
    class H3DAPI_API SFBound: 
      public TypedField< X3DBoundedObject::SFBound,
                         void,
                         AnyNumber< X3DBoundedObject::SFBound > > {
      /// The SFBound is updated to a bound that is the union of the 
      /// the Bound objects routed to it.
      virtual void update();
    };

    /// Constructor.
    X3DGroupingNode( 
                    Inst< MFChild  > _addChildren    = 0,
                    Inst< MFChild  > _removeChildren = 0,
                    Inst< MFChild > _children       = 0,
                    Inst< SFNode  > _metadata       = 0,
                    Inst< SFBound > _bound          = 0,
                    Inst< SFVec3f > _bboxCenter     = 0,
                    Inst< SFVec3f > _bboxSize       = 0 );

    /// Sets up the bound field using the bboxCenter and bboxSize fields.
    /// If bboxSize is (-1, -1, -1) the bound will be the union of all the
    /// bound objects of the Nodes in the children field. Otherwise it will
    /// be a BoxBound with center and origin determined by the bboxCenter
    /// and bboxOrigin fields.
    virtual void initialize() {
      const Vec3f &size = bboxSize->getValue();
      if( size.x == -1 && size.y == -1 && size.z == -1 ) {
        NodeVector children_nodes = children->getValue();
        use_union_bound = true;
        // set the values againg in order to run onNodeAdd again
        // with use_union_bound set to true to set the routes.
        children->setValue( children_nodes );
      } else {
        use_union_bound = false;
        BoxBound *bb = new BoxBound();
        bb->center->setValue( bboxCenter->getValue() );
        bb->size->setValue( bboxSize->getValue() );
        bound->setValue( bb );
      }
      X3DChildNode::initialize();
    }

    /// Render the children.
    virtual void render();

#ifdef USE_HAPTICS
    /// Traverse the scenegraph. traverseSG() is called in all children nodes.
    virtual void traverseSG( TraverseInfo &ti );
#endif
  
    /// if true a route will be set up between the bound field of the
    /// nodes in children and the bound field of the grouping node. 
    bool use_union_bound;
    
    /// The addChildren event appends nodes to the children field of a 
    /// grouping node. Any nodes passed to the addChildren field that 
    /// are already in the children list of the grouping node are ignored.
    ///
    /// <b>Access type: </b> inputOnly
    auto_ptr< MFChild  >  addChildren;

    /// The removeChildren event removes nodes from the children field of
    /// the grouping node . Any nodes in the removeChildren event that are
    /// not in the children list of the grouping node are ignored.
    ///
    /// <b>Access type: </b> inputOnly
    auto_ptr< MFChild  >  removeChildren;

    /// The nodes that are grouped together by this node.
    ///
    /// <b>Access type: </b> inputOutput
    auto_ptr< MFChild >  children;
    
    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  };
}

#endif
