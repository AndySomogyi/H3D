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
/// \file H3DHapticsDevice.h
/// \brief Header file for H3DHapticsDevice.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DHAPTICSDEVICE_H__
#define __H3DHAPTICSDEVICE_H__

// H3D includes
#include <FieldTemplates.h>
#include <SFNode.h>
#include <SFRotation.h>
#include <SFFloat.h>
#include <SFInt32.h>
#include <SFBool.h>
#include <SFMatrix4f.h>
#include <SFVec3f.h>
#include <MFVec3f.h>
#include <H3DHapticsRendererNode.h>

// H3DUtil includes
#include <Threads.h>

// HAPI includes
#include <HAPIHapticShape.h>
#include <HAPIHapticsDevice.h>
#include <HAPIForceEffect.h>


namespace H3D {

  /// \ingroup AbstractNodes
  /// \class H3DHapticsDevice
  /// Base class for all haptic devices. It contains information about the 
  /// haptics device, e.g. the position and orientation of the device, the
  /// forces being rendered and supported degrees of freedom. It also
  /// contains functions for rendering HapticObject instances on the device.
  /// Subclasses should override renderObjects() to render
  /// the objects on the H3DHapticsDevice.
  /// 
  /// \par Internal routes:
  /// \dotfile H3DHapticsDevice.dot
  class H3DAPI_API H3DHapticsDevice: public Node {
  protected:
    auto_ptr< HAPI::HAPIHapticsDevice > hapi_device;

  public:

    typedef HAPI::HAPIHapticsDevice::ErrorCode ErrorCode;
    typedef HAPI::HAPIHapticsDevice::DeviceState DeviceState;

    /// Saves the value of the field in a variable that can be accessed
    /// from the realtime loop.
    class H3DAPI_API PosCalibration: 
      public SFMatrix4f {
    public:
      /// Update the rt variables.
      virtual void setValue( const Matrix4f &v, int id = 0 ) {
        SFMatrix4f::setValue( v, id );
        rt_pos_calibration = value;
        rt_inv_pos_rotation = rt_pos_calibration.getRotationPart().inverse();
      }

      /// Update the rt variables.
      inline virtual void update() {
        SFMatrix4f::update();
        rt_pos_calibration = value;
        rt_inv_pos_rotation = rt_pos_calibration.getRotationPart().inverse();
      }

      /// The value of the PosCalibration field to be accessed from 
      /// the realtime loop.
      Matrix4d rt_pos_calibration;
      
      /// The inverse of the part of rt_pos_calibration that involves
      /// rotation.
      Matrix3d rt_inv_pos_rotation;
    };

    /// Saves the value of the field in a variable that can be accessed
    /// from the realtime loop.
    class H3DAPI_API OrnCalibration: 
      public SFRotation {
    public:
      /// Update the rt variable.
      inline virtual void setValue( const Rotation &v, int id = 0 ) {
        SFRotation::setValue( v, id );
        rt_orn_calibration = value;
      }

      /// Update the rt variable.
      virtual void update() {
        SFRotation::update();
        rt_orn_calibration = value;
      }

      /// The value of the OrnCalibration field to be accessed from 
      /// the realtime loop.
      Rotation rt_orn_calibration;
    };

    /// The TrackerPosition field updates itself from the device_position
    /// and position_calibration fields. 
    /// TrackerPosition = positionCalibration * devicePosition 
    ///
    /// - routes_in[0] is the positionCalibration field
    /// - routes_in[1] is the devicePosition field
    ///
    class H3DAPI_API TrackerPosition: 
      public TypedField< SFVec3f, Types< SFMatrix4f, SFVec3f > > {
      
      /// value = position_calibration * device_position.
      virtual void update() {
        H3DHapticsDevice *hd = static_cast< H3DHapticsDevice *>(owner);
        Matrix4f m;
        if( hd->followViewpoint->getValue() ) {
          m = hd->adjustedPositionCalibration->getValue();
        }
        else {
          m = static_cast< SFMatrix4f * >( routes_in[0] )->getValue();
        }
        const Vec3f &d_pos = 
          static_cast< SFVec3f * >( routes_in[1] )->getValue();
        value = m * d_pos;
      }
    };

