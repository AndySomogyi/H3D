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
/// \file TouchSensor.h
/// \brief Header file for TouchSensor, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __TOUCHSENSOR_H__
#define __TOUCHSENSOR_H__

#include "X3DTouchSensorNode.h"
#include "X3DShapeNode.h"
#include "X3DGeometryNode.h"
#include "SFVec3f.h"
#include "SFVec2f.h"

namespace H3D {

  /// \ingroup X3DNodes
  /// \class TouchSensor
  /// \brief A TouchSensor node tracks the location and state of the pointing
  /// device and detects when the user points at geometry contained by 
  /// the TouchSensor node's parent group.
  ///
  
  class H3DAPI_API TouchSensor : 
    public X3DTouchSensorNode {
  public:

    /// The IsActive class is specialize to send true events when isOver
    /// is true and the primary mouse button is pressed until the primary
    /// mouse button is released.
    ///
    /// routes_in[0] is the isOver field
    /// routes_in[1] is the leftButton field of a MouseSensor
    class H3DAPI_API SetIsActive: public AutoUpdate< TypedField < SFBool, Types< SFBool, SFBool > > > {
    public:
      SetIsActive() {
        leftMousePressedOutside = false;
      }

      virtual void setValue( const bool &b, int id = 0 ) {
        SFBool::setValue( b, id );
      }
    protected:
      virtual void update() {
        SFBool::update();
        TouchSensor *ts = 
              static_cast< TouchSensor * >( getOwner() );
        if( ts->enabled->getValue() ) {
          bool itIsActive = false;
          bool leftButton = static_cast< SFBool * >( routes_in[0] )->getValue();
          bool isOver = static_cast< SFBool * >( routes_in[1] )->getValue();
          if( leftButton ) {
            if( !leftMousePressedOutside && ( isOver || ts->isActive->getValue() ) )
              itIsActive = true;
            else
              leftMousePressedOutside = true;
          }
          else {
            itIsActive = false;
            leftMousePressedOutside = false;
          }

          if( itIsActive != ts->isActive->getValue() ) {
            ts->isActive->setValue( itIsActive, ts->id );
            if( itIsActive )
              ts->startTime = TimeStamp();
            else if( ts->isOver->getValue() )
              ts->touchTime->setValue( TimeStamp() - ts->startTime, ts->id );
          }
        }
      }
      bool leftMousePressedOutside;
    };
#ifdef __BORLANDC__
    friend class IsActive;
#endif

    /// Constructor.
    TouchSensor( Inst< SFString > _description = 0,
                        Inst< SFBool >  _enabled  = 0,
                        Inst< SFNode >  _metadata = 0,
                        Inst< SFVec3f > _hitNormal_changed = 0,
                        Inst< SFVec3f > _hitPoint_changed = 0,
                        Inst< SFVec2f > _hitTexCoord_changed = 0,
                        Inst< SFBool >  _isActive = 0,
                        Inst< SFBool > _isOver = 0,
                        Inst< SFTime > _touchTime = 0 );

    /// hitPoint_changed, hitNormal_changed and hitTexCoord_changed events.


    // Fields
    auto_ptr< SFVec3f > hitNormal_changed;
    auto_ptr< SFVec3f > hitPoint_changed;
    auto_ptr< SFVec2f > hitTexCoord_changed;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

    virtual void onIsOver( HAPI::Bounds::IntersectionInfo &result ) {
      hitPoint_changed->setValue( Vec3f( result.point ), id );
      hitNormal_changed->setValue( Vec3f( result.normal ), id );
      hitTexCoord_changed->setValue( Vec2f( result.tex_coord.x, result.tex_coord.y ), id );
    }

    protected:
      auto_ptr< SetIsActive > setIsActive;
      H3DTime startTime;
  };
}

#endif
