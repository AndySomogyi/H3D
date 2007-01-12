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
/// \file X3DGroupingNode.cpp
/// \brief CPP file for X3DGroupingNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DGroupingNode.h"
#include "H3DRenderStateObject.h"
#include "MatrixTransform.h"
#include "X3DPointingDeviceSensorNode.h"

using namespace H3D;

H3DNodeDatabase X3DGroupingNode::database( 
        "X3DGroupingNode", 
        NULL,
        typeid( X3DGroupingNode ),
        &X3DChildNode::database 
        );

namespace X3DGroupingNodeInternals {
  FIELDDB_ELEMENT( X3DGroupingNode, addChildren, INPUT_ONLY );
  FIELDDB_ELEMENT( X3DGroupingNode, removeChildren, INPUT_ONLY );
  FIELDDB_ELEMENT( X3DGroupingNode, children, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DGroupingNode, bboxCenter, INITIALIZE_ONLY );
  FIELDDB_ELEMENT( X3DGroupingNode, bboxSize, INITIALIZE_ONLY );
}

X3DGroupingNode::X3DGroupingNode( Inst< MFChild >  _addChildren,
                                  Inst< MFChild >  _removeChildren,
                                  Inst< MFChild >  _children,
                                  Inst< SFNode  >  _metadata,
                                  Inst< SFBound >  _bound,
                                  Inst< SFVec3f >  _bboxCenter,
                                  Inst< SFVec3f >  _bboxSize ) :
  X3DChildNode( _metadata ),
  X3DBoundedObject( _bound, _bboxCenter, _bboxSize ),
  use_union_bound( false ),
  addChildren   ( _addChildren    ),
  removeChildren( _removeChildren ),
  children      ( _children       ) {

  type_name = "X3DGroupingNode";
  database.initFields( this );

  displayList->setOwner( this );
  bound->setOwner( this );

  bound->setValue( new EmptyBound );
  children->route( displayList );
}

void X3DGroupingNode::render()     { 
  X3DChildNode::render();
  
  for( MFNode::const_iterator i = children->begin();
       i != children->end(); i++ ) {
    H3DRenderStateObject *l = dynamic_cast< H3DRenderStateObject* >( *i );
    if ( l ) {
      l->enableGraphicsState();
    }
  }
  
  //cerr << "X3DGroupingNode::render() children" << endl;
  for( MFNode::const_iterator i = children->begin();
       i != children->end(); i++ ) {
    H3DDisplayListObject *c = dynamic_cast< H3DDisplayListObject* >( *i );
    if ( c ) {
      c->displayList->callList();
    } else {
      //if(*i)
        (*i)->render();
    }
  }

  for( MFNode::const_iterator i = children->begin();
       i != children->end(); i++ ) {
    H3DRenderStateObject *l = dynamic_cast< H3DRenderStateObject* >( *i );
    if ( l ) {
      l->disableGraphicsState();
    }
  }

};

void X3DGroupingNode::traverseSG( TraverseInfo &ti ) {
  for( MFNode::const_iterator i = children->begin();
       i != children->end(); i++ ) {
    H3DRenderStateObject *l = dynamic_cast< H3DRenderStateObject* >( *i );
    if ( l ) {
      l->enableHapticsState( ti );
    }
  }

  // If there are pointing device sensors in this node add them
  // to the current_pt_dev_sensors vector and set the correct
  // matrix for the sensors. ( This is done here since MatrixTransform
  // calls X3DGroupingNodes traverseSG function.
  vector< X3DPointingDeviceSensorNode * > old_pt_dev_sens_nodes;
  if( !pt_dev_sens_nodes.empty() ) {
    old_pt_dev_sens_nodes = ti.current_pt_dev_sensors;
    bool cleared = false;
    for( unsigned int i = 0; i < pt_dev_sens_nodes.size(); i++ ) {
      if( !cleared && pt_dev_sens_nodes[i]->enabled->getValue() ) {
        cleared = true;
        ti.current_pt_dev_sensors.clear();
      }
      if( cleared ) {
        pt_dev_sens_nodes[i]->setCurrentMatrix( ti.getAccInverseMatrix() );
        ti.current_pt_dev_sensors.push_back( pt_dev_sens_nodes[i] );
      }
    }
  }

  // not using iterators since they can become invalid if the 
  // traversal changes the children field while iterating.
  const NodeVector &c = children->getValue();
  for( unsigned int i = 0; i < c.size(); i++ ) {
    if( c[i] )
      c[i]->traverseSG( ti );
  }

  // reset the current_pt_dev_sensors vector to the previous value
  if( !pt_dev_sens_nodes.empty() ) {
    ti.current_pt_dev_sensors = old_pt_dev_sens_nodes;
    for( unsigned int i = 0; i < pt_dev_sens_nodes.size(); i++ ) {
      pt_dev_sens_nodes[i]->setCurrentMatrix( Matrix4f() );
    }
  }

  for( MFNode::const_reverse_iterator i = children->rbegin();
       i != children->rend(); i++ ) {
    H3DRenderStateObject *l = dynamic_cast< H3DRenderStateObject* >( *i );
    if ( l ) {
      l->disableHapticsState( ti );
    }
  }
}

