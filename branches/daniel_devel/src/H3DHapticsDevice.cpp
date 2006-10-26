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
/// \file H3DHapticsDevice.cpp
/// \brief cpp file for H3DHapticsDevice
///
//
//////////////////////////////////////////////////////////////////////////////

#include "H3DHapticsDevice.h"
#include <PhantomHapticsDevice.h>
#include <X3DGeometryNode.h>
#include <DeviceInfo.h>
#include <OpenHapticsRenderer.h>
#include <GodObjectRenderer.h>
#include <RuspiniRenderer.h>
using namespace H3D;

H3DNodeDatabase H3DHapticsDevice::database( "H3DHapticsDevice", 
                                         NULL,
                                         typeid( H3DHapticsDevice ) ); 

namespace H3DHapticsDeviceInternals {
  FIELDDB_ELEMENT( H3DHapticsDevice, devicePosition, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, deviceOrientation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, trackerPosition, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, trackerOrientation, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, positionCalibration, INPUT_OUTPUT );
  FIELDDB_ELEMENT( H3DHapticsDevice, orientationCalibration, INPUT_OUTPUT );
  FIELDDB_ELEMENT( H3DHapticsDevice, proxyPosition, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, weightedProxyPosition, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, proxyWeighting, INPUT_OUTPUT );
  FIELDDB_ELEMENT( H3DHapticsDevice, mainButton, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, force, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, torque, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, inputDOF, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, outputDOF, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, hapticsRate, OUTPUT_ONLY );
  FIELDDB_ELEMENT( H3DHapticsDevice, stylus, INPUT_OUTPUT );
  FIELDDB_ELEMENT( H3DHapticsDevice, hapticsRenderer, INPUT_OUTPUT );
}


/// Constructor.
H3DHapticsDevice::H3DHapticsDevice( Inst< SFVec3f         > _devicePosition,
               Inst< SFRotation      > _deviceOrientation      ,
               Inst< TrackerPosition > _trackerPosition        ,
               Inst< TrackerOrientation > _trackerOrientation  ,
               Inst< PosCalibration  > _positionCalibration    ,
               Inst< OrnCalibration  > _orientationCalibration ,
               Inst< SFVec3f         > _proxyPosition          ,
               Inst< WeightedProxy   > _weightedProxyPosition  ,     
               Inst< SFFloat         > _proxyWeighting         ,
               Inst< SFBool          > _mainButton             ,
               Inst< SFVec3f         > _force                  ,
               Inst< SFVec3f         > _torque                 ,
               Inst< SFInt32         > _inputDOF               ,
               Inst< SFInt32         > _outputDOF              ,
               Inst< SFInt32         > _hapticsRate            ,
               Inst< SFNode          > _stylus                 ,
               Inst< SFBool          > _initialized            ,
               Inst< SFHapticsRendererNode > _hapticsRenderer  ):
  devicePosition( _devicePosition ),
  deviceOrientation( _deviceOrientation ),
  trackerPosition( _trackerPosition ),
  trackerOrientation( _trackerOrientation ),
  positionCalibration( _positionCalibration ),
  orientationCalibration( _orientationCalibration ),
  proxyPosition( _proxyPosition ),
  weightedProxyPosition( _weightedProxyPosition ),
  proxyWeighting( _proxyWeighting ),
  mainButton( _mainButton ),
  force( _force ),
  torque( _torque ),
  inputDOF( _inputDOF ),
  outputDOF( _outputDOF ),
  hapticsRate( _hapticsRate ),
  stylus( _stylus ),
  initialized( _initialized ),
  hapticsRenderer( _hapticsRenderer ) {

  type_name = "H3DHapticsDevice";  
  database.initFields( this );

  initialized->setValue( false );
  proxyWeighting->setValue( 0.95f );
  mainButton->setValue( false, id );

  positionCalibration->route( trackerPosition, id );
  devicePosition->route( trackerPosition, id );

  orientationCalibration->route( trackerOrientation, id );
  deviceOrientation->route( trackerOrientation, id );
  
  proxyPosition->route( weightedProxyPosition, id );
  trackerPosition->route( weightedProxyPosition, id );
  proxyWeighting->route( weightedProxyPosition, id );

}

