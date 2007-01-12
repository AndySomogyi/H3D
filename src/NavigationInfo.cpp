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
#include "X3DGeometryNode.h"

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
                                Inst< SFBool    > _transitionComplete ):
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

  moveAvatar->setValue( false );
  moveAvatar->setOwner( this );

  the_root = 0;
  last_time = TimeStamp();
  nav_type = "";
}

void NavigationInfo::detectCollision( X3DViewpointNode * vp,
                                      X3DChildNode *topNode ) {
  const Matrix4f &acc_fr_mt = vp->accForwardMatrix->getValue();
  Vec3f vp_pos = vp->position->getValue();
  Vec3f vp_full_pos = vp_pos + vp->rel_pos;
  Rotation vp_orientation = vp->orientation->getValue();
  Rotation vp_full_orientation = vp_orientation * vp->rel_orientation;
  the_root = topNode;
  H3DTime current_time = TimeStamp();
  H3DTime delta_time = current_time - last_time;
  last_time = current_time;
  string navigation_type = getUsedNavType();
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
    }
  }

  if( !linear_interpolate ) {
    if( navigation_type == "EXAMINE" || navigation_type == "ANY" ) {
      if( moveAvatar->getValue() ) {
        Vec3f center_of_rot = vp->centerOfRotation->getValue();
        Rotation rotation_value = moveAvatar->rotate_dir;
        Vec3f new_pos = Matrix3f( rotation_value ) *
          ( vp_full_pos - center_of_rot ) +
          center_of_rot;
        bool no_collision = true;
        if( (vp_pos - center_of_rot).lengthSqr() > Constants::f_epsilon ) {
          const Matrix4f &acc_fr_mt = vp->accForwardMatrix->getValue();
        }

        if( no_collision ) {
          vp->rel_pos = new_pos - vp_pos;
          Rotation new_rotation = rotation_value * vp_full_orientation;
          vp->rel_orientation = -vp_orientation * new_rotation;
          vp_full_orientation = new_rotation;
        }
        moveAvatar->setValue( false );
      }
    }
    else if( navigation_type == "WALK" ) {
      if( moveAvatar->getValue() ) {
        Vec3f scaling = acc_fr_mt.getScalePart();
        if( H3DAbs( scaling.x - scaling.y ) < Constants::f_epsilon
          && H3DAbs( scaling.y - scaling.z ) < Constants::f_epsilon ) {

          Vec3f translation_delta = moveAvatar->move_dir;
          translation_delta.normalizeSafe();
          translation_delta = translation_delta * scaling.x * speed->getValue() * delta_time;
          Vec3f new_pos = vp_full_pos + translation_delta;
          vector< H3DFloat > avatar_size = avatarSize->getValue();
          if( avatar_size.empty() ||
            !topNode->movingSphereIntersect( avatar_size[0], 
                                             acc_fr_mt * vp_full_pos,
                                             acc_fr_mt * new_pos ) ) {
            vp->rel_pos = new_pos - vp_pos;
            vp_full_pos = new_pos;
          }
        }
        else {
          Console(3) << "Warning: Non-uniform scaling in the"
            << " active Viewpoint ( "
            << vp->getName()
            << " ) nodes local coordinate system. Speed of "
            << "Avatar is undefined ";
        }
        moveAvatar->setValue( false );
      }
    }
    else if( navigation_type == "FLY" ) {
      if( moveAvatar->getValue() ) {
        Vec3f scaling = acc_fr_mt.getScalePart();
        if( H3DAbs( scaling.x - scaling.y ) < Constants::f_epsilon
          && H3DAbs( scaling.y - scaling.z ) < Constants::f_epsilon ) {

          Vec3f translation_delta = moveAvatar->move_dir;
          translation_delta.normalizeSafe();
          translation_delta =
            translation_delta * scaling.x * speed->getValue() * delta_time;
          Vec3f new_pos = vp_full_pos + translation_delta;
          vector< H3DFloat > avatar_size = avatarSize->getValue();
          if( avatar_size.empty() ||
            !topNode->movingSphereIntersect( avatar_size[0], 
                                             acc_fr_mt * vp_full_pos,
                                             acc_fr_mt * new_pos ) ) {
            vp->rel_pos = new_pos - vp_pos;
            vp_full_pos = new_pos;
          }
        }
        else {
          Console(3) << "Warning: Non-uniform scaling in the"
            << " active Viewpoint ( "
            << vp->getName()
            << " ) nodes local coordinate system. Speed of "
            << "Avatar is undefined ";
        }
        moveAvatar->setValue( false );
      }
    }
    else if( navigation_type == "LOOKAT" ) {
      if( moveAvatar->getValue() ) {
        GLint viewport[4];
        GLdouble mvmatrix[16], projmatrix[16];
        GLdouble wx, wy, wz;
        glGetIntegerv( GL_VIEWPORT, viewport );
        glGetDoublev( GL_MODELVIEW_MATRIX, mvmatrix );
        glGetDoublev( GL_PROJECTION_MATRIX, projmatrix );

        Vec2f mouse_pos = mouseSensor->position->getValue();
        mouse_pos.y = viewport[3] - mouse_pos.y - 1;
        gluUnProject( (GLdouble) mouse_pos.x, (GLdouble) mouse_pos.y,
          0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
        Vec3f near_plane_pos =
          Vec3f( (H3DFloat)wx, (H3DFloat)wy, (H3DFloat)wz );
        gluUnProject( (GLdouble) mouse_pos.x, (GLdouble) mouse_pos.y,
          1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
        Vec3f far_plane_pos =
          Vec3f( (H3DFloat)wx, (H3DFloat)wy, (H3DFloat)wz );

        vector< HAPI::Bounds::IntersectionInfo > result;
        vector< X3DGeometryNode * > theGeometry;
        vector< H3DInt32 > theGeometryIndex;
        Matrix4f temp_matrix;
        vector< Matrix4f > transform_matrices;
        if( topNode->lineIntersect( near_plane_pos, 
                              far_plane_pos,
                              result,
                              theGeometry,
                              theGeometryIndex,
                              temp_matrix,
                              transform_matrices ) ) {
          int closest = 0;
          if( theGeometry.size() > 1 ) {
            H3DFloat closestDistance = 
              (H3DFloat)(result[closest].point - near_plane_pos).lengthSqr();
            for( unsigned int kl = 1; kl < theGeometry.size(); kl++ ) {
              H3DFloat tempClose = 
                (H3DFloat)(result[kl].point - near_plane_pos).lengthSqr();
              if( tempClose < closestDistance ) {
                closestDistance = tempClose;
                closest = kl;
              }
            }
          }
          
          Vec3f approx_center;
          H3DFloat viewing_distance;
          BoxBound *box_bound = dynamic_cast< BoxBound * >(
            theGeometry[closest]->bound->getValue() );
          const Matrix4f &vp_acc_inv_mtx = vp->accInverseMatrix->getValue();
          
          if( box_bound ) {
            approx_center = vp_acc_inv_mtx *
              ( transform_matrices[closest ] * box_bound->center->getValue() );
            Vec3f acc_inv_scale = vp_acc_inv_mtx.getScalePart();
            Vec3f geom_scale = transform_matrices[closest].getScalePart();
            Vec3f size = H3DMax( H3DMax( acc_inv_scale.x, acc_inv_scale.y ),
                                 acc_inv_scale.z ) *
                         ( H3DMax( H3DMax( geom_scale.x, geom_scale.y ),
                                   geom_scale.z ) *
                         box_bound->size->getValue() );

            viewing_distance = 2 * size.length();
          }
          else {
            // calculate the center and viewing distance
            // since we know nothing about the geometry except
            // the point of intersection this is just a crude guess
            approx_center = vp_acc_inv_mtx *
              ( transform_matrices[closest ] * approx_center );
            viewing_distance = (vp_acc_inv_mtx *( transform_matrices[closest]
              * result[closest].point ) ).length() * 2;
          }
          Vec3f backward = vp_full_orientation * Vec3f( 0, 0, 1 );
          vp->centerOfRotation->setValue( approx_center );

          Vec3f new_pos = approx_center + viewing_distance * backward;
          vp->rel_pos = new_pos - vp_pos;
          vp_full_pos = new_pos;

          // No check is done to make sure that the selected object is not
          // obscured by other objects or that the new position is in a place
          // where fly and walk would detect collision.
        }
        moveAvatar->setValue( false );
      }
    }
    else if( navigation_type == "NONE" ) {
      // Forcing the user to navigate using only mechanisms in the scene
      // if the user want navigation again, a new navigationinfo has to
      // be bound. ( or rebind the current one with type changed )
      keySensor->enabled->setValue( false );
      mouseSensor->enabled->setValue( false );
    }
  }

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

string NavigationInfo::getUsedNavType() {
  vector< string > navigation_types = type->getValue();

  bool hasAny = false;

  if( !nav_type.empty() ) {
    for( unsigned int i = 0; i < navigation_types.size(); i++ ) {
      if( navigation_types[i] == "ANY" ) {
        hasAny = true;
        break;
      }
    }
    if( hasAny ) {
      if( nav_type == "EXAMINE" ||
          nav_type == "WALK" ||
          nav_type == "FLY" ||
          nav_type == "LOOKAT" ||
          nav_type == "NONE" )
        return nav_type;
    }
    else {
      for( unsigned int i = 0; i < navigation_types.size(); i++ ) {
        if( nav_type == navigation_types[i] )
          return nav_type;
      }
    }
  }

  bool onlyANY = false;
  // Default behaviour
  for( unsigned int i = 0; i < navigation_types.size(); i++ ) {
    if( navigation_types[i] == "EXAMINE" ) {
      return "EXAMINE";
    }
    else if( navigation_types[i] == "WALK" ) {
      return "WALK";
    }
    else if( navigation_types[i] == "FLY" ) {
      return "FLY";
    }
    else if( navigation_types[i] == "LOOKAT" ) {
      return "LOOKAT";
    }
    else if( navigation_types[i] == "ANY" ) {
      onlyANY = true;
    }
    else if( navigation_types[i] == "NONE" ) {
      return "NONE";
    }
  }

  if( onlyANY )
    return "ANY";

  return "";
}

void NavigationInfo::MoveAvatar::update() {
  SFBool::update();
  bool button_pressed = static_cast< SFBool * >(routes_in[1])->getValue();
  Vec2f motion = static_cast< SFVec2f * >(routes_in[2])->getValue();
  //motion.y = -motion.y;
  X3DViewpointNode * vp = X3DViewpointNode::getActive();

  if( vp && ( event.ptr == routes_in[0] || button_pressed ) ) {
    NavigationInfo *nI = 
          static_cast< NavigationInfo * >( getOwner() );
    string navigation_type = nI->getUsedNavType();
    if( navigation_type == "EXAMINE" || navigation_type == "ANY" ) {
      ifExamine( vp, button_pressed, motion );
    }
    else if( navigation_type == "FLY" || navigation_type == "WALK" ) {
      ifFlyOrWalk( vp, button_pressed, motion );
    }
    else if( navigation_type == "LOOKAT" ) {
      if( event.ptr == routes_in[1] ) {
        value = true;
      }
      else {
        value = false;
      }
    }
  }
}

void NavigationInfo::MoveAvatar::ifExamine( X3DViewpointNode * vp,
                                            bool button_pressed,
                                            Vec2f motion ) {
  Rotation rotation_value;

  if( button_pressed && event.ptr == routes_in[2] ) {
    Vec2f perp = Vec2f( -motion.y, -motion.x );
    perp.normalize();
    Vec3f axis = Matrix3f( vp->orientation->getValue() *
      vp->rel_orientation ) *
      Vec3f( perp.x, perp.y, 0 );
    rotation_value = Rotation( axis, motion.length() * 0.01f );
    value = true;
  }
  else if( event.ptr == routes_in[0] ) {
    int key = static_cast< SFInt32 * >(routes_in[0])->getValue();
    if( key == KeySensor::UP ) {
      Vec3f axis = Matrix3f( vp->orientation->getValue() *
        vp->rel_orientation ) * Vec3f( 1, 0, 0 );
      rotation_value = Rotation( axis, 0.1f );
    }
    if( key == KeySensor::DOWN ) {
      Vec3f axis = Matrix3f( vp->orientation->getValue() *
        vp->rel_orientation ) * Vec3f( 1, 0, 0 );
      rotation_value = Rotation( axis, -0.1f );
    }
    if( key == KeySensor::LEFT ) {
      Vec3f axis = Matrix3f( vp->orientation->getValue() *
        vp->rel_orientation ) * Vec3f( 0, 1, 0 );
      rotation_value = Rotation( axis, 0.1f );
    }
    if( key == KeySensor::RIGHT ) {
      Vec3f axis = Matrix3f( vp->orientation->getValue() *
        vp->rel_orientation ) * Vec3f( 0, 1, 0 );
      rotation_value = Rotation( axis, -0.1f );
    }
    value = true;
  }
  else {
    value = false;
  }
  
  rotate_dir = rotation_value;
}

void NavigationInfo::MoveAvatar::ifFlyOrWalk( X3DViewpointNode * vp,
                                              bool button_pressed,
                                              Vec2f motion ) {
  Vec3f forward = Vec3f( 0, 0, -1 );
  Vec3f right = Vec3f( 1, 0, 0 );
  Rotation full_rotation = vp->orientation->getValue() * vp->rel_orientation;
  forward = full_rotation * forward;
  right = full_rotation * right;

  Vec3f translation_delta;
  if( button_pressed && event.ptr == routes_in[2] ) {
    H3DFloat motion_length = motion.length();
    translation_delta = (forward * -motion.y / motion_length
                         + right * motion.x / motion_length);
    value = true;
  }
  else if( event.ptr == routes_in[0] ) {
    int key = static_cast< SFInt32 * >(routes_in[0])->getValue();
    if( key == KeySensor::UP ) {
      translation_delta = forward;
    }
    if( key == KeySensor::DOWN ) {
      translation_delta = -forward;
    }
    if( key == KeySensor::LEFT ) {
      translation_delta = -right;
    }
    if( key == KeySensor::RIGHT ) {
      translation_delta = right;
    }
    value = true;
  }
  else {
    value = false;
  }

  move_dir = translation_delta;
}