    /// The TrackerOrientation field updates itself from the device_orientation
    /// and orientation_calibration fields. 
    /// TrackerOrientation = orientationCalibration * deviceOrientation 
    ///
    /// - routes_in[0] is the orientationCalibration
    /// - routes_in[1] is the deviceOrientaiton
    ///
    class H3DAPI_API TrackerOrientation: 
      public TypedField< SFRotation, Types< SFRotation, SFRotation > > {

      virtual void update() {
        H3DHapticsDevice *hd = static_cast< H3DHapticsDevice *>(owner);
        Rotation cal;
        if( hd->followViewpoint->getValue() ) {
          cal = hd->adjustedOrnCalibration->getValue();
        }
        else {
          cal  = 
          static_cast< SFRotation * >( routes_in[0] )->getValue();
        }

        const Rotation &d_orn = 
          static_cast< SFRotation * >( routes_in[1] )->getValue();
        value = cal * d_orn;
      }
  
    };

    /// The WeightedProxy field updates to be a value between the proxy position
    /// and tracker position depending on the value of a weighting factor. 
    /// If the weightting factor is 1 if will be the proxy position, if 0 the
    /// tracker position. 
    /// 
    /// - routes_in[0] is the proxy position
    /// - routes_in[1] is the tracker position.
    /// - routes_in[2] is the weighting factor.
    ///
    class H3DAPI_API WeightedProxy:
      public TypedField< SFVec3f, Types< SFVec3f, SFVec3f, SFFloat > > {
      virtual void update() {
        const Vec3f &proxy_pos   = 
          static_cast< SFVec3f * >( routes_in[0] )->getValue();
        const Vec3f &tracker_pos = 
          static_cast< SFVec3f * >( routes_in[1] )->getValue();
        H3DFloat weighting = 
          static_cast< SFFloat * >( routes_in[2] )->getValue();
        value =  tracker_pos + weighting * ( proxy_pos - tracker_pos );
      }
    };

    /// SFHapticsRendererNode extends TypedSFNode< H3DHapticsRendererNode >
    /// in order to change the haptics renderer for the used HAPIHapticsDevice
    /// when changing H3DHapticsRendererNode.
    class H3DAPI_API SFHapticsRendererNode: 
      public TypedSFNode< H3DHapticsRendererNode > {
      virtual void onAdd( Node *n ) {
        TypedSFNode< H3DHapticsRendererNode >::onAdd( n );
        H3DHapticsRendererNode *renderer = 
          static_cast< H3DHapticsRendererNode * >( n );
        H3DHapticsDevice *device = 
          static_cast< H3DHapticsDevice * >( getOwner() );
        if( renderer && device->hapi_device.get() ) {
          for( unsigned int i = 0; i < device->hapi_device->nrLayers(); i++ )
            device->hapi_device->setHapticsRenderer( 
                                     renderer->getHapticsRenderer( i ) );
        }
      }
      virtual void onRemove( Node *n ) {
        H3DHapticsRendererNode *renderer = static_cast< H3DHapticsRendererNode * >( n );
        H3DHapticsDevice *device = static_cast< H3DHapticsDevice * >( getOwner() );
        if( renderer && device->hapi_device.get() ) {
          for( unsigned int i = 0; i < device->hapi_device->nrLayers(); i++ )
            device->hapi_device->setHapticsRenderer( NULL, i );
        }
        TypedSFNode< H3DHapticsRendererNode >::onRemove( n );
      }

    };


    /// SetEnabled specializes SFBool to go into reset mode when a true event
    /// is received.
    class H3DAPI_API SetEnabled: public OnValueChangeSField< SFBool > {
      virtual void onValueChange( const bool &v ) {
        H3DHapticsDevice *hd = 
          static_cast< H3DHapticsDevice * >( getOwner() );
        if( v ) hd->enableDevice();
        else hd->disableDevice();
      }
    };

