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
/// \file H3DThreadedHapticsDevice.cpp
/// \brief cpp file for H3DThreadedHapticsDevice
///
//
//////////////////////////////////////////////////////////////////////////////

#include "H3DThreadedHapticsDevice.h"

using namespace H3D;

H3DNodeDatabase H3DThreadedHapticsDevice::database( "H3DThreadedHapticsDevice", 
                                                    NULL,
                                                    typeid( H3DThreadedHapticsDevice ),
                                                    &H3DHapticsDevice::database ); 

namespace H3DThreadedHapticsDeviceInternals {



}


  // Callback function for changing force effect that are to be rendered.
Thread::CallbackCode H3DThreadedHapticsDevice::changeForceEffects( void *_data ) {
    void * * data = static_cast< void * * >( _data );
    H3DThreadedHapticsDevice *hd = 
      static_cast< H3DThreadedHapticsDevice * >( data[0] );
    HapticEffectVector* effects  = 
      static_cast< HapticEffectVector * >( data[1] );
    hd->getLastForceEffects().swap( hd->getCurrentForceEffects() );
    hd->getCurrentForceEffects().swap( *effects );
    TimeStamp now = TimeStamp();
    hd->last_loop_time = now - hd->last_effect_change;
    hd->last_effect_change = now;
    return Thread::CALLBACK_DONE;
  }
  // Callback function for rendering force effects on the 
  // HLHapticsDevice.  
  Thread::CallbackCode H3DThreadedHapticsDevice::forceEffectCallback( void *data ) {
    H3DThreadedHapticsDevice *hd = 
      static_cast< H3DThreadedHapticsDevice * >( data );
    
    Rotation rot = hd->getOrientation();
    Vec3f pos = hd->getPosition();
    Vec3f vel = hd->getVelocity();
    bool b = hd->getButtonStatus();
    if( hd->mainButton->getValue() != b )
      hd->mainButton->setValue( b );
    hd->devicePosition->setValue( pos, hd->id );
    hd->deviceOrientation->setValue( rot, hd->id );

    // apply the calibration matrices to get the values to
    // in the H3D API coordinate space.
    pos = hd->positionCalibration->rt_pos_calibration * pos;
    vel = hd->positionCalibration->rt_pos_calibration * vel;
    rot = hd->orientationCalibration->rt_orn_calibration * rot;

    TimeStamp dt = TimeStamp() - hd->last_effect_change;
    HapticForceEffect::EffectInput input( pos, vel, rot, dt );
    HapticForceEffect::EffectOutput output;

    // calculate the forces generated by the force effects from the
    // last loop that are to be interpolated. 
    HapticEffectVector & last_effects = hd->getLastForceEffects(); 
    for( HapticEffectVector::const_iterator i = last_effects.begin();
         i != last_effects.end();
         i++ ) {
      if( (*i)->isInterpolated() )
        output = output + (*i)->calculateForces( input );
    }

    double weighting = dt / hd->last_loop_time;
    if( weighting > 1 ) weighting = 1;
    // the previous force effects are to be decreased as time goes by.
    output = output * ( 1 - weighting );

    // calculate the forces generated by the active force effects
    HapticEffectVector & current_effects = hd->getCurrentForceEffects(); 
    for( HapticEffectVector::const_iterator i = current_effects.begin();
         i != current_effects.end();
         i++ ) {
      if( (*i)->isInterpolated() )
        output = output + (*i)->calculateForces( input ) * weighting;
      else
        output = output + (*i)->calculateForces( input );
    }
    
    output.force = 
      hd->positionCalibration->rt_inv_pos_rotation * output.force;

    // add the resulting force and torque to the rendered force.
    
    hd->sendForce( output.force );
    hd->sendTorque( output.torque );
    // force and torque fields are thread safe.
    hd->force->setValue( output.force, hd->id );
    hd->torque->setValue( output.torque, hd->id );
    hd->nr_haptics_loops++;
    return Thread::CALLBACK_CONTINUE;
  } 

/// Constructor.
H3DThreadedHapticsDevice::H3DThreadedHapticsDevice( 
               Inst< ThreadSafeSField< SFVec3f > > _devicePosition,
               Inst< ThreadSafeSField< SFRotation >  > _deviceOrientation,
               Inst< TrackerPosition > _trackerPosition        ,
               Inst< TrackerOrientation > _trackerOrientation  ,
               Inst< PosCalibration  > _positionCalibration    ,
               Inst< OrnCalibration  > _orientationCalibration ,
               Inst< SFVec3f         > _proxyPosition          ,
               Inst< WeightedProxy   > _weightedProxyPosition  ,     
               Inst< SFFloat         > _proxyWeighting         ,
               Inst< ThreadSafeSField< SFBool > > _mainButton  ,
               Inst< ThreadSafeSField< SFVec3f > > _force      ,
               Inst< ThreadSafeSField< SFVec3f > > _torque     ,
               Inst< SFInt32         > _inputDOF               ,
               Inst< SFInt32         > _outputDOF              ,
               Inst< SFInt32         > _hapticsRate            ,
               Inst< SFNode          > _stylus                 ,
               Inst< SFBool          > _initialized             ):
  H3DHapticsDevice( _devicePosition, _deviceOrientation, _trackerPosition,
                    _trackerOrientation, _positionCalibration, 
                    _orientationCalibration, _proxyPosition,
                    _weightedProxyPosition, _proxyWeighting, _mainButton,
                    _force, _torque, _inputDOF, _outputDOF, _hapticsRate,
                    _stylus, _initialized ),
  nr_haptics_loops( 0 ) {

  type_name = "H3DThreadedHapticsDevice";  
  database.initFields( this );

  trackerPosition->route( proxyPosition, id );
}


void H3DThreadedHapticsDevice::initDevice() {
  if( !initialized->getValue() ) {
#ifdef WIN32
    thread = new HapticThread(  THREAD_PRIORITY_TIME_CRITICAL, 1000 );
#else
    thread = new HapticThread( 0, 1000 );
#endif
    thread->asynchronousCallback( H3DThreadedHapticsDevice::forceEffectCallback,
                                  this );
    H3DHapticsDevice::initDevice();
  }
}

void H3DThreadedHapticsDevice::disableDevice() {
  delete thread;
  thread = NULL;
  H3DHapticsDevice::disableDevice();
}

void H3DThreadedHapticsDevice::renderEffects( 
                         const HapticEffectVector &effects ) {
  if( thread ) {
    // make a copy of the effects vector since it is swapped in
    // the callback.
    HapticEffectVector effects_copy( effects );
    typedef void *pp;
    void * param[] = { this, &effects_copy };
    // change the current_force_effects vector to render the new effects.
    thread->synchronousCallback( H3DThreadedHapticsDevice::changeForceEffects,
                                 param );
  }
}

void H3DThreadedHapticsDevice::updateDeviceValues() {
  H3DHapticsDevice::updateDeviceValues();
  TimeStamp now = TimeStamp();
  TimeStamp dt = now - last_update_values;
  last_update_values = now;
  H3DInt32 hr = (H3DInt32)( nr_haptics_loops / dt );
  nr_haptics_loops = 0;
  hapticsRate->setValue( hr, id );
}



