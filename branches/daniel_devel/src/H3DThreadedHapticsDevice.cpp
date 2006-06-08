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
PeriodicThread::CallbackCode H3DThreadedHapticsDevice::changeForceEffects( void *_data ) {
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
    return PeriodicThread::CALLBACK_DONE;
  }


PeriodicThread::CallbackCode H3DThreadedHapticsDevice::changeHapticShapes( void *_data ) {
    void * * data = static_cast< void * * >( _data );
    H3DThreadedHapticsDevice *hd = 
      static_cast< H3DThreadedHapticsDevice * >( data[0] );
    HapticShapeVector* shapes  = 
      static_cast< HapticShapeVector * >( data[1] );
    //hd->getLastForceEffects().swap( hd->getCurrentForceEffects() );
    hd->getCurrentHapticShapes().swap( *shapes );
    //TimeStamp now = TimeStamp();
    //hd->last_loop_time = now - hd->last_effect_change;
    //hd->last_effect_change = now;
    return PeriodicThread::CALLBACK_DONE;
  }

inline void onOnePlaneContact( const PlaneConstraint &c, 
                               HAPISurfaceObject::ContactInfo &contact ) {
  contact.y_axis = c.normal;
  Vec3d a( contact.y_axis.z, contact.y_axis.x, contact.y_axis.y );
  contact.x_axis = contact.y_axis % a;
  contact.z_axis = contact.x_axis % contact.y_axis;
  assert( c.haptic_shape );
  c.haptic_shape->surface->onContact( contact );
}

inline void onTwoPlaneContact( const PlaneConstraint &p0,
                               const PlaneConstraint &p1,
                               HAPISurfaceObject::ContactInfo &contact ) {
  Vec3d contact_global = contact.globalContactPoint();

  Vec3d line_dir = p0.normal % p1.normal;
  line_dir.normalizeSafe();
  Matrix4d m( p0.normal.x, p1.normal.x, line_dir.x, contact_global.x, 
              p0.normal.y, p1.normal.y, line_dir.y, contact_global.y, 
              p0.normal.z, p1.normal.z, line_dir.z, contact_global.z,
              0, 0, 0, 1 );
  Matrix4d m_inv = m.inverse();
  
  Vec3d local_pos = m_inv * contact.globalProbePosition();

  if( local_pos.x > Constants::f_epsilon ) {
    onOnePlaneContact( p1, contact );
  } else if( local_pos.y > Constants::f_epsilon ) {
    onOnePlaneContact( p0, contact );
  } else {
    H3DDouble sum = local_pos.x + local_pos.y;
    
    H3DDouble fdaf = line_dir.length();

    H3DDouble weight = 0;
    if( sum < 0 )
      weight = local_pos.x / sum;
    
    contact.y_axis = p0.normal * weight + p1.normal * (1 - weight );
    contact.y_axis.normalizeSafe();
    
    Vec3d a( contact.y_axis.z, contact.y_axis.x, contact.y_axis.y );
    contact.x_axis = contact.y_axis % a;
    contact.z_axis = contact.x_axis % contact.y_axis;
    contact.z_axis.normalizeSafe();
    contact.x_axis.normalizeSafe();
    
    Vec3d line_dir_local = contact.vectorToLocal( line_dir );

    Matrix3d mm = Matrix3d( contact.x_axis.x, contact.y_axis.x,  contact.z_axis.x,
                 contact.x_axis.y,  contact.y_axis.y,  contact.z_axis.y,
                 contact.x_axis.z,  contact.y_axis.z,  contact.z_axis.z ).inverse();
    Vec3d fff = mm * line_dir;

    assert( p0.haptic_shape );
    p0.haptic_shape->surface->onContact( contact );
    H3DDouble p0_proxy_movement = 
      Vec3d( contact.proxy_movement_local.x, 
             0, 
             contact.proxy_movement_local.y ) *
      line_dir_local;

    Vec3d p0_force = contact.force_global;
    
    assert( p1.haptic_shape );
    p1.haptic_shape->surface->onContact( contact );
    H3DDouble p1_proxy_movement = 
      Vec3d( contact.proxy_movement_local.x, 
             0, 
             contact.proxy_movement_local.y ) *
      line_dir_local;

    Vec3d p1_force = contact.force_global;
    
    Vec3d proxy_movement = 
      H3DMin( p0_proxy_movement, p1_proxy_movement ) * 
      line_dir_local;
    if( p0_proxy_movement > 0.1 )
      cerr << p0_proxy_movement << endl;
    contact.proxy_movement_local = Vec2d( proxy_movement.x, proxy_movement.z ); 
      
    
    //cerr << p0_proxy_movement.x << endl;
    contact.force_global = p0_force * weight + p1_force * ( 1 - weight );
  }
}

