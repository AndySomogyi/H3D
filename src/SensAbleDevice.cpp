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
  /*  FIELDDB_ELEMENT( SensAbleDevice, useGravityCompensation, INPUT_OUTPUT );
  FIELDDB_ELEMENT( SensAbleDevice, reset, INPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, waitForReset, INPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, endEffectorMass, INPUT_ONLY );
  FIELDDB_ELEMENT( SensAbleDevice, useBrakes, INPUT_OUTPUT );
  FIELDDB_ELEMENT( SensAbleDevice, deviceType, OUTPUT_ONLY );*/
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
               Inst< SFBool          > _mainButton,
               Inst< SFVec3f         > _force,
               Inst< SFVec3f         > _torque,
               Inst< SFInt32         > _inputDOF,
               Inst< SFInt32         > _outputDOF,
               Inst< SFInt32         > _hapticsRate,
               Inst< SFNode          > _stylus,
               Inst< SFBool          > _initialized,
               Inst< SFFloat         > _proxyRadius ) :
  H3DHapticsDevice( _devicePosition, _deviceOrientation, _trackerPosition,
              _trackerOrientation, _positionCalibration, 
              _orientationCalibration, _proxyPosition,
              _weightedProxyPosition, _proxyWeighting, _mainButton,
              _force, _torque, _inputDOF, _outputDOF, _hapticsRate,
              _stylus, _initialized ) {

  type_name = "SensAbleDevice";  
  database.initFields( this );

  hapi_device.reset( new HAPI::PhantomHapticsDevice );
}


void SensAbleDevice::Reset::onNewValue( const bool &v ) {
  SensAbleDevice *fd = 
    static_cast< SensAbleDevice * >( getOwner() );
  HAPI::PhantomHapticsDevice * dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( fd->hapi_device.get() );
//  if( v ) dhd->reset();
}

void SensAbleDevice::WaitReset::onNewValue( const bool &v ) {
  SensAbleDevice *fd = 
    static_cast< SensAbleDevice * >( getOwner() );
  HAPI::PhantomHapticsDevice * dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( fd->hapi_device.get() );
 // if( v ) dhd->waitForReset();
}

void SensAbleDevice::GravityComp::onValueChange( const bool &v ) {
  SensAbleDevice *fd = 
    static_cast< SensAbleDevice * >( getOwner() );
  HAPI::PhantomHapticsDevice * dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( fd->hapi_device.get() );
 // dhd->useGravityCompensation( v );
}

void SensAbleDevice::EffectorMass::onValueChange( const H3DFloat &v ) {
  SensAbleDevice *fd = 
    static_cast< SensAbleDevice * >( getOwner() );
  HAPI::PhantomHapticsDevice * dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( fd->hapi_device.get() );
  //dhd->setEffectorMass( v );
}

void SensAbleDevice::Brakes::onValueChange( const bool &v ) {
  SensAbleDevice *fd = 
    static_cast< SensAbleDevice * >( getOwner() );
  HAPI::PhantomHapticsDevice * dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( fd->hapi_device.get() );
  //dhd->useBrakes( v );
}


void SensAbleDevice::initDevice() {
  H3DHapticsDevice::initDevice();
  HAPI::PhantomHapticsDevice *dhd = 
    static_cast< HAPI::PhantomHapticsDevice * >( hapi_device.get() );
  //if( dhd )
  //  deviceType->setValue( dhd->getDeviceType(), id ); 
}

void SensAbleDevice::disableDevice() {
  H3DHapticsDevice::disableDevice();
  //deviceType->setValue( -1, id );
}
