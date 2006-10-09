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
/// \file HAPIDevice.cpp
/// \brief cpp file for HAPIDevice
///
//
//////////////////////////////////////////////////////////////////////////////

#include <HAPIDevice.h>
#include <PhantomHapticsDevice.h>
#include <X3DGeometryNode.h>
#include <DeviceInfo.h>

using namespace H3D;

H3DNodeDatabase HAPIDevice::database( "HAPIDevice", 
                                      &newInstance< HAPIDevice >,
                                      typeid( HAPIDevice ),
                                      &H3DHapticsDevice::database ); 

namespace HAPIDeviceInternals {
  FIELDDB_ELEMENT( HAPIDevice, proxyRadius, INPUT_OUTPUT );
}


// Callback function for changing force effect that are to be rendered.
PeriodicThread::CallbackCode HAPIDevice::changeForceEffects( void *_data ) {
    void * * data = static_cast< void * * >( _data );
    HAPIDevice *hd = 
      static_cast< HAPIDevice * >( data[0] );
    HapticEffectVector* effects  = 
      static_cast< HapticEffectVector * >( data[1] );
    //hd->getLastForceEffects().swap( hd->getCurrentForceEffects() );
    hd->hapi_device->swapEffects( *effects );
    TimeStamp now = TimeStamp();
    hd->last_loop_time = now - hd->last_effect_change;
    hd->last_effect_change = now;
    return PeriodicThread::CALLBACK_DONE;
  }


PeriodicThread::CallbackCode HAPIDevice::changeHapticShapes( void *_data ) {
    void * * data = static_cast< void * * >( _data );
    HAPIDevice *hd = 
      static_cast< HAPIDevice * >( data[0] );
    HapticShapeVector* shapes  = 
      static_cast< HapticShapeVector * >( data[1] );
    //hd->getLastForceEffects().swap( hd->getCurrentForceEffects() );
    hd->hapi_device->swapShapes(*shapes );
    //TimeStamp now = TimeStamp();
    //hd->last_loop_time = now - hd->last_effect_change;
    //hd->last_effect_change = now;
    return PeriodicThread::CALLBACK_DONE;
  }

/// Constructor.
HAPIDevice::HAPIDevice( 
               Inst< SFVec3f > _devicePosition,
               Inst< SFRotation > _deviceOrientation,
               Inst< TrackerPosition > _trackerPosition        ,
               Inst< TrackerOrientation > _trackerOrientation  ,
               Inst< PosCalibration  > _positionCalibration    ,
               Inst< OrnCalibration  > _orientationCalibration ,
               Inst< SFVec3f         > _proxyPosition          ,
               Inst< WeightedProxy   > _weightedProxyPosition  ,     
               Inst< SFFloat         > _proxyWeighting         ,
               Inst< SFBool          > _mainButton  ,
               Inst< SFVec3f         > _force      ,
               Inst< SFVec3f         > _torque     ,
               Inst< SFInt32         > _inputDOF               ,
               Inst< SFInt32         > _outputDOF              ,
               Inst< SFInt32         > _hapticsRate            ,
               Inst< SFNode          > _stylus                 ,
               Inst< SFBool          > _initialized,
               Inst< SFFloat         > _proxyRadius ):
  H3DHapticsDevice( _devicePosition, _deviceOrientation, _trackerPosition,
                    _trackerOrientation, _positionCalibration, 
                    _orientationCalibration, _proxyPosition,
                    _weightedProxyPosition, _proxyWeighting, _mainButton,
                    _force, _torque, _inputDOF, _outputDOF, _hapticsRate,
                    _stylus, _initialized ),
  proxyRadius( _proxyRadius ),
  nr_haptics_loops( 0 ),
  hapi_device( new PhantomHapticsDevice ) {

  type_name = "HAPIDevice";  
  database.initFields( this );

  proxyRadius->setValue( 0.01 );
  // trackerPosition->route( proxyPosition, id );
}


void HAPIDevice::initDevice() {
  if( !initialized->getValue() ) {
    if( hapi_device.get() ) {
      hapi_device->initDevice();
      hapi_device->enableDevice();
      // TODO TEMP:
      //hapi_device->setPositionCalibration( positionCalibration->getValue( ) );
      RuspiniRenderer *rr = 
        dynamic_cast< RuspiniRenderer * >(hapi_device->getHapticsRenderer());
      rr->setProxyRadius( proxyRadius->getValue() * 1e3 );
      
    }
    H3DHapticsDevice::initDevice();
  }
}

void HAPIDevice::disableDevice() {
  if( thread )
    delete thread;
  thread = NULL;
  if( hapi_device.get() ) {
    hapi_device->disableDevice();
    hapi_device->releaseDevice();
  }
  H3DHapticsDevice::disableDevice();
}

void HAPIDevice::renderEffects( 
                         const HapticEffectVector &effects ) {
  hapi_device->setEffects( effects );
}

void HAPIDevice::renderShapes( 
                         const HapticShapeVector &shapes ) {
  hapi_device->setShapes( shapes );
}

