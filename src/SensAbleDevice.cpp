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
/// \file SensAbleDevice.cpp
/// \brief cpp file for SensAbleDevice
///
//
//////////////////////////////////////////////////////////////////////////////


#include "SensAbleDevice.h"
#include <PhantomHapticsDevice.h>

using namespace H3D;

H3DNodeDatabase SensAbleDevice::database( "SensAbleDevice", 
                                          &(newInstance<SensAbleDevice>),
                                          typeid( SensAbleDevice ),
                                          &H3DHapticsDevice::database ); 
namespace SensAbleDeviceInternals {
  FIELDDB_ELEMENT( SensAbleDevice, deviceName, INITIALIZE_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, HDAPIVersion, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, deviceModelType, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, deviceDriverVersion, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, deviceFirmwareVersion, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, deviceVendor, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, deviceSerialNumber, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, maxWorkspaceDimensions, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, usableWorkspaceDimensions, OUTPUT_ONLY );  
  FIELDDB_ELEMENT( SensAbleDevice, tabletopOffset, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, maxForce, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, maxContinuousForce, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, gimbalAngles, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, jointAngles, OUTPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, needsCalibration, OUTPUT_ONLY )
  FIELDDB_ELEMENT( SensAbleDevice, calibrate, INPUT_ONLY )

}

/// Constructor.
SensAbleDevice::SensAbleDevice( 
               Inst< SFVec3f         > _devicePosition,
               Inst< SFRotation      > _deviceOrientation,
               Inst< TrackerPosition > _trackerPosition,
               Inst< TrackerOrientation > _trackerOrientation,
               Inst< PosCalibration  > _positionCalibration,
               Inst< OrnCalibration  > _orientationCalibration,
               Inst< SFVec3f         > _proxyPosition,
               Inst< WeightedProxy   > _weightedProxyPosition,     
               Inst< SFFloat         > _proxyWeighting,
               Inst< MainButton      > _mainButton,
               Inst< SecondaryButton > _secondary_button,
               Inst< SFInt32         > _buttons,
               Inst< SFVec3f         > _force,
               Inst< SFVec3f         > _torque,
               Inst< SFInt32         > _inputDOF,
               Inst< SFInt32         > _outputDOF,
               Inst< SFInt32         > _hapticsRate,
               Inst< SFNode          > _stylus,
               Inst< SFFloat         > _proxyRadius,
               Inst< SFString        > _deviceName ) :
  H3DHapticsDevice( _devicePosition, _deviceOrientation, _trackerPosition,
              _trackerOrientation, _positionCalibration, 
              _orientationCalibration, _proxyPosition,
              _weightedProxyPosition, _proxyWeighting, _mainButton,
                    _secondary_button, _buttons,
              _force, _torque, _inputDOF, _outputDOF, _hapticsRate,
              _stylus ),
    deviceName( _deviceName ),
  HDAPIVersion( new SFString ),
  deviceModelType( new SFString ),
  deviceDriverVersion( new SFString ),
  deviceFirmwareVersion( new SFDouble ),
  deviceVendor(  new SFString ),
  deviceSerialNumber( new SFString ),
  maxWorkspaceDimensions( new MFVec3f ),
  usableWorkspaceDimensions( new MFVec3f ),
  tabletopOffset( new SFFloat ),
  maxForce( new SFFloat ),
  maxContinuousForce( new SFFloat ),
  gimbalAngles( new SFVec3f ),
  jointAngles( new SFVec3f ),
  needsCalibration( new SFBool ),
  calibrate( new SFBool ) { 

  type_name = "SensAbleDevice";  
  database.initFields( this );

  hapi_device.reset( new HAPI::PhantomHapticsDevice );

  maxForce->setValue( 0, id );
  maxContinuousForce->setValue( 0, id );
  tabletopOffset->setValue( 0, id );
  deviceFirmwareVersion->setValue( 0, id );
  maxWorkspaceDimensions->resize( 2, Vec3f(0,0,0), id );
  usableWorkspaceDimensions->resize( 2, Vec3f(0,0,0), id );
  needsCalibration->setValue( false, id );
}


H3DHapticsDevice::ErrorCode SensAbleDevice::initDevice() {
  HAPI::HAPIHapticsDevice::ErrorCode e = H3DHapticsDevice::initDevice();
   HAPI::PhantomHapticsDevice *pd = 
    dynamic_cast< HAPI::PhantomHapticsDevice * >(hapi_device.get() );
  if( e == HAPI::HAPIHapticsDevice::SUCCESS && pd ) {
    HDAPIVersion->setValue( pd->getHDAPIVersion(), id );
    deviceModelType->setValue( pd->getDeviceModelType(), id );
    deviceDriverVersion->setValue( pd->getDeviceDriverVersion(), id );
    deviceVendor->setValue( pd->getDeviceVendor(), id );
    deviceSerialNumber->setValue( pd->getDeviceSerialNumber(), id );
    deviceFirmwareVersion->setValue( pd->getDeviceFirmwareVersion(), id );
    HAPI::Vec3 max, min;
    pd->getMaxWorkspaceDimensions( min, max );
    maxWorkspaceDimensions->setValue( 0, (Vec3f)min, id ); 
    maxWorkspaceDimensions->setValue( 1, (Vec3f)max, id ); 
    pd->getUsableWorkspaceDimensions( min, max );
    usableWorkspaceDimensions->setValue( 0, (Vec3f)min, id ); 
    usableWorkspaceDimensions->setValue( 1, (Vec3f)max, id ); 
    tabletopOffset->setValue( pd->getTabletopOffset(), id );
    maxForce->setValue( pd->getMaxForce(), id );
    maxContinuousForce->setValue( pd->getMaxContinuousForce(), id );
    needsCalibration->setValue( pd->needsCalibration(), id );
  }
  return e;
}

void SensAbleDevice::updateDeviceValues() {
  H3DHapticsDevice::updateDeviceValues();
  HAPI::PhantomHapticsDevice *pd = 
    dynamic_cast< HAPI::PhantomHapticsDevice *>(hapi_device.get() );
  if( pd ) {
    bool b = pd->needsCalibration();
    if( needsCalibration->getValue() != b ) {
      needsCalibration->setValue( b );
    }
    gimbalAngles->setValue( (Vec3f)pd->getGimbalAngles(), id );
    jointAngles->setValue( (Vec3f) pd->getJointAngles(), id );
  }
}
