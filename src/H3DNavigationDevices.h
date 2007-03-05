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
/// \file H3DNavigationDevices.h
/// \brief Header file for H3DNavigationDevices, classes for navigation
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DNAVIGATIONDEVICES_H__
#define __H3DNAVIGATIONDEVICES_H__

#include "MouseSensor.h"
#include "KeySensor.h"
#include "SFVec3f.h"
#include "SFRotation.h"

namespace H3D {
  
  class H3DAPI_API H3DNavigationDevices {
  public:

    /// Constructor.
    H3DNavigationDevices();

    virtual ~H3DNavigationDevices() {
      h3dnavigations.remove( this );
    }

    static bool getMoveInfo( Vec3f &translationSum, Rotation &rotationSum,
                             Vec3f &center_of_rot_sum, bool &use_center_sum ) {
      bool somethingmoved = false;
      int centerCounter = 0;
      use_center_sum = false;
      for( list< H3DNavigationDevices * >::iterator i = h3dnavigations.begin();
           i != h3dnavigations.end(); i++ ) {
        if( (*i)->shouldGetInfo->getValue() ) {
          translationSum += (*i)->move_dir;
          rotationSum = rotationSum * (*i)->rel_rot;
          if( (*i)->use_center ) {
            use_center_sum = true;
            centerCounter++;
            center_of_rot_sum += (*i)->center_of_rot;
          }
          (*i)->resetAll();
          somethingmoved = true;
        }
      }
      if( use_center_sum )
        center_of_rot_sum /= centerCounter;
      return somethingmoved;
    }

    virtual void resetAll();
    
    static void setNavTypeForAll( string &_nav_type ) {
      for( list< H3DNavigationDevices * >::iterator i = h3dnavigations.begin();
           i != h3dnavigations.end(); i++ ) {
          (*i)->setNavType( _nav_type );
      }
    }

    void setNavType( string &_nav_type ) {
      nav_type = _nav_type;
    }

    string getNavType( ) {
      return nav_type;
    }

    Vec3f move_dir;
    Rotation rel_rot;
    Vec3f center_of_rot;
    bool use_center;

  protected:
    auto_ptr< SFBool > shouldGetInfo;
    static list< H3DNavigationDevices * > h3dnavigations;
    string nav_type;
  };


  /// Take care of mouseNavigation
  class H3DAPI_API MouseNavigation : public H3DNavigationDevices {
  public:

    class CalculateMouseMoveInfo :
      public AutoUpdate< TypedField< SFBool, Types< SFBool, SFVec2f > > > {
    public:
      virtual void update();
      MouseNavigation *the_owner;
    };
#ifdef __BORLANDC__
    friend class CalculateMouseMoveInfo;
#endif

    /// Constructor.
    MouseNavigation();

    virtual void resetAll();

  protected:
    auto_ptr< CalculateMouseMoveInfo > calculateMouseMoveInfo;
    auto_ptr< MouseSensor > mouseSensor;
  };

  class H3DAPI_API KeyboardNavigation : public H3DNavigationDevices {
  public:

    class CalculateKeyboardMoveInfo :
      public AutoUpdate< TypedField< SFBool, Types< SFInt32, SFInt32 > > > {
    public:
      CalculateKeyboardMoveInfo() {
        upPressed = downPressed = leftPressed = rightPressed = false;
      }
      virtual void update();

      KeyboardNavigation *the_owner;
    protected:
      bool upPressed;
      bool downPressed;
      bool leftPressed;
      bool rightPressed;
    };
#ifdef __BORLANDC__
    friend class CalculateKeyboardMoveInfo;
#endif

    /// Constructor.
    KeyboardNavigation();

    virtual void resetAll();

  protected:
    auto_ptr< CalculateKeyboardMoveInfo > calculateKeyboardMoveInfo;
    auto_ptr< KeySensor > keySensor;
  };

  class H3DAPI_API HapticDeviceNavigation : public H3DNavigationDevices {
  public:

    class CalculateHapticDeviceMoveInfo :
      public AutoUpdate< TypedField< SFBool, Types< SFBool > > > {

    public:
      CalculateHapticDeviceMoveInfo() {
        button_pressed = false;
      }

      virtual void update();

      HapticDeviceNavigation *the_owner;
    protected:
      bool button_pressed;
      Rotation last_orn;
      Vec3f last_pos;
      Vec3f last_weight_pos;
    };
#ifdef __BORLANDC__
    friend class CalculateHapticDeviceMoveInfo;
#endif

    /// Constructor.
    HapticDeviceNavigation();

    virtual void resetAll();

  protected:
    auto_ptr< CalculateHapticDeviceMoveInfo > calculateHapticDeviceMoveInfo;
  };
}

#endif