bool X3DGroupingNode::lineIntersect(
                  const Vec3f &from, 
                  const Vec3f &to,    
                  vector< HAPI::Bounds::IntersectionInfo > &result,
                  vector< X3DGeometryNode * > &theGeometry,
                  vector< H3DInt32 > &theGeometryIndex,
                  const Matrix4f &current_matrix,
                  vector< Matrix4f > &geometry_transforms ) {
  const NodeVector &children_nodes = children->getValue();
  bool intersect = false;
  for( unsigned int i = 0; i < children_nodes.size(); i++ ) {
    if( children_nodes[i]->
         lineIntersect( from, to, result, 
                        theGeometry, theGeometryIndex,
                        current_matrix,
                        geometry_transforms ) ) {
      intersect = true;
    }
  }
  return intersect;
}

void X3DGroupingNode::closestPoint(
                  const Vec3f &p,
                  vector< Vec3f > &closest_point,
                  vector< Vec3f > &normal,
                  vector< Vec3f > &tex_coord ) {
  const NodeVector &children_nodes = children->getValue();
  for( unsigned int i = 0; i < children_nodes.size(); i++ ) {
    children_nodes[i]->closestPoint( p, closest_point, normal, tex_coord);
  }
}

void X3DGroupingNode::SFBound::update() {
  value = Bound::SFBoundUnion( routes_in.begin(),
                               routes_in.end() );
}

void X3DGroupingNode::MFChild::onAdd( Node *n ) {
  MFChildBase::onAdd( n );
  X3DChildNode *c = static_cast< X3DChildNode* >( n );
  X3DGroupingNode *o = static_cast< X3DGroupingNode* >( owner );
  if ( c ) {
    //c->displayList->route( o->displayList );
    if( o->use_union_bound ) {
      H3DBoundedObject *bo = 
        dynamic_cast< H3DBoundedObject * >( n );
      if( bo ) {
        MatrixTransform *t = dynamic_cast< MatrixTransform *>( n );
        if( t ) {
          t->transformedBound->route( o->bound );
        } else {
          bo->bound->route( o->bound );
        }
      }
    }

    // If we have a X3DPointingDeviceSensorNode add it to a separate
    // vector.
    X3DPointingDeviceSensorNode * pdsn = 
      dynamic_cast< X3DPointingDeviceSensorNode * >( n );
    if( pdsn )
      o->pt_dev_sens_nodes.push_back( pdsn );
  }
}

void X3DGroupingNode::MFChild::onRemove( Node *n ) {
  X3DChildNode *c = static_cast< X3DChildNode* >( n );
  X3DGroupingNode *o = static_cast< X3DGroupingNode* >( owner );
  if ( c ) {
    //c->displayList->unroute( o->displayList );
    if( o->use_union_bound ) {
      H3DBoundedObject *bo = 
        dynamic_cast< H3DBoundedObject * >( n );
      if( bo ) {
        MatrixTransform *t = dynamic_cast< MatrixTransform *>( n );
        if( t ) {
          t->transformedBound->route( o->bound );
        } else {
          bo->bound->unroute( o->bound );
        }
      }
    }

    // Remove eventual X3DPointingDeviceSensorNodes from the
    // separate vector.
    if( !o->pt_dev_sens_nodes.empty() ) {
      X3DPointingDeviceSensorNode * pdsn = 
        dynamic_cast< X3DPointingDeviceSensorNode * >( n );
      if( pdsn ) {
        vector< X3DPointingDeviceSensorNode * >::iterator i;
        bool found = false;
        for( i = o->pt_dev_sens_nodes.begin();
             i < o->pt_dev_sens_nodes.end(); i++ )
          if( *i == n ) {
            found = true;
            break;
          }
          if( found )
            o->pt_dev_sens_nodes.erase( i );
      }
    }
  }
  MFChildBase::onRemove( n );
}


bool X3DGroupingNode::movingSphereIntersect( H3DFloat radius,
                                             const Vec3f &from, 
                                             const Vec3f &to ) {
  const NodeVector &children_nodes = children->getValue();
  for( unsigned int i = 0; i < children_nodes.size(); i++ ) {
    if( children_nodes[i]->movingSphereIntersect( radius, from, to ) )
      return true;
  }
  return false;
}