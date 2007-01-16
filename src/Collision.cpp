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
/// \file Collision.cpp
/// \brief CPP file for Collision, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "Collision.h"
#include "Scene.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Collision::database( 
                                    "Collision", 
                                    &(newInstance<Collision>), 
                                    typeid( Collision ),
                                    &X3DGroupingNode::database );

namespace CollisionInternals {
  FIELDDB_ELEMENT( Collision, collideTime, OUTPUT_ONLY );
  FIELDDB_ELEMENT( Collision, enabled, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Collision, isActive, OUTPUT_ONLY );
  FIELDDB_ELEMENT( Collision, proxy, INPUT_OUTPUT );
}



Collision::Collision( Inst< MFChild > _addChildren,
                      Inst< MFChild > _removeChildren,
                      Inst< SFBool  > _enabled,
                      Inst< MFChild > _children,
                      Inst< SFNode  > _metadata,
                      Inst< SFTime  > _collideTime,
                      Inst< SFBool  > _isActive,
                      Inst< SFBound > _bound,
                      Inst< SFVec3f > _bboxCenter,
                      Inst< SFVec3f > _bboxSize,
                      Inst< SFChild > _proxy) :
  X3DGroupingNode( _addChildren, _removeChildren, _children,
                   _metadata, _bound, _bboxCenter, _bboxSize ),
  enabled        ( _enabled ),
  collideTime    ( _collideTime ),
  isActive       ( _isActive ),
  proxy          ( _proxy ) {
  
  type_name = "Collision";
  database.initFields( this );

  enabled->setValue( true );
  isActive->setValue( false, id );
}

bool Collision::lineIntersect(
                  const Vec3f &from, 
                  const Vec3f &to,    
                  vector< HAPI::Bounds::IntersectionInfo > &result,
                  vector< X3DGeometryNode * > &theGeometry,
                  vector< H3DInt32 > &theGeometryIndex,
                  const Matrix4f &current_matrix,
                  vector< Matrix4f > &geometry_transforms ) {
  if( enabled->getValue() ) {
    X3DChildNode * temp_proxy = proxy->getValue();
    if( temp_proxy )
      return temp_proxy->lineIntersect( from, to, result, theGeometry,
                                        theGeometryIndex, current_matrix,
                                        geometry_transforms );
    else
      return X3DGroupingNode::lineIntersect(  from, to, result, theGeometry,
                                              theGeometryIndex, current_matrix,
                                              geometry_transforms );
  }
  return false;
}

void Collision::closestPoint(
                  const Vec3f &p,
                  vector< Vec3f > &closest_point,
                  vector< Vec3f > &normal,
                  vector< Vec3f > &tex_coord ) {
  if( enabled->getValue() ) {
    X3DChildNode * temp_proxy = proxy->getValue();
    if( temp_proxy )
      return temp_proxy->closestPoint( p, closest_point, normal, tex_coord );
    else
      return X3DGroupingNode::closestPoint( p, closest_point, normal,
             tex_coord);
  }
}

bool Collision::movingSphereIntersect( H3DFloat radius,
                                       const Vec3f &from, 
                                       const Vec3f &to ) {
  if( enabled->getValue() ) {
    bool intersect = false;
    X3DChildNode * temp_proxy = proxy->getValue();
    if( temp_proxy )
      intersect = temp_proxy->movingSphereIntersect( radius, from, to );
    else
      intersect = X3DGroupingNode::movingSphereIntersect( radius, from, to );

    if( intersect ) {
      if( !isActive->getValue() ) {
        isActive->setValue( true, id );
        collideTime->setValue( Scene::time->getValue(), id );
      }
    }
    else {
      if( isActive->getValue() )
        isActive->setValue( false, id );
    }
    return intersect;
  }
  return false;
}