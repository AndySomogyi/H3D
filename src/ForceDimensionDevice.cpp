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
/// \file ForceDimensionDevice.cpp
/// \brief cpp file for ForceDimensionDevice
///
//
//////////////////////////////////////////////////////////////////////////////


#include "ForceDimensionDevice.h"
#include <DHDHapticsDevice.h>

using namespace H3D;

H3DNodeDatabase ForceDimensionDevice::database( "ForceDimensionDevice", 
                                            &(newInstance<ForceDimensionDevice>),
                                            typeid( ForceDimensionDevice ),
                                            &H3DHapticsDevice::database ); 
namespace ForceDimensionDeviceInternals {
  FIELDDB_ELEMENT( ForceDimensionDevice, useGravityCompensation, INPUT_OUTPUT );
  FIELDDB_ELEMENT( ForceDimensionDevice, reset, INPUT_ONLY );
  FIELDDB_ELEMENT( ForceDimensionDevice, waitForReset, INPUT_ONLY );
  FIELDDB_ELEMENT( ForceDimensionDevice, endEffectorMass, INPUT_ONLY );
  FIELDDB_ELEMENT( ForceDimensionDevice, useBrakes, INPUT_OUTPUT );
  FIELDDB_ELEMENT( ForceDimensionDevice, deviceType, OUTPUT_ONLY );
}

/// Constructor.
ForceDimensionDevice::ForceDimensionDevice( 
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
               Inst< GravityComp     > _useGravityCompensation,
               Inst< Reset           > _reset,
               Inst< WaitReset       > _waitForReset,
               Inst< EffectorMass    > _endEffectorMass,
               Inst< Brakes          > _useBrakes,
               Inst< SFInt32         > _deviceType ) :
  H3DHapticsDevice( _devicePosition, _deviceOrientation, _trackerPosition,
              _trackerOrientation, _positionCalibration, 
              _orientationCalibration, _proxyPosition,
              _weightedProxyPosition, _proxyWeighting, _mainButton,
                    _secondary_button, _buttons,
              _force, _torque, _inputDOF, _outputDOF, _hapticsRate,
              _stylus ),
  useGravityCompensation( _useGravityCompensation ),
  reset( _reset ),
  waitForReset( _waitForReset ),
  endEffectorMass( _endEffectorMass ),
  useBrakes( _useBrakes ),
  deviceType( _deviceType ) {

  type_name = "ForceDimensionDevice";  
  database.initFields( this );

  hapi_device.reset( new HAPI::DHDHapticsDevice );

  useGravityCompensation->setValue( true );
  useBrakes->setValue( false );
  deviceType->setValue( -1, id );
}


void ForceDimensionDevice::Reset::onNewValue( const bool &v ) {
  ForceDimensionDevice *fd = 
    static_cast< ForceDimensionDevice * >( getOwner() );
  HAPI::DHDHapticsDevice * dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( fd->hapi_device.get() );
  if( v ) dhd->reset();
}

void ForceDimensionDevice::WaitReset::onNewValue( const bool &v ) {
  ForceDimensionDevice *fd = 
    static_cast< ForceDimensionDevice * >( getOwner() );
  HAPI::DHDHapticsDevice * dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( fd->hapi_device.get() );
  if( v ) dhd->waitForReset();
}

void ForceDimensionDevice::GravityComp::onValueChange( const bool &v ) {
  ForceDimensionDevice *fd = 
    static_cast< ForceDimensionDevice * >( getOwner() );
  HAPI::DHDHapticsDevice * dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( fd->hapi_device.get() );
  dhd->useGravityCompensation( v );
}

void ForceDimensionDevice::EffectorMass::onValueChange( const H3DFloat &v ) {
  ForceDimensionDevice *fd = 
    static_cast< ForceDimensionDevice * >( getOwner() );
  HAPI::DHDHapticsDevice * dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( fd->hapi_device.get() );
  dhd->setEffectorMass( v );
}

void ForceDimensionDevice::Brakes::onValueChange( const bool &v ) {
  ForceDimensionDevice *fd = 
    static_cast< ForceDimensionDevice * >( getOwner() );
  HAPI::DHDHapticsDevice * dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( fd->hapi_device.get() );
  dhd->useBrakes( v );
}


H3DHapticsDevice::ErrorCode ForceDimensionDevice::initDevice() {
  HAPI::HAPIHapticsDevice::ErrorCode e = H3DHapticsDevice::initDevice();
  HAPI::DHDHapticsDevice *dhd = 
    static_cast< HAPI::DHDHapticsDevice * >( hapi_device.get() );
  if( dhd )
    deviceType->setValue( dhd->getDeviceType(), id ); 

  return e;
}

H3DHapticsDevice::ErrorCode ForceDimensionDevice::releaseDevice() {
  HAPI::HAPIHapticsDevice::ErrorCode e = H3DHapticsDevice::releaseDevice();
  deviceType->setValue( -1, id );
  return e;
}
