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
/// \file NavigationInfo.cpp
/// \brief CPP file for NavigationInfo, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "NavigationInfo.h"

using namespace H3D;

X3DViewpointNode * NavigationInfo::old_vp = 0;
bool NavigationInfo::linear_interpolate = false;
Vec3f NavigationInfo::goal_position = Vec3f();
Rotation NavigationInfo::goal_orientation = Rotation();
Vec3f NavigationInfo::old_vp_pos = Vec3f();
Rotation NavigationInfo::old_vp_orientation = Rotation();

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase NavigationInfo::database( 
                                    "NavigationInfo", 
                                    &(newInstance<NavigationInfo>), 
                                    typeid( NavigationInfo ),
				    &X3DBindableNode::database );

namespace NavigationInfoInternals {
  FIELDDB_ELEMENT( NavigationInfo, avatarSize, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, headlight, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, speed, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, transitionTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, transitionType, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, type, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, visibilityLimit, INPUT_OUTPUT );
  FIELDDB_ELEMENT( NavigationInfo, transitionComplete, OUTPUT_ONLY );
  FIELDDB_ELEMENT( NavigationInfo, useNavigationKeys, INPUT_OUTPUT );
}



NavigationInfo::NavigationInfo( Inst< SFSetBind > _set_bind,
                                Inst< SFNode    > _metadata,
                                Inst< SFTime    > _bindTime,
                                Inst< SFBool    > _isBound,
                                Inst< MFFloat   > _avatarSize,
                                Inst< SFBool    > _headlight,
                                Inst< SFFloat   > _speed,
                                Inst< SFTime    > _transitionTime,
                                Inst< MFString  > _transitionType,
                                Inst< MFString  > _type,
                                Inst< SFFloat   > _visibilityLimit,
                                Inst< SFBool    > _transitionComplete,
                                Inst< SFBool    > _useNavigationKeys ):
  X3DBindableNode( "NavigationInfo",_set_bind, _metadata, _bindTime,
                   _isBound ),
  avatarSize( _avatarSize ),
  headlight( _headlight  ),
  speed( _speed ),
  transitionTime( _transitionTime ),
  transitionType( _transitionType  ),
  type( _type ),
  visibilityLimit( _visibilityLimit ),
  transitionComplete( _transitionComplete ),
  useNavigationKeys( _useNavigationKeys ),
  moveAvatar( new MoveAvatar ) {
  
  type_name = "NavigationInfo";
  database.initFields( this );

  avatarSize->push_back( (H3DFloat)0.25 );
  avatarSize->push_back( (H3DFloat)1.6 );
  avatarSize->push_back( (H3DFloat)0.75 );
  headlight->setValue( true );
  speed->setValue( 1 );
  transitionTime->setValue( 1.0 );
  transitionType->push_back( "LINEAR" );  
  type->push_back( "EXAMINE" );
  type->push_back( "ANY" );
  visibilityLimit->setValue( 0 );

  old_vp = X3DViewpointNode::getActive();
  if( old_vp ) {
    old_vp_pos = old_vp->position->getValue() + old_vp->rel_pos;
    old_vp_orientation = old_vp->orientation->getValue() *
                         old_vp->rel_orientation;
  }

  keySensor = new KeySensor;
  mouseSensor = new MouseSensor;

  NavigationInfo * isActive = getActive();

  if( isActive != this ) {
    keySensor->enabled->setValue( false );
    mouseSensor->enabled->setValue( false );
  }
  else {
    keySensor->enabled->setValue( true );
    mouseSensor->enabled->setValue( true );
  }

  keySensor->actionKeyPress->routeNoEvent( moveAvatar );
  mouseSensor->leftButton->routeNoEvent( moveAvatar );
  mouseSensor->motion->routeNoEvent( moveAvatar );

  moveAvatar->setValue( true );
  moveAvatar->setOwner( this );

  the_root = 0;
}