inline void onThreeOrMorePlaneContact(  const PlaneConstraint &p0,
                                        const PlaneConstraint &p1,
                                        const PlaneConstraint &p2,
                                        HAPISurfaceObject::ContactInfo &contact ) {
  Vec3d contact_global = contact.globalContactPoint();

  Matrix4d m( p0.normal.x, p1.normal.x, p2.normal.x, contact_global.x, 
              p0.normal.y, p1.normal.y, p2.normal.y, contact_global.y, 
              p0.normal.z, p1.normal.z, p2.normal.z, contact_global.z,
              0, 0, 0, 1 );

  Matrix4d m_inv = m.inverse();
  
  Vec3d local_pos = m_inv * contact.globalProbePosition();
  
  if( local_pos.x > Constants::f_epsilon ) {
    onTwoPlaneContact( p1, p2, contact );
  } else if( local_pos.y > Constants::f_epsilon ) {
    onTwoPlaneContact( p0, p2, contact );
  } else if( local_pos.z > Constants::f_epsilon ) {
    onTwoPlaneContact( p0, p1, contact );
  } else {
    H3DDouble sum = local_pos.x + local_pos.y + local_pos.z;
    
    Vec3d weight;
    if( sum < 0 )
      weight = local_pos / sum;

    contact.y_axis = 
      p0.normal * weight.x + 
      p1.normal * weight.y + 
      p2.normal * weight.z;
    
    Vec3d a( contact.y_axis.z, contact.y_axis.x, contact.y_axis.y );
    contact.x_axis = contact.y_axis % a;
    contact.z_axis = contact.x_axis % contact.y_axis;

    contact.x_axis.normalizeSafe();
    contact.y_axis.normalizeSafe();
    contact.z_axis.normalizeSafe();

    assert( p0.haptic_shape );
    p0.haptic_shape->surface->onContact( contact );
    Vec3d p0_force = contact.force_global;
    
    assert( p1.haptic_shape );
    p1.haptic_shape->surface->onContact( contact );
    Vec3d p1_force = contact.force_global;

    assert( p2.haptic_shape );
    p2.haptic_shape->surface->onContact( contact );
    Vec3d p2_force = contact.force_global;
    
    contact.proxy_movement_local = Vec2d( 0, 0 ); 
    
    contact.force_global = 
      p0_force * weight.x + 
      p1_force * weight.y + 
      p2_force * weight.z;
  }
}