    /// The MainButton class masks out the bit 0 from the incoming integer.
    ///
    /// inputs[0] is the buttons field
    class H3DAPI_API MainButton: public TypedField< SFBool, SFInt32 > {
      virtual void update() {
        H3DInt32 buttons   = 
          static_cast< SFInt32 * >( routes_in[0] )->getValue();
        value = (buttons & 0x01) != 0;
      }
    }; 

    /// The SecondaryButton class masks out bit 1 from the incoming integer.
    /// inputs[0] is the buttons field
    class H3DAPI_API SecondaryButton: public TypedField< SFBool, SFInt32 > {
      virtual void update() {
        H3DInt32 buttons   = 
          static_cast< SFInt32 * >( routes_in[0] )->getValue();
        value = (buttons & 0x10) != 0;
      }
    }; 

    /// Constructor.
    H3DHapticsDevice( Inst< SFVec3f         > _devicePosition         = 0,
                      Inst< SFRotation      > _deviceOrientation      = 0,
                      Inst< TrackerPosition > _trackerPosition        = 0,
                      Inst< TrackerOrientation > _trackerOrientation  = 0,
                      Inst< PosCalibration  > _positionCalibration    = 0,
                      Inst< OrnCalibration  > _orientationCalibration = 0,
                      Inst< SFVec3f         > _proxyPosition          = 0,
                      Inst< WeightedProxy   > _weightedProxyPosition  = 0,     
                      Inst< SFFloat         > _proxyWeighting         = 0,
                      Inst< MainButton      > _main_button            = 0,
                      Inst< SecondaryButton > _secondary_button       = 0,
                      Inst< SFInt32         > _buttons                = 0,
                      Inst< SFVec3f         > _force                  = 0,
                      Inst< SFVec3f         > _torque                 = 0,
                      Inst< SFInt32         > _inputDOF               = 0,
                      Inst< SFInt32         > _outputDOF              = 0,
                      Inst< SFInt32         > _hapticsRate            = 0,
                      Inst< SFNode          > _stylus                 = 0,
                      Inst< SFHapticsRendererNode > _hapticsRenderer  = 0,
                      Inst< MFVec3f         > _proxyPositions         = 0,
                      Inst< SFBool          > _followViewpoint        = 0 );

    /// Destuctor.
    virtual ~H3DHapticsDevice() {
      releaseDevice();
    }

    /// Get the proxy position from the previous loop.
    inline Vec3f getPreviousProxyPosition() { return previuos_proxy_pos; }

    /// Returns the default xml containerField attribute value.
    /// For this node it is "appearance".
    virtual string defaultXMLContainerField() {
      return "device";
    }

    /// Does all the initialization needed for the device before starting to
    /// use it.
    virtual ErrorCode initDevice();

    /// Perform cleanup and let go of all device resources that are allocated.
    /// After a call to this function no haptic rendering can be performed on
    /// the device until the initDevice() function has been called again.
    virtual ErrorCode releaseDevice();

    /// Enable the device. Positions can be read and force can be sent.
    virtual ErrorCode enableDevice();

    /// Temporarily disable the device. Forces sent will be ignored and
    /// positions and orientation will stay the same as previous values.
    virtual ErrorCode disableDevice();

    /// Get the thread that is used to run this haptics device.
    inline H3DUtil::PeriodicThreadBase *getThread() { 
      if( hapi_device.get() ) {
        return hapi_device->getThread();
      } else {
        return NULL;
      }
    }

    /// This function is used to transfer device values, such as position, 
    /// button status etc from the realtime loop to the fields of H3DHapticsDevice,
    /// and possible vice versa.
    virtual void updateDeviceValues();

    /// This function is called at the start of each scenegraph loop before any
    /// calls to other HapticDevice functions and can be used to perform any 
    /// necessary operation that are needed for the other calls to function
    /// properly.
    virtual void preRender() {}