void HAPIDevice::updateDeviceValues() {
  H3DHapticsDevice::updateDeviceValues();
  TimeStamp now = TimeStamp();
  TimeStamp dt = now - last_update_values;
  last_update_values = now;
  H3DInt32 hr = (H3DInt32)( nr_haptics_loops / dt );
  nr_haptics_loops = 0;
  hapticsRate->setValue( hr, id );
  if( hapi_device.get() ) {
    HAPIHapticsDevice::DeviceValues dv = hapi_device->getRawDeviceValues();
    // convert to metres
    devicePosition->setValue( (Vec3f)dv.position * 1e-3, id);
    deviceOrientation->setValue( dv.orientation, id);
    force->setValue( (Vec3f)dv.force, id);
    torque->setValue( (Vec3f)dv.torque, id);
    hapi_device->setPositionCalibration( positionCalibration->rt_pos_calibration );
    hapi_device->setOrientationCalibration( orientationCalibration->rt_orn_calibration );
    //cerr << deviceOrientation->getValue() << endl;
    //cerr << trackerOrientation->getValue() << endl;
    RuspiniRenderer *ruspini = dynamic_cast< RuspiniRenderer * >( hapi_device->getHapticsRenderer() );
    if( ruspini ) {
      proxyPosition->setValue( (Vec3f)(ruspini->getProxyPosition() * 1e-3), id );

      // find the index of the haptics device
      DeviceInfo *di = DeviceInfo::getActive();
      int device_index = -1;
      if( di ) {
        const NodeVector &devices = di->device->getValue();
        for( unsigned int i = 0; i < devices.size(); i++ ) {
          if( (Node *)devices[i] == this )
            device_index = i;
        }
      }
      assert( device_index != -1 );

      RuspiniRenderer::Contacts contacts = ruspini->getContacts();
      for( RuspiniRenderer::Contacts::iterator i = contacts.begin();
           i != contacts.end(); i++ ) {
        X3DGeometryNode *geom = static_cast< X3DGeometryNode * >((*i).first->userdata );
      
        // make sure all fields have the right size
        if( device_index > (int)geom->force->size() -1 )
          geom->force->resize( device_index + 1, Vec3f( 0, 0, 0 ), geom->id );
        if( device_index > (int)geom->contactPoint->size() -1 )
          geom->contactPoint->resize( device_index + 1, Vec3f( 0, 0, 0 ), geom->id );
        if( device_index > (int)geom->contactNormal->size() -1 )
          geom->contactNormal->resize( device_index + 1, Vec3f( 1, 0, 0 ), geom->id );
        if( device_index > (int)geom->isTouched->size() -1 )
          geom->isTouched->resize( device_index + 1, false, geom->id );
        
        HAPISurfaceObject::ContactInfo ci = (*i).second;

        // TODO: shpould be able to do it in a faster/better way.
        Matrix4d global_to_local = (*i).first->transform.inverse();
        Matrix3d global_vec_to_local = global_to_local.getRotationPart();

        Vec3f cp( global_to_local * (ci.globalSurfaceContactPoint() *1e-3) );

        if( geom->contactPoint->getValueByIndex( device_index ) != cp ) 
          geom->contactPoint->setValue( device_index, cp, geom->id ); 

        Vec3f n( global_vec_to_local * ci.y_axis );

        if( geom->contactNormal->getValueByIndex( device_index ) != n ) 
          geom->contactNormal->setValue( device_index, n, geom->id ); 
        
        Vec3f f( global_vec_to_local * ci.force_global );

        if( geom->force->getValueByIndex( device_index ) != f ) 
          geom->force->setValue(device_index, f, geom->id ); 

        bool contact_last_time = 
          geom->isTouched->getValueByIndex( device_index );
        if( !contact_last_time ) 
          geom->isTouched->setValue( device_index, true, geom->id );
      }

      for( RuspiniRenderer::Contacts::iterator j = last_contacts.begin();
           j != last_contacts.end(); j++ ) {
        bool still_in_contact = false;
        for( RuspiniRenderer::Contacts::iterator i = contacts.begin();
             i != contacts.end(); i++ ) {
          cerr << (*i).first->userdata << " " << (*j).first->userdata << endl;
          if( (*i).first->userdata == (*j).first->userdata ) {
            still_in_contact = true;
            break;
          }
        }
        if( !still_in_contact ) {
          X3DGeometryNode *geom = 
            static_cast< X3DGeometryNode * >((*j).first->userdata );
          geom->isTouched->setValue( device_index, false, geom->id );
        }
           }
      // TODO: untouch
    
      last_contacts.swap( contacts );
    } else {
      proxyPosition->setValue( trackerPosition->getValue(), id );
    }
    /*
    cerr << "F: " << devicePosition->getValue() * 1e3<< endl;
    cerr << "F: " << trackerPosition->getValue() * 1e3<< endl;
    cerr << "T: " << proxyPosition->getValue() * 1e3 << endl;
    cerr << "W: " << weightedProxyPosition->getValue()* 1e3<< endl;
    */
  }

  //cerr << hr << endl;
  // TODO: lock
  //proxyPosition->setValue( (Vec3f)proxy_position, id );
}




