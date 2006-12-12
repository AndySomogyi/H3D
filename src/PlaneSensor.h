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
/// \file PlaneSensor.h
/// \brief Header file for PlaneSensor, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __PLANESENSOR_H__
#define __PLANESENSOR_H__

#include "X3DDragSensorNode.h"

namespace H3D {

  /// \ingroup X3DNodes
  /// \class PlaneSensor
  /// \brief The PlaneSensor node maps pointing device motion into
  /// two-dimensional translation in a plane parallel to the Z=0 plane of the
  /// local coordinate system. The PlaneSensor node uses the descendent
  /// geometry of its parent node to determine whether it is liable to
  /// generate events.
  ///
  /// The description field in the PlaneSensor node specifies a textual
  /// description of the PlaneSensor node. This may be used by browser-specific
  /// user interfaces that wish to present users with more detailed information
  /// about the PlaneSensor.
  ///
  /// The enabled field enables and disables the PlaneSensor. If enabled is
  /// TRUE, the sensor reacts appropriately to user events. If enabled is
  /// FALSE, the sensor does not track user input or send events. If enabled
  /// receives a FALSE event and isActive is TRUE, the sensor becomes disabled
  /// and deactivated, and outputs an isActive FALSE event. If enabled receives
  /// a TRUE event, the sensor is enabled and made ready for user activation.
  ///
  /// The PlaneSensor node generates events when the pointing device is
  /// activated while the pointer is indicating any descendent geometry nodes
  /// of the sensor's parent group. See 20.2.3 Activating and manipulating
  /// pointing device sensors, for details on using the pointing device to
  /// activate the PlaneSensor.
  ///
  /// Upon activation of the pointing device (e.g., mouse button down) while
  /// indicating the sensor's geometry, an isActive TRUE event is sent. Pointer
  /// motion is mapped into relative translation in the tracking plane,
  /// (a plane parallel to the sensor's local Z=0 plane and coincident with
  /// the initial point of intersection). For each subsequent movement of the
  /// bearing, a translation_changed event is output which corresponds to the
  /// sum of the relative translation from the original intersection point to
  /// the intersection point of the new bearing in the plane plus the offset
  /// value. The sign of the translation is defined by the Z=0 plane of the
  /// sensor's coordinate system. trackPoint_changed events reflect the
  /// unclamped drag position on the surface of this plane. When the pointing
  /// device is deactivated and autoOffset is TRUE, offset is set to the last
  /// translation_changed value and an offset_changed event is generated.
  /// 
  /// minPosition and maxPosition may be set to clamp translation_changed 
  /// events to a range of values as measured from the origin of the Z=0 plane.
  /// If the X or Y component of minPosition is greater than the corresponding
  /// component of maxPosition, translation_changed events are not clamped in
  /// that dimension. If the X or Y component of minPosition is equal to the
  /// corresponding component of maxPosition, that component is constrained to
  /// the given value. This technique provides a way to implement a line sensor
  /// that maps dragging motion into a translation in one dimension.
  ///
  /// While the pointing device is activated and moved, trackPoint_changed and
  /// translation_changed events are sent. trackPoint_changed events represent
  /// the unclamped intersection points on the surface of the tracking plane.
  /// If the pointing device is dragged off of the tracking plane while
  /// activated (e.g., above horizon line), browsers may interpret this in a
  /// variety ways (e.g., clamp all values to the horizon. H3DAPI resend the
  /// last event and write an error message ). Each movement of
  /// the pointing device, while isActive is TRUE, generates trackPoint_changed
  /// and translation_changed events.
  