    /// This function is called at the end of each scenegraph loop after all
    /// calls to other HapticDevice functions and can be used to perform any 
    /// necessary operation that are needed.
    virtual void postRender() {}

    /// Perform haptic rendering for the given HapticShape instances. 
    /// HapticShape objects that are to be be rendered haptically must be 
    /// rendered with this function each scenegraph loop. 
    /// \param objects The haptic shapes to render.
    /// \param layer The haptic layer to render them in.
    virtual void renderShapes( const HapticShapeVector &shapes, 
                               unsigned int layer = 0 );

    /// Perform haptic rendering for the given HAPIForceEffect instances. 
    /// HAPIForceEffect objects that are to be be rendered haptically must
    /// be rendered with this function each scenegraph loop.
    /// \param objects The haptic objects to render.
    ///
    virtual void renderEffects( const HapticEffectVector &effects );
    
    /// The position of the device given in the coordinate system of the 
    /// device.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_devicePosition.dot
    auto_ptr< SFVec3f >    devicePosition;

    /// The orientation of the device given in the coordinate system of the 
    /// device. Only applicable if the device supports orientation as input.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_deviceOrientation.dot
    auto_ptr< SFRotation > deviceOrientation;
    
    /// The position of the device in the world coordinates of the API.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_trackerPosition.dot
    auto_ptr< TrackerPosition >    trackerPosition;

    /// The orientation of the device in the world coordinates of the API.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_trackerOrientation.dot
    auto_ptr< TrackerOrientation > trackerOrientation;

    /// The calibration matrix between devicePosition and trackerPosition.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Unit matrix \n
    /// 
    /// \dotfile H3DHapticsDevice_positionCalibration.dot
    auto_ptr< PosCalibration > positionCalibration;
    
    /// The calibration matrix between devicePosition and trackerPosition
    /// adjusted with the movement of the viewpoint.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Unit matrix \n
    /// 
    /// \dotfile H3DHapticsDevice_adjustedPositionCalibration.dot
    auto_ptr< PosCalibration > adjustedPositionCalibration;

    /// The calibration rotation between deviceOrientation and 
    /// trackerOrientation.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Rotation( 1, 0, 0, 0 ) \n
    /// 
    /// \dotfile H3DHapticsDevice_orientationCalibration.dot
    auto_ptr< OrnCalibration > orientationCalibration;

    /// The calibration rotation between deviceOrientation and 
    /// trackerOrientation adjusted with the movement of the viewpoint.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> Rotation( 1, 0, 0, 0 ) \n
    /// 
    /// \dotfile H3DHapticsDevice_orientationCalibration.dot
    auto_ptr< OrnCalibration > adjustedOrnCalibration;

    /// The position of the proxy used in the haptic rendering(layer 0). 
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_proxyPosition.dot
    auto_ptr< SFVec3f >   proxyPosition;

    /// A weighted position between proxyPosition and trackerPosition.
    /// The weighting factor is the proxyWeigting field. By default
    /// this is the field that is used to display the stylus graphically.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_weightedProxyPosition.dot
    auto_ptr< SFVec3f >   weightedProxyPosition;

    /// The weighting between proxyPosition and trackerPosition when
    /// calculating weightedProxyPosition. The value should be 
    /// between 0 and 1, with 1 meaning that the weightedProxyPosition
    /// is the proxyPosition and 0 that it is the trackerPosition.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> 0.95 \n
    /// 
    /// \dotfile H3DHapticsDevice_proxyWeighting.dot
    auto_ptr< SFFloat >   proxyWeighting;

    /// The state of the main button(button 0). 
    /// true means that the button is pressed.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_mainButton.dot
    auto_ptr< MainButton >   mainButton;

    /// The state of the secondary button (button 1).
    /// true means that the button is pressed.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_mainButton.dot
    auto_ptr< SecondaryButton >   secondaryButton;

    /// The state of all buttons. Bit 0 is button 0, bit 1 is button 1,..
    /// A 1 in the bit position indicates that the button is pressed.
    /// Use mainButton and secondaryButton fields for quick access to
    /// button 0 and 1
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_buttons
    auto_ptr< SFInt32 > buttons;

