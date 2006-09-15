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

#include "HAPIDevice.h"
#include <PhantomHapticsDevice.h>

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
      hapi_device->setPositionCalibration( positionCalibration->getValue( ) );
      RuspiniRenderer *rr = 
        dynamic_cast< RuspiniRenderer * >(hapi_device->getHapticsRenderer());
      rr->setProxyRadius( proxyRadius->getValue() );
      
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
    devicePosition->setValue( (Vec3f)hapi_device->getPosition(), id);
    deviceOrientation->setValue( hapi_device->getOrientation(), id);
    cerr << deviceOrientation->getValue() << endl;
    cerr << trackerOrientation->getValue() << endl;
    RuspiniRenderer *ruspini = dynamic_cast< RuspiniRenderer * >( hapi_device->getHapticsRenderer() );
    if( ruspini ) {
      proxyPosition->setValue( (Vec3f)ruspini->getProxyPosition(), id );
    } else {
      proxyPosition->setValue( trackerPosition->getValue(), id );
    }

  }

  //cerr << hr << endl;
  // TODO: lock
  //proxyPosition->setValue( (Vec3f)proxy_position, id );
}




