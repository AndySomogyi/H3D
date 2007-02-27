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
/// \file H3DNavigationDevices.cpp
/// \brief CPP file for H3DNavigationDevices, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "H3DNavigationDevices.h"

using namespace H3D;

list<H3DNavigationDevices *> H3DNavigationDevices::h3dnavigations;

H3DNavigationDevices::H3DNavigationDevices() : shouldGetInfo( new SFBool ) {
  shouldGetInfo->setValue( false );
  h3dnavigations.push_back( this );
}

void H3DNavigationDevices::resetAll() {
  move_dir = Vec3f();
  rel_rot = Rotation();
}

MouseNavigation::MouseNavigation() : calculateMouseMoveInfo( new CalculateMouseMoveInfo ), mouseSensor( new MouseSensor() ) {
  calculateMouseMoveInfo->the_owner = this;
  calculateMouseMoveInfo->setValue( false );
  mouseSensor->leftButton->routeNoEvent( calculateMouseMoveInfo );
  mouseSensor->motion->routeNoEvent( calculateMouseMoveInfo );
  calculateMouseMoveInfo->route( shouldGetInfo );
}

void MouseNavigation::resetAll() {
  H3DNavigationDevices::resetAll();
  calculateMouseMoveInfo->setValue( false );
}

void MouseNavigation::CalculateMouseMoveInfo::update( ) {
  bool button_pressed = static_cast< SFBool * >(routes_in[0])->getValue();
  Vec2f motion = static_cast< SFVec2f * >(routes_in[1])->getValue();
  string nav_type = the_owner->getNavType();
  
  
  if( event.ptr == routes_in[0] ) {
    if( nav_type == "LOOKAT" ) {
      if( button_pressed ) {
         Vec2f temp_pos = the_owner->mouseSensor->position->getValue();
         the_owner->move_dir = Vec3f( temp_pos.x, temp_pos.y, 0 );
         value = true;
      }
    }
    else {
      the_owner->move_dir = Vec3f();
      the_owner->rel_rot = Rotation();
      value = false;
    }
  }
  else if( button_pressed ) {
    Vec2f perp = Vec2f( -motion.y, -motion.x );
    perp.normalize();
    if( nav_type == "EXAMINE" ) {
      the_owner->rel_rot *= Rotation( perp.x, perp.y, 0, motion.length() * 0.01f );
      the_owner->move_dir = Vec3f();
    }
    else if( nav_type == "WALK" || nav_type == "FLY" ) {
      H3DFloat abs_x = H3DAbs( motion.x );
      if( abs_x > Constants::f_epsilon ) {
        the_owner->rel_rot *= Rotation( 0, -motion.x / abs_x, 0, abs_x * 0.01f );
      }
      else
        the_owner->rel_rot *= Rotation();
      the_owner->move_dir = Vec3f();
    }
    else {
      the_owner->rel_rot = Rotation();
      the_owner->move_dir = Vec3f();
    }
    value = true;
  }
  else {
    if( nav_type != "LOOKAT" ) {
      the_owner->move_dir = Vec3f();
      the_owner->rel_rot = Rotation();
      value = false;
    }
  }
}

KeyboardNavigation::KeyboardNavigation() : calculateKeyboardMoveInfo( new CalculateKeyboardMoveInfo ), keySensor( new KeySensor() ) {
  calculateKeyboardMoveInfo->the_owner = this;
  calculateKeyboardMoveInfo->setValue( false );
  keySensor->actionKeyPress->route( calculateKeyboardMoveInfo );
  keySensor->actionKeyRelease->route( calculateKeyboardMoveInfo );
  calculateKeyboardMoveInfo->route( shouldGetInfo );
}

void KeyboardNavigation::resetAll() {
}

void KeyboardNavigation::CalculateKeyboardMoveInfo::update( ) {
  if( event.ptr == routes_in[0] ) {
    int key = static_cast< SFInt32 * >(routes_in[0])->getValue();
    switch( key ) {
      case KeySensor::UP: {
        upPressed = true;
        break;
      }
      case KeySensor::DOWN: {
        downPressed = true;
        break;
      }
      case KeySensor::LEFT: {
        leftPressed = true;
        break;
      }
      case KeySensor::RIGHT: {
        rightPressed = true;
        break;
      }
      default: { }
    }
  }
  else if( event.ptr == routes_in[1] ) {
    int key = static_cast< SFInt32 * >(routes_in[1])->getValue();
    switch( key ) {
      case KeySensor::UP: {
        upPressed = false;
        break;
      }
      case KeySensor::DOWN: {
        downPressed = false;
        break;
      }
      case KeySensor::LEFT: {
        leftPressed = false;
        break;
      }
      case KeySensor::RIGHT: {
        rightPressed = false;
        break;
      }
      default: { }
    }
  }
  Vec3f temp_move_dir;
  Rotation temp_rel_rot;
  bool temp_value = false;
  string nav_type = the_owner->getNavType();
  if( nav_type == "WALK" || nav_type == "FLY" ) {
    if( upPressed ) {
      temp_move_dir += Vec3f( 0, 0, -1 );
      temp_rel_rot = Rotation();
      temp_value = true;
    }
    if( downPressed ) {
      temp_move_dir += Vec3f( 0, 0, 1 );
      temp_rel_rot = Rotation();
      temp_value = true;
    }
    if( leftPressed ) {
      temp_move_dir += Vec3f( -1, 0, 0 );
      temp_rel_rot = Rotation();
      temp_value = true;
    }
    if( rightPressed ) {
      temp_move_dir += Vec3f( 1, 0, 0 );
      temp_rel_rot = Rotation();
      temp_value = true;
    }
  }
  else if( nav_type == "EXAMINE" ) {
    if( upPressed ) {
      temp_move_dir = Vec3f( 0, 0, 0 );
      temp_rel_rot *= Rotation( 1, 0, 0, 0.01f );
      temp_value = true;
    }
    if( downPressed ) {
      temp_move_dir = Vec3f( 0, 0, 0 );
      temp_rel_rot *= Rotation( 1, 0, 0, -0.01f );
      temp_value = true;
    }
    if( leftPressed ) {
      temp_move_dir = Vec3f( 0, 0, 0 );
      temp_rel_rot *= Rotation( 0, 1, 0, 0.01f );
      temp_value = true;
    }
    if( rightPressed ) {
      temp_move_dir = Vec3f( 0, 0, 0 );
      temp_rel_rot *= Rotation( 0, 1, 0, -0.01f );
      temp_value = true;
    }
  }
  the_owner->move_dir = temp_move_dir;
  the_owner->rel_rot = temp_rel_rot;
  value = temp_value;
}