    /// The approximation of the force that has been rendered during the last 
    /// scenegraph loop.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_force.dot
    auto_ptr< SFVec3f >    force;

    /// The approximation of the torque that has been rendered during the last 
    /// scenegraph loop.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_torque.dot
    auto_ptr< SFVec3f >    torque;

    /// The degrees of freedom supported as input.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_inputDOF.dot
    auto_ptr< SFInt32 >   inputDOF;

    /// The degrees of freedom supported as output, i.e. forces and torques.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_outputDOF.dot
    auto_ptr< SFInt32 >   outputDOF;

    /// The update rate of the servoloop of the H3DHapticsDevice.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_hapticsRate.dot
    auto_ptr< SFInt32 >   hapticsRate;
    
    /// The Node used as the visual representation of the stylus.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_stylus.dot
    auto_ptr< SFNode > stylus;

    /// true if the device is initialized and ready for calls to 
    /// updateDeviceValues() and renderObjects(). If not initDevice() will have
    /// to be called in order to initialize it before calling those functions.
    ///
    /// <b>Access type:</b> outputOnly \n
    /// <b>Default value:</b> false \n
    /// 
    /// \dotfile H3DHapticsDevice_initialized.dot
    auto_ptr< SFBool > initialized;

    /// Specifies the haptics rendering algorithm to use to generate forces
    /// from geometric shapes.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// 
    /// \dotfile H3DHapticsDevice_hapticsRenderer.dot
    auto_ptr< SFHapticsRendererNode > hapticsRenderer;

    /// The positions of the proxies for each layer used in haptic
    /// rendering(layer 0). 
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_proxyPositions.dot
    auto_ptr< MFVec3f >   proxyPositions;

    /// Enable/disable the device. A disabled device does not update its 
    /// positions and does not generate any forces.
    ///
    /// <b>Access type:</b> inputOnly \n
    /// 
    /// \dotfile H3DHapticsDevice_enabled.dot
    auto_ptr< SetEnabled > set_enabled;

    /// true if the device is enabled, e.g. positions and forces are updated 
    /// and sent
    ///
    /// <b>Access type:</b> outputOnly \n
    /// <b>Default value:</b> false \n
    /// 
    /// \dotfile H3DHapticsDevice_enabled.dot
    auto_ptr< SFBool > enabled;

    /// true if the device should follow the viewpoint.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> true \n
    /// 
    /// \dotfile H3DHapticsDevice_followViewpoint.dot
    auto_ptr< SFBool > followViewpoint;
    
    /// Node database entry
    static H3DNodeDatabase database;
  protected:
    Vec3f previuos_proxy_pos;

    // true if we have a default_vp_pos
    bool vp_initialized;

    // the position in global coordinates of the viewpoint which should
    // be used as default reference if the haptic device should follow
    // viewpoint movement.
    Matrix4f default_vp_pos_mtx;
    Matrix4f default_vp_pos_mtx_mm;
    Vec3f default_vp_pos;

    // the orientation in global coordinates of the viewpoint which should
    // be used as default reference if the haptic device should follow
    // viewpoint movement.
    Matrix4f default_vp_orn_mtx;

    // The time of the last call to updateDeviceValues.
    TimeStamp last_update_values;

    /// The time for the last call to the changeForceEffects callback 
    /// function.
    TimeStamp last_effect_change;
    
    /// The time between the previous two calls to changeForceEffects.
    TimeStamp last_loop_time;

    /// Callcack function to transfer the force effect vector to the 
    /// haptics loop.
    static H3DUtil::PeriodicThread::CallbackCode changeForceEffects( void *_data ); 
    /// Callcack function to transfer the shapes vector for the 
    /// haptics loop.
    static H3DUtil::PeriodicThread::CallbackCode changeHapticShapes( void *_data ); 

    // TODO
    HAPI::HAPIHapticsRenderer::Contacts last_contacts;

  };
}

#endif
