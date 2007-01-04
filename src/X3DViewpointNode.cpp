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
/// \file X3DViewpointNode.cpp
/// \brief CPP file for X3DViewpointNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DViewpointNode.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase X3DViewpointNode::database( 
                                    "X3DViewpointNode", 
                                    NULL,
                                    typeid( X3DViewpointNode ),
                                    &X3DBindableNode::database );

namespace X3DViewpointNodeInternals {
  FIELDDB_ELEMENT( X3DViewpointNode, centerOfRotation, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, description, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, jump, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, orientation, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, position, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, retainUserOffsets, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DViewpointNode, accForwardMatrix, OUTPUT_ONLY );
  FIELDDB_ELEMENT( X3DViewpointNode, accInverseMatrix, OUTPUT_ONLY );
}



X3DViewpointNode::X3DViewpointNode( 
                     Inst< SFSetBind  > _set_bind,
                     Inst< SFVec3f    > _centerOfRotation,
                     Inst< SFString   > _description,
                     Inst< SFBool    >  _jump,
                     Inst< SFNode     > _metadata,
                     Inst< SFRotation > _orientation,
                     Inst< SFVec3f    > _position,
                     Inst< SFBool     > _retainUserOffsets,
                     Inst< SFTime     > _bindTime,
                     Inst< SFBool     > _isBound,
                     Inst< SFMatrix4f > _accForwardMatrix,
                     Inst< SFMatrix4f > _accInverseMatrix ) :
  X3DBindableNode( "X3DViewpointNode", _set_bind, _metadata, 
                   _bindTime, _isBound ),
  centerOfRotation( _centerOfRotation ),
  description     ( _description      ),
  jump            ( _jump             ),
  orientation     ( _orientation      ),
  position        ( _position         ),
  retainUserOffsets( _retainUserOffsets ),
  accForwardMatrix( _accForwardMatrix ),
  accInverseMatrix( _accInverseMatrix ) {
  
  type_name = "X3DViewpointNode";
  database.initFields( this );

  centerOfRotation->setValue( Vec3f( 0, 0, 0 ) );
  orientation->setValue( Rotation( 0, 0, 1, 0 ) );
  position->setValue( Vec3f( 0, 0, 10 ) );
  retainUserOffsets->setValue( false );
  // need to give the id since the fields are output only.
  accForwardMatrix->setValue( Matrix4f(), id );
  accInverseMatrix->setValue( Matrix4f(), id );
}

void X3DViewpointNode::removeFromStack() {
  StackType &s =  stack[bindable_stack_name];
  if( s.size() > 0 ) {
    bool is_active = (s.front() == this);
    X3DBindableNode::removeFromStack();
    if( is_active ) {
      if( s.size() > 0 ) {
        X3DViewpointNode * new_vp =
          static_cast<X3DViewpointNode * >(s.front());
        if( !new_vp->jump->getValue() ) {

          const Matrix4f &vp_acc_inv_mtx =
            new_vp->accInverseMatrix->getValue();
          const Matrix4f &old_vp_acc_frw_mtx = accForwardMatrix->getValue();

          new_vp->rel_pos = vp_acc_inv_mtx *
                            ( old_vp_acc_frw_mtx *
                              ( position->getValue() + rel_pos ) )
                            - new_vp->position->getValue();

          new_vp->rel_orientation = -new_vp->orientation->getValue() * 
            ( Rotation( vp_acc_inv_mtx.getScaleRotationPart() ) *
              ( Rotation(old_vp_acc_frw_mtx.getScaleRotationPart() ) *
                ( orientation->getValue() * rel_orientation ) ) );
        }
      }
    }
  }
}

void X3DViewpointNode::toStackTop() {
  StackType &s =  stack[bindable_stack_name];
  X3DBindableNode *active = NULL;
  if( s.size() > 0 ) active = s.front();
  if ( active != this ) {
    if( jump->getValue() ) {
      if( !retainUserOffsets->getValue() ) {
        rel_pos = Vec3f( 0, 0, 0 );
        rel_orientation = Rotation( 0, 0, 0, 0 );
      }
    }
    else {
      if( active ) {
        X3DViewpointNode * old_vp = static_cast< X3DViewpointNode * >(active);
        const Matrix4f &vp_acc_inv_mtx = accInverseMatrix->getValue();
        const Matrix4f &old_vp_acc_frw_mtx =
          old_vp->accForwardMatrix->getValue();

        rel_pos = vp_acc_inv_mtx *
                  ( old_vp_acc_frw_mtx *
                    ( old_vp->position->getValue() + old_vp->rel_pos ) )
                    - position->getValue();

        rel_orientation = -orientation->getValue() * 
        ( Rotation( vp_acc_inv_mtx.getScaleRotationPart() ) *
          ( Rotation(old_vp_acc_frw_mtx.getScaleRotationPart() ) *
            ( old_vp->orientation->getValue() * old_vp->rel_orientation ) ) );
      }
    }
    X3DBindableNode::toStackTop();
  }
}