void H3DHapticsDevice::initDevice() {
  if( !initialized->getValue() ) {
    if( hapi_device.get() ) {
      if( hapi_device->initDevice() == HAPI::HAPIHapticsDevice::SUCCESS ) {
        hapi_device->enableDevice();
        // TODO TEMP:
        //hapi_device->setPositionCalibration( positionCalibration->getValue( ) );
//        HAPI::RuspiniRenderer *rr = 
 //         dynamic_cast< HAPI::RuspiniRenderer * >(hapi_device->getHapticsRenderer());
 //       if( rr )
 //          rr->setProxyRadius( proxyRadius->getValue() * 1e3 );
        initialized->setValue( true, id );
      } else {
        Console(4) << hapi_device->getLastErrorMsg() << endl;
      }
    }
  }
}

void H3DHapticsDevice::disableDevice() {
  if( hapi_device.get() ) {
    hapi_device->disableDevice();
    hapi_device->releaseDevice();
  }
  initialized->setValue( false, id );
}

void H3DHapticsDevice::renderEffects( 
                         const HapticEffectVector &effects ) {
  if( hapi_device.get() )
    hapi_device->setEffects( effects );
}

void H3DHapticsDevice::renderShapes( 
                         const HapticShapeVector &shapes ) {
  if( hapi_device.get() ) {
    hapi_device->setShapes( shapes );
    hapi_device->transferObjects();
  }
}

void H3DHapticsDevice::updateDeviceValues() {
   previuos_proxy_pos = proxyPosition->getValue();
  TimeStamp now = TimeStamp();
  TimeStamp dt = now - last_update_values;
  last_update_values = now;
  H3DInt32 hr = (H3DInt32)( nr_haptics_loops / dt );
  nr_haptics_loops = 0;
  hapticsRate->setValue( hr, id );
  if( hapi_device.get() ) {
    HAPI::HAPIHapticsDevice::DeviceValues dv = hapi_device->getRawDeviceValues();
    // convert to metres
    devicePosition->setValue( (Vec3f)dv.position * 1e-3, id);
    deviceOrientation->setValue( dv.orientation, id);
    force->setValue( (Vec3f)dv.force, id);
    torque->setValue( (Vec3f)dv.torque, id);
    hapi_device->setPositionCalibration( positionCalibration->rt_pos_calibration );
    hapi_device->setOrientationCalibration( orientationCalibration->rt_orn_calibration );
    //cerr << deviceOrientation->getValue() << endl;
    //cerr << trackerOrientation->getValue() << endl;
    HAPI::RuspiniRenderer *ruspini = dynamic_cast< HAPI::RuspiniRenderer * >( hapi_device->getHapticsRenderer() );
    if( ruspini ) {
      proxyPosition->setValue( (Vec3f)(ruspini->getProxyPosition() * 1e-3), id );
    } else {
      HAPI::OpenHapticsRenderer *oh = 
         dynamic_cast< HAPI::OpenHapticsRenderer * >( hapi_device->getHapticsRenderer() );
      if( oh ) {
        proxyPosition->setValue( (Vec3f)(oh->getProxyPosition() ), id );
      }
      else {
        HAPI::GodObjectRenderer *go = dynamic_cast< HAPI::GodObjectRenderer * >( hapi_device->getHapticsRenderer() );
      if( go ) {
        proxyPosition->setValue( (Vec3f)(go->getProxyPosition() * 1e-3 ), id );
      } else 
        proxyPosition->setValue( trackerPosition->getValue(), id );
      }
    }

    HAPI::HAPIHapticsRenderer *renderer = hapi_device->getHapticsRenderer();
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

    HAPI::RuspiniRenderer::Contacts contacts;
    if( renderer ) contacts = renderer->getContacts();
    for( HAPI::RuspiniRenderer::Contacts::iterator i = contacts.begin();
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
        
        HAPI::HAPISurfaceObject::ContactInfo ci = (*i).second;

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

      for( HAPI::RuspiniRenderer::Contacts::iterator j = last_contacts.begin();
           j != last_contacts.end(); j++ ) {
        bool still_in_contact = false;
        for( HAPI::RuspiniRenderer::Contacts::iterator i = contacts.begin();
             i != contacts.end(); i++ ) {
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




