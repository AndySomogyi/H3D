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


//#define NEW
const H3DDouble dotproduct_epsilon = 1e-7;
const H3DDouble length_sqr_point_epsilon = 1e-10;
const H3DDouble length_sqr_normal_epsilon = 1e-3;
const H3DDouble plane_width_epsilon = 1e-8;

H3DNodeDatabase H3DThreadedHapticsDevice::database( "H3DThreadedHapticsDevice", 
                                                    NULL,
                                                    typeid( H3DThreadedHapticsDevice ),
                                                    &H3DHapticsDevice::database ); 

namespace H3DThreadedHapticsDeviceInternals {



}


// Callback function for changing force effect that are to be rendered.
HAPI::PeriodicThread::CallbackCode H3DThreadedHapticsDevice::changeForceEffects( void *_data ) {
    void * * data = static_cast< void * * >( _data );
    H3DThreadedHapticsDevice *hd = 
      static_cast< H3DThreadedHapticsDevice * >( data[0] );
#ifdef USE_HAPTICS
    HapticEffectVector* effects  = 
      static_cast< HapticEffectVector * >( data[1] );
    hd->getLastForceEffects().swap( hd->getCurrentForceEffects() );
    hd->getCurrentForceEffects().swap( *effects );
    TimeStamp now = TimeStamp();
    hd->last_loop_time = now - hd->last_effect_change;
    hd->last_effect_change = now;
#endif
    return HAPI::PeriodicThread::CALLBACK_DONE;
  }


HAPI::PeriodicThread::CallbackCode H3DThreadedHapticsDevice::changeHapticShapes( void *_data ) {
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
    return HAPI::PeriodicThread::CALLBACK_DONE;
  }

inline void onOnePlaneContact( const HAPI::PlaneConstraint &c, 
                               HAPI::HAPISurfaceObject::ContactInfo &contact ) {
  //cerr << "1";
  contact.y_axis = c.normal;
  // cerr << "1: " << contact.y_axis << endl;
  Vec3d a( contact.y_axis.z, contact.y_axis.x, contact.y_axis.y );
  contact.x_axis = contact.y_axis % a;
  contact.z_axis = contact.x_axis % contact.y_axis;
  assert( c.haptic_shape );
  c.haptic_shape->surface->onContact( contact );
}

inline void onTwoPlaneContact( const HAPI::PlaneConstraint &p0,
                               const HAPI::PlaneConstraint &p1,
                               HAPI::HAPISurfaceObject::ContactInfo &contact ) {

  if( 1 - p0.normal * p1.normal < dotproduct_epsilon ) {
    onOnePlaneContact( p0, contact );
    return;
  }
  Vec3d contact_global = contact.globalContactPoint();

  // find the finger vector in terms of the basis formed
  // by the two surface normals.
  Vec3d n0 = p0.normal;
  Vec3d n1 = p1.normal;
  H3DDouble ab = n0 * n1;
  H3DDouble d = 1 - ab * ab;
  //cerr << "d = " << d << endl;
  /*
  if( d < dotproduct_epsilon ) {
		onOnePlaneContact( p0, contact );
    cerr<<"@"<< endl;
    return;
  } else {
		d = 1 / d;
    
		H3DDouble h0 = ( p0.point - contact_global ) * p0.normal;
		H3DDouble h1 = ( p1.point - contact_global ) * p1.normal;
		H3DDouble x = ( h0 - ab * h1 ) * d;
		Vec3d off = x * p0.normal + ( h1 - ab*x ) * p1.normal;
		H3DDouble offl = off.length();
		if( offl > 1e-5 ) {
		  // get rid of the lower plane.
		  if( h1 < h0 ) {
        onOnePlaneContact( p0, contact );
        return;
		  } else {
        onOnePlaneContact( p1, contact );
        return;
		  }
		} 
    }*/

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
    //cerr << "2";
    H3DDouble sum = local_pos.x + local_pos.y;
    
    H3DDouble weight = 0;
    if( sum < 0 )
      weight = local_pos.x / sum;
    else {
      cerr << "FDAFDASFSDAFSDAFSDAFDASF" << endl;
    }
    contact.y_axis = p0.normal * weight + p1.normal * (1 - weight );
    contact.y_axis = p0.normal * local_pos.x + p1.normal * local_pos.y;
    contact.y_axis.normalizeSafe();
    
    //cerr << "2: " << contact.y_axis << endl;

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

    Vec2d pm_0;
    Vec2d pm_1;

    assert( p0.haptic_shape );
    p0.haptic_shape->surface->onContact( contact );
    H3DDouble p0_proxy_movement = 
      Vec3d( contact.proxy_movement_local.x, 
             0, 
             contact.proxy_movement_local.y ) *
      line_dir_local;

    pm_0 = contact.proxy_movement_local;

    Vec3d p0_force = contact.force_global;
    
    assert( p1.haptic_shape );
    p1.haptic_shape->surface->onContact( contact );
    H3DDouble p1_proxy_movement = 
      Vec3d( contact.proxy_movement_local.x, 
             0, 
             contact.proxy_movement_local.y ) *
      line_dir_local;

     pm_1 = contact.proxy_movement_local;
    Vec3d p1_force = contact.force_global;
    
    Vec3d proxy_movement = 
      H3DMin( p0_proxy_movement, p1_proxy_movement ) * 
      line_dir_local;
    if( p0_proxy_movement > 0.1 )
      cerr << p0_proxy_movement << endl;
    contact.proxy_movement_local = Vec2d( proxy_movement.x, proxy_movement.z ); 
      
    //contact.proxy_movement_local = pm_0 * weight + pm_1 * ( 1 - weight );
    //contact.proxy_movement_local = Vec2f( 0, 0 );
    //cerr << p0_proxy_movement.x << endl;
    contact.force_global = p0_force * weight + p1_force * ( 1 - weight );
  }
}