void NavigationInfo::detectCollision( X3DViewpointNode * vp,
                                      X3DChildNode *topNode ) {
  const Matrix4f &acc_fr_mt = vp->accForwardMatrix->getValue();
  Vec3f vp_pos = vp->position->getValue();
  Vec3f vp_full_pos = vp_pos + vp->rel_pos;
  Vec3f global_point = acc_fr_mt * vp_full_pos;
  Rotation vp_orientation = vp->orientation->getValue();
  Rotation vp_full_orientation = vp_orientation * vp->rel_orientation;
  the_root = topNode;
  if( old_vp && old_vp != vp ) {
    // if the viewpoint is switched when a transition is going on
    // reset the old viewpoint and calculate the new transition from
    // current position and viewpoint.
    if( linear_interpolate ) {
      if( !old_vp->retainUserOffsets->getValue() ) {
        old_vp->rel_pos = goal_position;
        old_vp->rel_orientation = goal_orientation;
      }
      linear_interpolate = false;
    }

    if( vp->jump->getValue() ) {

      string transition = "LINEAR";
      //TODO: allow for run-time choices of transitionType somehow.
      // checking which values are allowed. (transitionType is a MFString).
      if( !transitionType->empty() )
        transition = transitionType->getValueByIndex( 0 );

      if( transition == "TELEPORT" ) {
        transitionComplete->setValue( true, id );
      }
      else if( transition == "ANIMATE" ) {
        // there should be some other kind of behaviour here.
        transitionComplete->setValue( true, id );
      }
      else {
        linear_interpolate = true;
        const Matrix4f &vp_acc_inv_mtx = vp->accInverseMatrix->getValue();
        const Matrix4f &old_vp_acc_frw_mtx = 
          old_vp->accForwardMatrix->getValue();

        start_orientation = -vp_orientation * 
          ( (Rotation)vp_acc_inv_mtx.getScaleRotationPart() *
          ( (Rotation)old_vp_acc_frw_mtx.getScaleRotationPart()
          * old_vp_orientation ) );

        /*start_orientation = Rotation((vp_acc_inv_mtx *
        (old_vp_acc_frw_mtx
        * Matrix4f(old_vp_orientation))).getRotationPart());*/

        goal_orientation = vp->rel_orientation;

        start_position = vp_acc_inv_mtx * 
          (old_vp_acc_frw_mtx * old_vp_pos) - vp_pos;
        goal_position = vp->rel_pos;
        move_direction = goal_position - start_position;
        start_time = TimeStamp();
      }
    }
    old_vp = vp;
  }

  // When a transition takes place navigationinfo negates external
  // use of setValue of the position field of a viewpoint
  if( linear_interpolate ) {
    H3DTime current_time = TimeStamp();
    H3DTime elapsed_time = current_time - start_time;
    H3DTime total_time = transitionTime->getValue();
    if( elapsed_time < total_time ) {
      H3DDouble interpolation = elapsed_time / total_time;
      vp->rel_pos =  start_position +
        move_direction * interpolation;
      vp->rel_orientation = start_orientation.slerp( goal_orientation,
        (H3DFloat)interpolation );
    }
    else {
      linear_interpolate = false;
      transitionComplete->setValue( true, id );
      vp->rel_pos = goal_position;
      vp_full_pos = vp_pos + goal_position;
      vp->rel_orientation = goal_orientation;
      vp_full_orientation = vp_orientation * goal_orientation;
      global_point = acc_fr_mt * vp_full_pos;
    }
  }

  //vector< H3DFloat > avatar_size = avatarSize->getValue();
  //if( !avatar_size.empty() ) {
  //  if( !linear_interpolate &&
  //      global_point != old_vp->accForwardMatrix->getValue() * old_vp_pos ) {
  //    vector< Vec3f > the_points;
  //    vector< Vec3f > the_normals;
  //    vector< Vec3f > the_tex_coords;
  //    topNode->closestPoint( global_point,
  //      the_points,
  //      the_normals,
  //      the_tex_coords );
  //    Vec3f scaling = acc_fr_mt.getScalePart();
  //    if( scaling.x == scaling.y && scaling.y == scaling.z ) {
  //      Vec3f resulting_move = Vec3f();
  //      for( unsigned int i = 0; i < the_points.size(); i++ ) {
  //        H3DFloat distance = (the_points[i] - global_point).length();
  //        if( distance < scaling.x * avatar_size[0] ) {
  //          //TODO: find a better correction of avatars position.
  //          // this one can result in switching back and forth between
  //          // two positions (or worse).
  //          resulting_move += the_normals[i] * distance;
  //        }
  //      }
  //      vp->rel_pos += resulting_move;
  //      vp_full_pos = vp->rel_pos + vp_pos;
  //    }
  //    else {
  //      Console(3) << "Warning: Non-uniform scaling in the active Viewpoint "
  //        << "nodes local coordinate system. Collision Detection with "
  //        << "Avatar is undefined ";
  //    }
  //  }
  //}
  //else {
  //  Console(3) << "Warning: The field avatarSize( " << avatarSize->getName()
  //    << " ) in NavigationInfo node( "
  //    << getName() << " ) is empty."
  //    << " No collision with avatar will be detected " << endl;
  //}

  old_vp_pos = vp_full_pos;
  old_vp_orientation = vp_full_orientation;
}

void NavigationInfo::removeFromStack() {
  bool is_active = (getActive() == this);
  if( is_active ) {
    X3DBindableNode::removeFromStack();
    keySensor->enabled->setValue( false );
    mouseSensor->enabled->setValue( false );
  }
}

void NavigationInfo::toStackTop() {
  if ( getActive() != this ) {
    X3DBindableNode::toStackTop();
    keySensor->enabled->setValue( true );
    mouseSensor->enabled->setValue( true );
  }
}