// Callback function for rendering force effects on the 
// HLHapticsDevice.  
PeriodicThread::CallbackCode H3DThreadedHapticsDevice::forceEffectCallback( void *data ) {
  H3DThreadedHapticsDevice *hd = 
    static_cast< H3DThreadedHapticsDevice * >( data );
  
    Rotation rot = hd->getOrientation();
    Vec3d pos = hd->getPosition();
    Vec3d vel = hd->getVelocity();
    bool b = hd->getButtonStatus();
    if( hd->mainButton->getValue() != b )
      hd->mainButton->setValue( b );
    hd->devicePosition->setValue( Vec3f(pos), hd->id );
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
    
    bool has_intersection = false;
    H3DDouble d2;
    Bounds::IntersectionInfo closest_intersection;
    vector< Bounds::PlaneConstraint > constraints;

    // calculate the forces generated by the active haptic shapes

    // get the constraints from the current shapes.
    HapticShapeVector & current_shapes = hd->getCurrentHapticShapes(); 
    for( HapticShapeVector::const_iterator i = current_shapes.begin();
         i != current_shapes.end();
         i++ ) {
      (*i)->getConstraints( hd->proxy_position, 0.1, constraints );
    }

    Vec3d proxy_pos = hd->proxy_position;
    H3DDouble proxy_radius = 0.01;

    
    // make sure the proxy is above any constraints
    /*for( vector< Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      H3DDouble d = (*i).normal * (proxy_pos - (*i).point );
      if( d < 0 && d > -proxy_radius ) {
        //cerr << (*i).normal << " " << d << endl;
        proxy_pos = proxy_pos + (*i).normal * (-d+1e-7);
      }
      }*/

    vector< PlaneConstraint > intersected_constraints;
    vector< PlaneConstraint > closest_constraints;

    for( vector< Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      Bounds::IntersectionInfo intersection;
      Vec3d vv =pos -  proxy_pos;
      vv.normalizeSafe();
      if( (*i).lineIntersect( proxy_pos-vv*1e-7, pos+vv*1e-7, intersection ) ) {
        if( !has_intersection ) {
          closest_intersection = intersection;
          Vec3d v = intersection.point - proxy_pos;
          d2 = v * v;
          has_intersection = true;
          closest_constraints.push_back( *i );
        } else {
          Vec3d v = intersection.point - proxy_pos;
          H3DDouble distance = v * v; 
      
          if( (closest_intersection.point - intersection.point).lengthSqr()
              < 1e-14 ) {
            bool unique_constraint = true;
            for( vector< Bounds::PlaneConstraint >::iterator j = 
                   closest_constraints.begin();
                 j != closest_constraints.end(); j++ ) {
              if( H3DAbs( intersection.normal * (*j).normal - 1 ) < 
                  1e-6 ) {
                unique_constraint = false;
              }
            }

            // only add the constraint to the closest_constraints vector
            // if it is an unique constraint.
            if( unique_constraint ) {
              closest_constraints.push_back( *i );
            } else {
              intersected_constraints.push_back( *i );
            }
          } else if( distance < d2 ) {
            closest_intersection = intersection;
            d2 = distance;
            intersected_constraints.insert( intersected_constraints.end(),
                                            closest_constraints.begin(),
                                            closest_constraints.end() );
            closest_constraints.clear();
            closest_constraints.push_back( *i );
          } else {
            intersected_constraints.push_back( *i );
          }
        }
      } else {
        intersected_constraints.push_back( *i );
      }
    } 
    
    unsigned int nr_constraints = closest_constraints.size();

    Vec3d new_proxy_pos, new_force;

    HAPISurfaceObject::ContactInfo contact;

    contact.contact_point_global = closest_intersection.point;
    contact.probe_position_global = pos;

    if( nr_constraints == 0 ) {
      new_proxy_pos = proxy_pos + (pos-proxy_pos)*0.05;
    } else {
      if( nr_constraints == 1 ) {
        onOnePlaneContact( closest_constraints[0], contact );
      } else if( nr_constraints == 2 ) {
        onTwoPlaneContact( closest_constraints[0],
                           closest_constraints[1], contact );
      } if( nr_constraints >= 3 ) {
        onThreeOrMorePlaneContact( closest_constraints[0],
                                   closest_constraints[1],
                                   closest_constraints[2],
                                   contact );
      } 

      new_proxy_pos = 
        closest_intersection.point + 
        contact.proxy_movement_local.x * contact.x_axis + 
        contact.proxy_movement_local.y * contact.z_axis;
      new_force = contact.force_global;
    }
    
    has_intersection = false;
    Vec3d closest_point;
    for( vector< Bounds::PlaneConstraint >::iterator i = 
           intersected_constraints.begin();
         i != intersected_constraints.end(); i++ ) {
      
      Bounds::IntersectionInfo intersection;
      
      Vec3d vv = new_proxy_pos - proxy_pos;
      vv.normalizeSafe();
      if( (*i).lineIntersect( proxy_pos-vv*1e-7, 
                              new_proxy_pos+vv*1e-7, intersection ) ) {
        if( !has_intersection ) {
          closest_point = intersection.point;
          Vec3d v = intersection.point - proxy_pos;
          d2 = v * v;
          has_intersection = true;
        } else {
          Vec3d v = intersection.point - proxy_pos;
          H3DDouble distance = v * v;
          if( distance < d2 ) {
            closest_point = intersection.point;
            d2 = distance;
          }
        }
      } 
    }
    
    if( has_intersection ) {
      //           cerr << proxy_pos << endl;
      //           cerr << new_proxy_pos << endl;
      new_proxy_pos = closest_point;
      //cerr << new_proxy_pos << " " << pos << endl;
    }

    hd->proxy_position = new_proxy_pos;
    output.force = new_force;
    
    output.force = 
      hd->positionCalibration->rt_inv_pos_rotation * output.force;

    // add the resulting force and torque to the rendered force.
    
    hd->sendForce( output.force );
    hd->sendTorque( output.torque );
    // force and torque fields are thread safe.
    hd->force->setValue( (Vec3f)output.force, hd->id );
    hd->torque->setValue( (Vec3f)output.torque, hd->id );
    hd->nr_haptics_loops++;
    return PeriodicThread::CALLBACK_CONTINUE;
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

  // trackerPosition->route( proxyPosition, id );
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

void H3DThreadedHapticsDevice::renderShapes( 
                         const HapticShapeVector &shapes ) {
  if( thread ) {
    // make a copy of the effects vector since it is swapped in
    // the callback.
    HapticShapeVector shapes_copy( shapes );
    typedef void *pp;
    void * param[] = { this, &shapes_copy };
    // change the current_force_effects vector to render the new effects.
    thread->synchronousCallback( H3DThreadedHapticsDevice::changeHapticShapes,
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
  // TODO: lock
  proxyPosition->setValue( (Vec3f)proxy_position, id );
}




