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

X3DViewpointNode * NavigationInfo::old_viewpoint = 0;
bool NavigationInfo::linear_interpolate = false;
Vec3f NavigationInfo::goal_position = Vec3f();
Rotation NavigationInfo::goal_orientation = Rotation();

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
  X3DBindableNode( "NavigationInfo",_set_bind, _metadata, _bindTime, _isBound ),
  avatarSize( _avatarSize ),
  headlight( _headlight  ),
  speed( _speed ),
  transitionType( _transitionType  ),
  transitionTime( _transitionTime ),
  type( _type ),
  visibilityLimit( _visibilityLimit ),
  transitionComplete( _transitionComplete ) {
  
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

  old_viewpoint = X3DViewpointNode::getActive();
}

void NavigationInfo::detectCollision( X3DViewpointNode * vp,
                                      X3DChildNode *topNode ) {
  const Matrix4f &acc_fr_mt = vp->accForwardMatrix->getValue();
  Vec3f vp_pos = vp->position->getValue();
  Vec3f old_vp_pos = old_viewpoint->position->getValue();
  Vec3f global_point = acc_fr_mt * vp_pos;
  Rotation vp_orientation = vp->orientation->getValue();
  Rotation old_vp_orientation = old_viewpoint->orientation->getValue();
  vector< H3DFloat > avatar_size = avatarSize->getValue();
  if( !avatar_size.empty() ) {
    if( global_point != 
        old_viewpoint->accForwardMatrix->getValue() * old_vp_pos ) {
      
      if( old_viewpoint != vp ) {

        // if the viewpoint is switched when a transition is going on
        // reset the old viewpoint and calculate the new transition from
        // current position and viewpoint.
        if( linear_interpolate ) {
          old_viewpoint->position->setValue( goal_position );
          old_viewpoint->orientation->setValue( goal_orientation );
        }

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
            old_viewpoint->accForwardMatrix->getValue();

          start_orientation = Rotation( vp_acc_inv_mtx.getRotationPart() ) *
                              ( Rotation(old_vp_acc_frw_mtx.getRotationPart() )
                                * old_vp_orientation );
          goal_orientation = vp_orientation;
          start_position = vp_acc_inv_mtx * 
                           (old_vp_acc_frw_mtx * old_vp_pos);
          goal_position = vp_pos;
          move_direction = goal_position - start_position;
          start_time = TimeStamp();
          total_time = transitionTime->getValue();
        }
        old_viewpoint = vp;
      }

      if( !linear_interpolate ) {
        vector< Vec3f > the_points;
        vector< Vec3f > the_normals;
        vector< Vec3f > the_tex_coords;
        topNode->closestPoint( global_point,
                               the_points,
                               the_normals,
                               the_tex_coords );
        Vec3f scaling = acc_fr_mt.getScalePart();
        if( scaling.x == scaling.y && scaling.y == scaling.z ) {
          for( unsigned int i = 0; i < the_points.size(); i++ ) {
            H3DFloat distance = (the_points[i] - global_point).length();
            if( distance < scaling.x * avatar_size[0] ) {
              //TODO: correction of avatars position.
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

    // When a transition takes place navigationinfo negates external
    // use of setValue of the position field of a viewpoint
    if( linear_interpolate ) {
      H3DTime current_time = TimeStamp();
      H3DDouble elapsed_time = current_time - start_time;
      if( elapsed_time < total_time ) {
        H3DDouble interpolation = elapsed_time / total_time;
        vp_pos = start_position +
          move_direction * interpolation;
        vp->position->setValue( vp_pos );
        vp_orientation =
          start_orientation.slerp( goal_orientation, (H3DFloat)interpolation );
        vp->orientation->setValue( vp_orientation );
      }
      else {
        linear_interpolate = false;
        transitionComplete->setValue( true, id );
        vp_pos = goal_position;
        vp->position->setValue( vp_pos );
        vp_orientation = goal_orientation;
        vp->orientation->setValue( vp_orientation );
      }
    }
  }
  else {
    Console(3) << "Warning: The field avatarSize( " << avatarSize->getName()
               << " ) in NavigationInfo node( "
				       << getName() << " ) is empty."
               << " No collision with avatar will be detected " << endl;
  }
}