  class H3DAPI_API PlaneSensor : 
    public X3DDragSensorNode {
  public:


    /// The Set_Translation_Changed 
    ///
    /// routes_in[0] is the position field of a MouseSensor
    /// routes_in[1] is the isActive field.
    class H3DAPI_API Set_Events: 
      public AutoUpdate< TypedField < SFBool, Types< SFVec2f, SFBool > > > {
    public:
      virtual void setValue( const bool &b, int id = 0 ) {
        SFBool::setValue( b, id );
      }
    protected:
      virtual void update() {
        SFBool::update();
        PlaneSensor *ps = 
          static_cast< PlaneSensor * >( getOwner() );
        if( ps->enabled->getValue() ) {
          bool isActive = static_cast< SFBool * >(routes_in[1])->getValue();
          if( isActive ) {
            if( ps->getTrackPlane ) {
              ps->originalIntersection = ps->oldIntersection;
              ps->originalGeometry = ps->oldGeometry;
              ps->getTrackPlane = false;
              ps->planeNormal = Vec3f( 0, 0, 1);
              ps->planeD = ps->planeNormal * ps->originalIntersection;
              ps->trackPoint_changed->setValue( ps->originalIntersection, ps->id );
            }
            else {
              H3DFloat t;
              Vec3f intersectionPoint;
              if( ps->intersectSegmentPlane( ps->geometryMatrices[ps->oldGeometry] * nearPlanePos, ps->geometryMatrices[ps->oldGeometry] * farPlanePos, t, intersectionPoint ) ) {
                Vec3f translation_changed = intersectionPoint - ps->originalIntersection + ps->offset->getValue();
                if( ps->minPosition->getValue().x <= ps->maxPosition->getValue().x ) {
                  translation_changed.x = ps->Clamp( translation_changed.x, ps->minPosition->getValue().x, ps->maxPosition->getValue().x );
                }
                if( ps->minPosition->getValue().y <= ps->maxPosition->getValue().y ) {
                  translation_changed.y = ps->Clamp( translation_changed.y, ps->minPosition->getValue().y, ps->maxPosition->getValue().y );
                }
                ps->trackPoint_changed->setValue( intersectionPoint, ps->id );
                ps->translation_changed->setValue( translation_changed , ps->id );
              }
              else {
                cerr << "Outside the plane due to near- and farplane clipping or other reason, last event resent." << endl;
                ps->trackPoint_changed->touch();
                ps->translation_changed->touch();
              }
            }
          }
          else
            if( !ps->getTrackPlane ) {
              ps->getTrackPlane = true;
              if( ps->autoOffset->getValue() )
                ps->offset->setValue( ps->translation_changed->getValue(), ps->id );
            }
        }
      } 
    };
#ifdef __BORLANDC__
    friend class Set_Translation_Changed;
#endif

    /// Constructor.
    PlaneSensor(      Inst< SFBool >  _autoOffset = 0,
                      Inst< SFString > _description = 0,
                      Inst< SFBool >  _enabled = 0,
                      Inst< SFVec2f > _maxPosition = 0,
                      Inst< SFNode >  _metadata = 0,
                      Inst< SFVec2f > _minPosition = 0,
                      Inst< SFVec3f > _offset = 0,
                      Inst< SFBool >  _isActive = 0,
                      Inst< SFBool > _isOver = 0,
                      Inst< SFVec3f >  _trackPoint_changed = 0,
                      Inst< SFVec3f >  _translation_changed = 0);

    ~PlaneSensor();

    // fields
    /// maxPosition can be used to clamp translation_changed events.
    ///
    /// <b>Access type:</b> inputOutput \n
    ///
    /// <b>Default value:</b> -1 -1 \n
    ///
    /// \dotfile PlaneSensor_maxPosition.dot
    auto_ptr< SFVec2f > maxPosition;

    /// minPosition can be used to clamp translation_changed events
    ///
    /// <b>Access type:</b> inputOutput \n
    ///
    /// <b>Default value:</b> 0 0 \n
    ///
    /// \dotfile PlaneSensor_minPosition.dot
    auto_ptr< SFVec2f > minPosition;

    /// Stores the last translation_changed value at deactivation
    /// if autoOffset is true.
    ///
    /// <b>Access type:</b> inputOutput \n
    ///
    /// <b>Default value:</b> 0 0 0 \n
    ///
    /// \dotfile PlaneSensor_offset.dot
    auto_ptr< SFVec3f > offset;

    /// For each subsequent movement of the bearing, a translation_changed
    /// event is output which corresponds to the sum of the relative
    /// translation from the original intersection point to the intersection
    /// point of the new bearing in the plane plus the offset value.
    ///
    /// <b>Access type:</b> outputOnly \n
    ///
    /// \dotfile PlaneSensor_translation_changed.dot
    auto_ptr< SFVec3f > translation_changed;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  protected:
    auto_ptr< Set_Events > set_Events;
    /// Called to generate isOver events and other events (dependent on isOver)
    // if they should be generated.
    virtual void onIsOver( bool newValue,
                           HAPI::Bounds::IntersectionInfo &result,
                           int geometryIndex );

    
    int intersectSegmentPlane( Vec3f a, Vec3f b, float &t, Vec3f &q );

    H3DFloat Clamp( H3DFloat n, H3DFloat min, H3DFloat max );

    Vec3f originalIntersection, oldIntersection;
    int originalGeometry, oldGeometry;
    bool getTrackPlane;

    H3DFloat planeD;
    Vec3f planeNormal;
  };
}

#endif