inline void onThreeOrMorePlaneContact(  vector< HAPI::PlaneConstraint > &constraints,
                                      HAPI::HAPISurfaceObject::ContactInfo &contact ) {
  vector< HAPI::PlaneConstraint >::iterator i = constraints.begin();
  HAPI::PlaneConstraint &p0 = (*i++);
  HAPI::PlaneConstraint &p1 = (*i++);
  HAPI::PlaneConstraint &p2 = (*i++);
#ifdef NEW
  if( 1 - p0.normal * p1.normal < dotproduct_epsilon ||
      1 - p0.normal * p2.normal < dotproduct_epsilon ) {
    onTwoPlaneContact( p1, p2, contact );
    return;
  }

  if( 1 - p1.normal * p2.normal < dotproduct_epsilon ) {
    onTwoPlaneContact( p0, p2, contact );
    return;
  }
#endif

  Vec3d contact_global = contact.globalContactPoint();

  while( i != constraints.end() ) {
    Matrix4d m( p0.normal.x, p1.normal.x, p2.normal.x, contact_global.x, 
                p0.normal.y, p1.normal.y, p2.normal.y, contact_global.y, 
                p0.normal.z, p1.normal.z, p2.normal.z, contact_global.z,
                0, 0, 0, 1 );
    Matrix4d m_inv = m.inverse();
    Vec3d local_pos = m_inv * contact.globalProbePosition();
    
    if( local_pos.x > Constants::f_epsilon ) {
      std::swap( p0, *i++ ); 
    } else if( local_pos.y > Constants::f_epsilon ) {
      std::swap( p1, *i++ ); 
    } else if( local_pos.z > Constants::f_epsilon ) {
      std::swap( p2, *i++ );
    } else {
      break;
    }
  }


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
    //cerr << "3";
    H3DDouble sum = local_pos.x + local_pos.y + local_pos.z;
    
    Vec3d weight;
    if( sum < 0 )
      weight = local_pos / sum;

    contact.y_axis = 
      p0.normal * weight.x + 
      p1.normal * weight.y + 
      p2.normal * weight.z;
    
    //cerr << "31: " << contact.y_axis << endl;
    Vec3d bb = contact.globalContactPoint() - contact.globalProbePosition();
    bb.normalizeSafe();
    contact.y_axis= bb;
    //cerr << "32: " << bb << endl;

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
    
    //contact.proxy_movement_local = Vec2d( 0, 0 ); 
    
    contact.force_global = 
      p0_force * weight.x + 
      p1_force * weight.y + 
      p2_force * weight.z;
  }
}


