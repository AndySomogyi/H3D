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
/// \file MagneticGeometryEffect.cpp
/// \brief cpp file for MagneticGeometryEffect
///
//
//////////////////////////////////////////////////////////////////////////////
#include <H3D/MagneticGeometryEffect.h> 
#include <H3D/H3DHapticsDevice.h>
#include <H3D/HapticsOptions.h>
#include <H3D/Sphere.h>
#include <HAPI/HapticShapeConstraint.h>
#include <HAPI/HapticPrimitiveSet.h>

using namespace H3D;
using namespace HAPI;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase MagneticGeometryEffect::database( 
                                       "MagneticGeometryEffect", 
                                       &(newInstance<MagneticGeometryEffect>),
                                       typeid( MagneticGeometryEffect ),
                                       &H3DForceEffect::database );

namespace MagneticGeometryEffectInternals {
  FIELDDB_ELEMENT( MagneticGeometryEffect, enabled, INPUT_OUTPUT );
  FIELDDB_ELEMENT( MagneticGeometryEffect, startDistance, INPUT_OUTPUT );
  FIELDDB_ELEMENT( MagneticGeometryEffect, escapeDistance, INPUT_OUTPUT );
  FIELDDB_ELEMENT( MagneticGeometryEffect, active, OUTPUT_ONLY );
  FIELDDB_ELEMENT( MagneticGeometryEffect, springConstant, INPUT_OUTPUT );
  FIELDDB_ELEMENT( MagneticGeometryEffect, geometry, INPUT_OUTPUT );

}

/// Constructor
MagneticGeometryEffect::MagneticGeometryEffect(
                     Inst< SFBool  > _enabled,
                     Inst< SFNode  >  _metadata,
                     Inst< SFFloat > _startDistance,
                     Inst< SFFloat > _escapeDistance,
                     Inst< SFBool  > _active,
                     Inst< SFFloat > _springConstant,
                     Inst< SFGeometry > _geometry ) :
  H3DForceEffect( _metadata ),
  enabled( _enabled ),
  startDistance( _startDistance ),
  escapeDistance( _escapeDistance ),
  active( _active ),
  springConstant( _springConstant ),
  geometry( _geometry ) {
  
  type_name = "MagneticGeometryEffect";

  database.initFields( this );
  
  enabled->setValue( true );
  startDistance->setValue( 0.01f );
  escapeDistance->setValue( 0.01f );
  active->setValue( false, id );
  springConstant->setValue( 100 );
  geometry->setValue( 0 );
}

// TODO: remove counter when Sensable fixes the bug with invalid positions
// at startup. HLAPI reports the wrong position for the first couple of loops
// which can cause the spring to be added even though it should not.
int magneticgeometry_counter = 0;
void MagneticGeometryEffect::traverseSG( TraverseInfo &ti ) {
  if( magneticgeometry_counter < 5 ) {
    magneticgeometry_counter++;
  } else {
    if( ti.hapticsEnabled()  ) {
      if( enabled->getValue() ) {
        const vector< H3DHapticsDevice * > &devices = ti.getHapticsDevices();
        for( unsigned int i = 0; i < devices.size(); i++ ) {
          H3DHapticsDevice *hd = devices[i];
          const Vec3f &pos = ti.getAccInverseMatrix() *
            hd->trackerPosition->getValue();
          X3DGeometryNode * the_geometry = geometry->getValue();

          if( the_geometry ) {
            NodeIntersectResult result;
            the_geometry->closestPoint( pos, result );
            result.transformResult();
            H3DFloat distance = (H3DFloat)( 
              (result.result.front().point - pos).length() );
            bool add_force_effect = false;
            if( active->getValue() ) {
              if( distance >= escapeDistance->getValue() ) {
                active->setValue( false, id );
              } else {
                add_force_effect = true;
              }
            }
            else {
              if( distance <= startDistance->getValue() )
                add_force_effect = true;
            }

            if( add_force_effect ) {
              if( dynamic_cast< Sphere * >(the_geometry) ) {
                Vec3f scale = ti.getAccForwardMatrix().getScalePart();
                ti.addForceEffect( i,
                  new HapticShapeConstraint(
                    new HAPI::Collision::Sphere(
                      ti.getAccForwardMatrix() * Vec3f(),
                      dynamic_cast< Sphere * >(the_geometry)
                        ->radius->getValue() * max( scale.x,
                          max( scale.y, scale.z ) ) ),
                      springConstant->getValue() ) );
              } else {
                H3DFloat lookahead_factor = 3;
                HapticsOptions *haptics_options = NULL;
                the_geometry->getOptionNode( haptics_options );

                if( haptics_options )
                  lookahead_factor =
                  haptics_options->lookAheadFactor->getValue();

                Matrix4f to_local = ti.getAccInverseMatrix();
                Vec3f scale = to_local.getScalePart();
                Vec3f local_proxy =  to_local * hd->proxyPosition->getValue();
                Vec3f local_last_proxy =
                  to_local * hd->getPreviousProxyPosition();
                Vec3f movement = local_proxy - local_last_proxy;
                H3DFloat addDistance = 0.01f;
                H3DFloat move_length = movement.length();
                if( move_length > addDistance )
                  addDistance = move_length;
                vector< HAPI::Collision::Triangle > tris;
                tris.reserve( 200 );
                vector< HAPI::Collision::LineSegment > lines;
                lines.reserve( 200 );
                vector< HAPI::Collision::Point > points;
                points.reserve( 200 );
                the_geometry->boundTree->getValue()
                  ->getPrimitivesIntersectedByMovingSphere(
                    ( distance + addDistance ) *
                    H3DMax( scale.x, H3DMax( scale.y, scale.z ) ),
                    local_proxy,
                    local_proxy + movement * lookahead_factor,
                    tris,
                    lines,
                    points );
                vector< HAPI::Collision::GeometryPrimitive * > primitives;
                primitives.reserve( 200 );
                for( unsigned int j = 0; j < tris.size(); j++ ) {
                  primitives.push_back(
                    new HAPI::Collision::Triangle(
                      tris[j].a, tris[j].b, tris[j].c,
                      tris[j].ta, tris[j].tb, tris[j].tc ) );
                }
                for( unsigned int j = 0; j < lines.size(); j++ ) {
                  primitives.push_back(
                    new HAPI::Collision::LineSegment( lines[j].start,
                                                      lines[j].end ) );
                }
                for( unsigned int j = 0; j < points.size(); j++ ) {
                  primitives.push_back(
                    new HAPI::Collision::Point( points[j].position ) );
                }
                /// The HapticPrimitiveSet hold a reference to all primitives.
                HapticPrimitiveSet *haptic_primitive_set =
                  new HapticPrimitiveSet( ti.getAccForwardMatrix(),
                                          primitives, NULL );
                ti.addForceEffect( i,
                  new HapticShapeConstraint( haptic_primitive_set,
                                             springConstant->getValue() ) );
              }
            }
          }
        }
      }
    }
  }
}