void NavigationInfo::MoveAvatar::update() {
  SFBool::update();
  NavigationInfo *nI = 
          static_cast< NavigationInfo * >( getOwner() );
  bool button_pressed = static_cast< SFBool * >(routes_in[1])->getValue();
  Vec2f motion = static_cast< SFVec2f * >(routes_in[2])->getValue();
  //motion.y = -motion.y;
  vector< string > move_type = nI->type->getValue();
  X3DViewpointNode * vp = X3DViewpointNode::getActive();

  if( vp && !move_type.empty() && ( event.ptr == routes_in[0] ||
      ( event.ptr == routes_in[2] &&
        motion * motion > Constants::f_epsilon && button_pressed ) ) ) {
    
    Rotation rotation_value;

    if( button_pressed && event.ptr == routes_in[2] ) {
      if( move_type[0] == "EXAMINE" ) {
        Vec2f perp = Vec2f( -motion.y, -motion.x );
        perp.normalize();
        rotation_value = Rotation( perp.x, perp.y, 0, motion.length() * 0.01f );
      }
    }
    else if( event.ptr == routes_in[0] ) {
      int key = static_cast< SFInt32 * >(routes_in[0])->getValue();
      if( key == KeySensor::UP ) {
        if( move_type[0] == "EXAMINE" ) {
          Vec3f axis = Matrix3f( vp->orientation->getValue() *
            vp->rel_orientation ) * Vec3f( 1, 0, 0 );
          rotation_value = Rotation( axis, -0.1f );
          //rotation_value = Rotation( 1, 0, 0, -0.1f );
        }
      }
      if( key == KeySensor::DOWN ) {
        if( move_type[0] == "EXAMINE" ) {
          Vec3f axis = Matrix3f( vp->orientation->getValue() *
            vp->rel_orientation ) * Vec3f( 1, 0, 0 );
          rotation_value = Rotation( axis, 0.1f );
          //rotation_value = Rotation( 1, 0, 0, 0.1f );
        }
      }
      if( key == KeySensor::LEFT ) {
        if( move_type[0] == "EXAMINE" ) {
          Vec3f axis = Matrix3f( vp->orientation->getValue() *
            vp->rel_orientation ) * Vec3f( 0, 1, 0 );
          rotation_value = Rotation( axis, -0.1f );
          //rotation_value = Rotation( 0, 1, 0, -0.1f );
        }
      }
      if( key == KeySensor::RIGHT ) {
        if( move_type[0] == "EXAMINE" ) {
          Vec3f axis = Matrix3f( vp->orientation->getValue() *
            vp->rel_orientation ) * Vec3f( 0, 1, 0 );
          rotation_value = Rotation( axis, 0.1f );
          //rotation_value = Rotation( 0, 1, 0, 0.1f );
        }
      }
    }
    Vec3f center_of_rot = vp->centerOfRotation->getValue();
    Vec3f vp_pos = vp->position->getValue();
    Vec3f vp_rel_pos = vp->rel_pos;
    Vec3f vp_full_pos = vp_pos + vp_rel_pos;

    Vec3f new_pos = Matrix3f( rotation_value ) *
      ( vp_full_pos - center_of_rot ) +
      center_of_rot;
    bool no_collision = true;
    if( (vp_pos - center_of_rot).lengthSqr() > Constants::f_epsilon ) {
      const Matrix4f &acc_fr_mt = vp->accForwardMatrix->getValue();
      Vec3f global_point = acc_fr_mt * new_pos;

      vector< H3DFloat > avatar_size = nI->avatarSize->getValue();
      if( !avatar_size.empty() ) {
        vector< Vec3f > the_points;
        vector< Vec3f > the_normals;
        vector< Vec3f > the_tex_coords;
        X3DChildNode *big_root = nI->the_root;
        if( big_root ) {
          big_root->closestPoint( global_point,
            the_points,
            the_normals,
            the_tex_coords );
          Vec3f scaling = acc_fr_mt.getScalePart();
          if( scaling.x == scaling.y && scaling.y == scaling.z ) {
            Vec3f resulting_move = Vec3f();
            for( unsigned int i = 0; i < the_points.size(); i++ ) {
              H3DFloat distance = (the_points[i] - global_point).length();
              if( distance < scaling.x * avatar_size[0] ) {
                no_collision = false;
                cerr << " we have collision " << endl;
                break;
              }
            }
          }
          else {
            Console(3) << "Warning: Non-uniform scaling in the active Viewpoint "
              << "nodes local coordinate system. Collision Detection with "
              << "Avatar is undefined ";
          }
        }
      }
      else {
        Console(3) << "Warning: The field avatarSize( " << nI->avatarSize->getName()
          << " ) in NavigationInfo node( "
          << nI->getName() << " ) is empty."
          << " No collision with avatar will be detected " << endl;
      }
    }

    if( no_collision ) {
      vp->rel_pos = new_pos - vp_pos;
      Rotation full_rotation = vp->orientation->getValue() * vp->rel_orientation;
      Rotation new_rotation = rotation_value * full_rotation;
      vp->rel_orientation = -vp->orientation->getValue() * new_rotation;
    }
  }
}