// Callback function for rendering force effects on the 
// HLHapticsDevice.  
HAPI::PeriodicThread::CallbackCode H3DThreadedHapticsDevice::forceEffectCallback( void *data ) {
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
    HAPI::HapticForceEffect::EffectInput input( pos, vel, rot, dt );
    HAPI::HapticForceEffect::EffectOutput output;

#ifdef USE_HAPTICS
    // calculate the forces generated by the force effects from the
    // last loop that are to be interpolated. 
    HapticEffectVector & last_effects = hd->getLastForceEffects(); 
    for( HapticEffectVector::const_iterator i = last_effects.begin();
         i != last_effects.end();
         i++ ) {
      if( (*i)->isInterpolated() )
        output = output + (*i)->calculateForces( input );
    }
#endif

    double weighting = dt / hd->last_loop_time;
    if( weighting > 1 ) weighting = 1;
    // the previous force effects are to be decreased as time goes by.
    output = output * ( 1 - weighting );

#ifdef USE_HAPTICS
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
#endif
    
    bool has_intersection = false;
    H3DDouble d2;
    HAPI::Bounds::IntersectionInfo closest_intersection;
    vector< HAPI::Bounds::PlaneConstraint > constraints;

    // calculate the forces generated by the active haptic shapes

    // get the constraints from the current shapes.
    HapticShapeVector & current_shapes = hd->getCurrentHapticShapes(); 
    for( HapticShapeVector::const_iterator i = current_shapes.begin();
         i != current_shapes.end();
         i++ ) {
      (*i)->getConstraints( hd->proxy_position, 0.1, constraints );
    }

           cerr << current_shapes.size() << " " << constraints.size() << endl;

   //      cerr << "S: " << current_shapes.size() << endl;

    Vec3d proxy_pos = hd->proxy_position;
    H3DDouble proxy_radius = 0.01;

    bool done = false;
    //cerr << "#####" << endl;
    while( !done ) {
      done = true;
    // make sure the proxy is above any constraints
      for( vector< HAPI::Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      H3DDouble d = (*i).normal * (proxy_pos - (*i).point );
      //cerr << d << endl;
      if( d < 0 && d > -proxy_radius ) {
        //cerr << (*i).normal << " " << d << endl;
        proxy_pos = proxy_pos + (*i).normal * (-d+1e-15);
          done = false;
      }
    }
      
    }

    vector< HAPI::PlaneConstraint > intersected_constraints;
    vector< HAPI::PlaneConstraint > closest_constraints;

#ifdef NEW

    Vec3d from = proxy_pos;
    Vec3d to = pos;
H3DDouble epsilon = plane_width_epsilon;
int nr = constraints.size();
    for( vector< Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      Bounds::IntersectionInfo intersection;
      Vec3d n = (*i).normal;
      H3DFloat d = (*i).point * n;
      H3DFloat fn = from * n - d;
      H3DFloat tn = to * n - d;
      if( fn > -epsilon ) {
        // normal case, finger is in the surface
        if( tn < epsilon ) {
          short mask;
          if( fn < epsilon )  mask  = 0x10;
          else                mask  = 0x20;
          if( tn > -epsilon ) mask |= 0x01;
          else                mask |= 0x02;
          
          // if( off * off < 5e-8 ) {
          {
            H3DDouble k;
            switch( mask ) {
            case 0x11:
              // both from and to are on the plane.
              to -= tn * n;
              closest_constraints.push_back( *i );
              break;
            case 0x12:
              // from is on the plane, to is below.
              //closest_constraints.push_back( *i );
              to = from - fn * n;
              /*intersected_constraints.insert( intersected_constraints.end(),
                                              closest_constraints.begin(),
                                              closest_constraints.end() );
                                              closest_constraints.clear();*/
              closest_constraints.push_back( *i );
              break;
            case 0x21:
              // to is on the plane, from is above.
              to -= tn * n;
              closest_constraints.push_back( *i );
              break;
            case 0x22:
              // from is above the plane, to is below.
              k = fn / ( fn - tn );
              to = k * to + (1-k) * from;
              intersected_constraints.insert( intersected_constraints.end(),
                                              closest_constraints.begin(),
                                              closest_constraints.end() );
              closest_constraints.clear();
              closest_constraints.push_back( *i );
              break;
            }
          }
        }
      }
    }
         if( closest_constraints.size() == 1 ) {
       cerr << "1" << endl;
          Vec3d from = proxy_pos;
    Vec3d to = pos;
H3DDouble epsilon = plane_width_epsilon;
int nr = constraints.size();
    for( vector< Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      Bounds::IntersectionInfo intersection;
      Vec3d n = (*i).normal;
      H3DFloat d = (*i).point * n;
      H3DFloat fn = from * n - d;
      H3DFloat tn = to * n - d;
      if( fn > -epsilon ) {
        // normal case, finger is in the surface
        if( tn < epsilon ) {
          short mask;
          if( fn < epsilon )  mask  = 0x10;
          else                mask  = 0x20;
          if( tn > -epsilon ) mask |= 0x01;
          else                mask |= 0x02;
          
          // if( off * off < 5e-8 ) {
          {
            H3DDouble k;
            switch( mask ) {
            case 0x11:
              // both from and to are on the plane.
              to -= tn * n;
              closest_constraints.push_back( *i );
              break;
            case 0x12:
              // from is on the plane, to is below.
              //closest_constraints.push_back( *i );
              to = from - fn * n;
              /*intersected_constraints.insert( intersected_constraints.end(),
                                              closest_constraints.begin(),
                                              closest_constraints.end() );
                                              closest_constraints.clear();*/
              closest_constraints.push_back( *i );
              break;
            case 0x21:
              // to is on the plane, from is above.
              to -= tn * n;
              closest_constraints.push_back( *i );
              break;
            case 0x22:
              // from is above the plane, to is below.
              k = fn / ( fn - tn );
              to = k * to + (1-k) * from;
              intersected_constraints.insert( intersected_constraints.end(),
                                              closest_constraints.begin(),
                                              closest_constraints.end() );
              closest_constraints.clear();
              closest_constraints.push_back( *i );
              break;
            }
          }
        }
      }
    }
         }
    closest_intersection.point = to;
#else
   // cerr << "#######" << endl;
  for( vector< HAPI::Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      HAPI::Bounds::IntersectionInfo intersection;
      Vec3d vv =pos -  proxy_pos;
      vv.normalizeSafe();
      if( (*i).lineIntersect( proxy_pos, pos, intersection ) ) {
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
              < length_sqr_point_epsilon ) {
            bool unique_constraint = true;
            for( vector< HAPI::Bounds::PlaneConstraint >::iterator j = 
                   closest_constraints.begin();
                 j != closest_constraints.end(); j++ ) {
              if( ( intersection.normal - (*j).normal ).lengthSqr() < 
                  length_sqr_normal_epsilon ) {
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

         
         if( closest_constraints.size() == 1 ) {

           closest_constraints.clear();
           intersected_constraints.clear();
           has_intersection = false;
           for( vector< HAPI::Bounds::PlaneConstraint >::iterator i = constraints.begin();
         i != constraints.end(); i++ ) {
      HAPI::Bounds::IntersectionInfo intersection;
      Vec3d vv =pos -  proxy_pos;
      vv.normalizeSafe();
      if( (*i).lineIntersect( proxy_pos, pos, intersection ) ) {
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
              < length_sqr_point_epsilon ) {
            bool unique_constraint = true;
            for( vector< HAPI::Bounds::PlaneConstraint >::iterator j = 
                   closest_constraints.begin();
                 j != closest_constraints.end(); j++ ) {
              if( ( intersection.normal - (*j).normal ).lengthSqr() < 
                  length_sqr_normal_epsilon ) {//if( H3DAbs( intersection.normal * (*j).normal - 1 ) < 
                  //1e-9 ) {
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
         }
#endif
    unsigned int nr_constraints = closest_constraints.size();

    Vec3d new_proxy_pos, new_force;

    HAPI::HAPISurfaceObject::ContactInfo contact;

    contact.contact_point_global = closest_intersection.point;
    contact.probe_position_global = pos;

    //if( nr_constraints == 0 ) cerr << "0";
    //if( nr_constraints == 1 || nr_constraints == 3 )
      cerr << nr_constraints;

    if( nr_constraints == 0 ) {
      new_proxy_pos = proxy_pos + (pos-proxy_pos)*0.05;
    } else {
      if( nr_constraints == 1 ) {
        onOnePlaneContact( closest_constraints[0], contact );
      } else if( nr_constraints == 2 ) {
        onTwoPlaneContact( closest_constraints[0],
                           closest_constraints[1], contact );
      } if( nr_constraints >= 3 ) {
        onThreeOrMorePlaneContact( closest_constraints,
                                   contact );
      } 

      
      new_proxy_pos = 
        closest_intersection.point + 
        contact.proxy_movement_local.x * contact.x_axis + 
        contact.proxy_movement_local.y * contact.z_axis;
      new_force = contact.force_global;
    }
   
#ifdef NEW
    from = proxy_pos;
    to = new_proxy_pos;
    for( vector< HAPI::Bounds::PlaneConstraint >::iterator i = 
           constraints.begin();
         i != constraints.end(); i++ ) {
      HAPI::Bounds::IntersectionInfo intersection;
      Vec3d n = (*i).normal;
      H3DFloat d = (*i).point * n;
      H3DFloat fn = from * n - d;
      H3DFloat tn = to * n - d;
      if( fn > -epsilon ) {
        // normal case, finger is in the surface
        if( tn < epsilon ) {
          short mask;
          if( fn < epsilon )  mask  = 0x10;
          else                mask  = 0x20;
          if( tn > -epsilon ) mask |= 0x01;
          else                mask |= 0x02;
          
          // if( off * off < 5e-8 ) {
          {
            H3DDouble k;
            switch( mask ) {
            case 0x11:
              // both from and to are on the plane.
              //to -= tn * n;
              //closest_constraints.push_back( *i );
              break;
            case 0x12:
              // from is on the plane, to is below.
              //closest_constraints.push_back( *i );
              to = from - fn * n;
              /*intersected_constraints.insert( intersected_constraints.end(),
                                              closest_constraints.begin(),
                                              closest_constraints.end() );
                                              closest_constraints.clear();*/
              //closest_constraints.push_back( *i );
              break;
            case 0x21:
              // to is on the plane, from is above.
              to -= tn * n;
              //closest_constraints.push_back( *i );
              break;
            case 0x22:
              // from is above the plane, to is below.
              k = fn / ( fn - tn );
              to = k * to + (1-k) * from;
              //intersected_constraints.insert( intersected_constraints.end(),
              //                                closest_constraints.begin(),
              //                                closest_constraints.end() );
              //closest_constraints.clear();
              //closest_constraints.push_back( *i );
              break;
            }
          }
        }
      }
    }

    new_proxy_pos = to;


#else
    has_intersection = false;
    Vec3d closest_point;
    for( vector< HAPI::Bounds::PlaneConstraint >::iterator i = 
           intersected_constraints.begin();
         i != intersected_constraints.end(); i++ ) {
      
      HAPI::Bounds::IntersectionInfo intersection;
      
      Vec3d vv = new_proxy_pos - proxy_pos;
      vv.normalizeSafe();
      if( (*i).lineIntersect( closest_intersection.point, 
                              new_proxy_pos, intersection ) ) {
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
#endif
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
    return HAPI::PeriodicThread::CALLBACK_CONTINUE;
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
    #ifdef PROFILING
    thread = NULL;
    #else
    thread = new HAPI::HapticThread(  THREAD_PRIORITY_ABOVE_NORMAL, 1000 );
    #endif
#else
    thread = new HAPI::HapticThread( 0, 1000 );
#endif
#ifndef PROFILING
    thread->asynchronousCallback( H3DThreadedHapticsDevice::forceEffectCallback,
                                  this );
#endif
    H3DHapticsDevice::initDevice();
  }
}

void H3DThreadedHapticsDevice::disableDevice() {
  if( thread )
    delete thread;
  thread = NULL;
  H3DHapticsDevice::disableDevice();
}

#ifdef USE_HAPTICS
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
#endif

void H3DThreadedHapticsDevice::renderShapes( 
                         const HapticShapeVector &shapes ) {
#ifndef PROFILING
  if( thread ) {
#endif
    // make a copy of the effects vector since it is swapped in
    // the callback.
    HapticShapeVector shapes_copy( shapes );
    typedef void *pp;
    void * param[] = { this, &shapes_copy };
    // change the current_force_effects vector to render the new effects.
#ifdef PROFILING
     H3DThreadedHapticsDevice::changeHapticShapes( param );
     H3DThreadedHapticsDevice::forceEffectCallback( this );
#else
     thread->synchronousCallback( H3DThreadedHapticsDevice::changeHapticShapes,
                                  param );
  }
#endif
}

void H3DThreadedHapticsDevice::updateDeviceValues() {
  H3DHapticsDevice::updateDeviceValues();
  TimeStamp now = TimeStamp();
  TimeStamp dt = now - last_update_values;
  last_update_values = now;
  H3DInt32 hr = (H3DInt32)( nr_haptics_loops / dt );
  nr_haptics_loops = 0;
  hapticsRate->setValue( hr, id );
  //cerr << hr << endl;
  // TODO: lock
  proxyPosition->setValue( (Vec3f)proxy_position, id );
}




