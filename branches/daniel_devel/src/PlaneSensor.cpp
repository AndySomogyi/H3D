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
/// \file PlaneSensor.cpp
/// \brief CPP file for PlaneSensor, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "PlaneSensor.h"

using namespace H3D;

H3DNodeDatabase PlaneSensor::database( 
        "PlaneSensor", 
        &(newInstance< PlaneSensor > ),
        typeid( PlaneSensor ),
        &X3DDragSensorNode::database 
        );

namespace PlaneSensorInternals {
  FIELDDB_ELEMENT( PlaneSensor, maxPosition, INPUT_OUTPUT );
  FIELDDB_ELEMENT( PlaneSensor, minPosition, INPUT_OUTPUT );
  FIELDDB_ELEMENT( PlaneSensor, offset, INPUT_OUTPUT );
  FIELDDB_ELEMENT( PlaneSensor, translation_changed, OUTPUT_ONLY );
}

PlaneSensor::PlaneSensor(
                              Inst< SFBool >  _autoOffset,
                      Inst< SFString > _description,
                      Inst< SFBool >  _enabled,
                      Inst< SFVec2f > _maxPosition,
                      Inst< SFNode >  _metadata,
                      Inst< SFVec2f > _minPosition,
                      Inst< SFVec3f > _offset,
                      Inst< SFBool >  _isActive,
                      Inst< SFBool > _isOver,
                      Inst< SFVec3f >  _trackPoint_changed,
                      Inst< SFVec3f >  _translation_changed ) :
  X3DDragSensorNode( _autoOffset, _description, _enabled,
                     _metadata, _isActive, _isOver, _trackPoint_changed ),
  maxPosition ( _maxPosition  ),
  minPosition( _minPosition ),
  offset( _offset ),
  translation_changed( _translation_changed ),
  set_Events( new Set_Events ) {

  type_name = "PlaneSensor";
  database.initFields( this );

  maxPosition->setValue( Vec2f( -1, -1 ) );
  minPosition->setValue( Vec2f( 0, 0 ) );
  offset->setValue( Vec3f( 0, 0, 0 ) );

  getTrackPlane = true;
  originalGeometry = -1;

  set_Events->setOwner( this );
  mouseSensor->position->routeNoEvent( set_Events );
  isActive->routeNoEvent( set_Events );
}

/// Destructor. 
PlaneSensor::~PlaneSensor() {
}

void PlaneSensor::onIsOver( bool newValue,
                           HAPI::Bounds::IntersectionInfo &result,
                           int geometryIndex ) {
  if( isEnabled && ( isActive->getValue() || someAreActive == 0 ) ) {
    X3DPointingDeviceSensorNode::onIsOver( newValue,
                                           result,
                                           geometryIndex );
    if( newValue ) {
      Vec3f newNormalPoint = geometryMatrices[geometryIndex]
      * Vec3f( result.point + result.normal );
      Vec3f newPoint =
        geometryMatrices[geometryIndex] * Vec3f( result.point );
      newNormalPoint = newNormalPoint - newPoint;
      newNormalPoint.normalize();
      oldIntersection = newPoint;
      oldGeometry = geometryIndex;
    }
  }
}

int PlaneSensor::intersectSegmentPlane( Vec3f a, Vec3f b, float &t, Vec3f &q ){
  // Compute the t value for the directed line ab intersecting the plane
  Vec3f ab = b - a;
  t = (planeD - planeNormal * a ) / ( planeNormal * ab );

  // If t in [0..1] compute and return intersection point
  if( t >= 0.0f && t <= 1.0f ) {
    q = a + t * ab;
    return 1;
  }

  // Else no intersection
  return 0;
}

H3DFloat PlaneSensor::Clamp( H3DFloat n, H3DFloat min, H3DFloat max ) {
	if( n < min ) return min;
	if( n > max ) return max;
	return n;
}
