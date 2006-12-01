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
/// \file X3DPointingDeviceSensorNode.h
/// \brief Header file for X3DPointingDeviceSensorNode, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __X3DPOINTINGDEVICESENSORNODE_H__
#define __X3DPOINTINGDEVICESENSORNODE_H__

#include "X3DSensorNode.h"
#include "X3DGroupingNode.h"
#include "SFString.h"
#include "MouseSensor.h"

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DPointingDeviceSensorNode
  /// \brief This abstract node type is the base type for all 
  /// pointing device sensors.
  ///
  /// Pointing-device sensors detect user pointing events such as the user
  /// clicking on a piece of geometry (i.e., TouchSensor). The following node
  /// types are pointing-device sensors:
  /// <ul>
  /// <li>CylinderSensor</a></li>
  /// <li>PlaneSensor</a></li>
  /// <li>SphereSensor</a></li>
  /// <li>TouchSensor</a></li>
  /// </ul>
  ///
  /// A pointing-device sensor is activated when the user locates the pointing
  /// device over geometry that is influenced by that specific pointing-device
  /// sensor. Pointing-device sensors have influence over all geometry that is
  /// descended from the sensor's parent groups. Typically, the pointing-device
  /// sensor is a sibling to the geometry that it influences. In other cases,
  /// the sensor is a sibling to groups which contain geometry
  /// (i.e., are influenced by the pointing-device sensor).
  ///
  /// The appearance properties of the geometry do not affect activation of the
  /// sensor. In particular, transparent materials or textures shall be treated
  /// as opaque with respect to activation of pointing-device sensors.
  ///
  /// For a given user activation, the lowest enabled pointing-device sensor in
  /// the hierarchy is activated. All other pointing-device sensors above the
  /// lowest enabled pointing-device sensor are ignored. The hierarchy is
  /// defined by the geometry node over which the pointing-device sensor is
  /// located and the entire hierarchy upward. If there are multiple
  /// pointing-device sensors tied for lowest, each of these is activated
  /// simultaneously and independently, possibly resulting in multiple sensors
  /// activating and generating output simultaneously. This feature allows
  /// combinations of pointing-device sensors
  /// (e.g., TouchSensor and PlaneSensor). If a pointing-device sensor appears
  /// in the transformation hierarchy multiple times (DEF/USE), it shall be
  /// tested for activation in all of the coordinate systems in which it
  /// appears.
  ///
  /// If a pointing-device sensor is not enabled when the pointing-device
  /// button is activated, it will not generate events related to the pointing
  /// device until after the pointing device is deactivated and the sensor is
  /// enabled (i.e., enabling a sensor in the middle of dragging does not
  /// result in the sensor activating immediately).
  ///
  /// The pointing device controls a pointer in the virtual world. While 
  /// activated by the pointing device, a sensor will generate events as 
  /// the pointer moves. Typically the pointing device may be categorized as 
  /// either 2D (e.g., conventional mouse) or 3D (e.g., wand). It is suggested
  /// that the pointer controlled by a 2D device is mapped onto a plane a fixed
  /// distance from the viewer and perpendicular to the line of sight. The
  /// mapping of a 3D device may describe a 1:1 relationship between movement
  /// of the pointing device and movement of the pointer.
  ///
  /// The position of the pointer defines a bearing which is used to determine
  /// which geometry is being indicated. When implementing a 2D pointing device
  /// it is suggested that the bearing is defined by the vector from the viewer
  /// position through the location of the pointer. When implementing a 3D
  /// pointing device it is suggested that the bearing is defined by extending
  /// a vector from the current position of the pointer in the direction
  /// indicated by the pointer. 
  ///
  /// In all cases the pointer is considered to be indicating a specific
  /// geometry when that geometry is intersected by the bearing. If the bearing
  /// intersects multiple sensors' geometries, only the sensor nearest to the
  /// pointer will be eligible for activation.
  ///
  /// TODO: only use the lowest sensor(s) in the hierarchy.
  /// TODO: Enabling mean to not generate events until the mousebutton has been released once.
  /// TODO: Allowing for 3D pointing device. ( which could be a haptics device perhaps)
  /// TODO: Only activate sensor nearest to pointer e.g. depth testing.
  /// TODO: Only generate true if the touchsensors geometry is not occluded by other geometry.
  
  class H3DAPI_API X3DPointingDeviceSensorNode : 
    public X3DSensorNode {
  public:

    /// The IsOver class is specialize to detect if the X3DPointingDeviceSensorNode
    /// is over a contained geometry. In that case it will call a virtual function
    /// which decides what happens for the different implementations of the
    /// X3DPointingDeviceSensorNode.
    ///
    /// routes_in[0] is the position field of a MouseSensor
    class H3DAPI_API SetIsOver: public AutoUpdate< TypedField < SFBool, SFVec2f > > {
    public:
      virtual void setValue( const bool &b, int id = 0 ) {
        SFBool::setValue( b, id );
      }
    protected:
      virtual void update() {
        SFBool::update();
        X3DPointingDeviceSensorNode *pdsn = 
          static_cast< X3DPointingDeviceSensorNode * >( getOwner() );
        if( pdsn->enabled->getValue() ) {
          bool itIsOver = false;
          if( !pdsn->parentNodes.empty() )
          {
            Vec2f mousePos2d = static_cast< SFVec2f * >( routes_in[0] )->getValue();
            GLint viewport[4];
            GLdouble mvmatrix[16], projmatrix[16];
            GLdouble wx, wy, wz;
            glGetIntegerv( GL_VIEWPORT, viewport );
            glGetDoublev( GL_MODELVIEW_MATRIX, mvmatrix );
            glGetDoublev( GL_PROJECTION_MATRIX, projmatrix );
            mousePos2d.y = viewport[3] - (GLint ) mousePos2d.y - 1;
            gluUnProject( (GLdouble) mousePos2d.x, (GLdouble) mousePos2d.y,
              0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
            Vec3f nearPlanePos = Vec3f( wx, wy, wz );
            gluUnProject( (GLdouble) mousePos2d.x, (GLdouble) mousePos2d.y,
              1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
            Vec3f farPlanePos = Vec3f( wx, wy, wz );

            for( unsigned int i = 0; i < pdsn->parentNodes.size(); i++ ) {
              HAPI::Bounds::IntersectionInfo result;
              if( pdsn->parentNodes[i]->lineIntersect( nearPlanePos, farPlanePos, result, true ) ) {
                itIsOver = true;
                pdsn->onIsOver( result );
              }
            }
          }
          if( pdsn->isOver->getValue() != itIsOver ) {
            pdsn->isOver->setValue( itIsOver, pdsn->id );
          }
        }
      }
    };
#ifdef __BORLANDC__
    friend class IsOver;
#endif

    /// Constructor.
    X3DPointingDeviceSensorNode( Inst< SFString > _description = 0,
                                 Inst< SFBool >  _enabled  = 0,
                                 Inst< SFNode >  _metadata = 0,
                                 Inst< SFBool >  _isActive = 0,
                                 Inst< SFBool > _isOver = 0 );

    // Fields
    /// The description field in a X3DPointingDeviceSensorNode node specifies a
    /// textual description of the X3DPointingDeviceSensorNode node. This may 
    /// be used by browser-specific user interfaces that wish to present users 
    /// with more detailed information about the X3DPointingDeviceSensorNode.
    ///
    /// <b>Access type:</b> inputOutput
    /// 
    /// \dotfile X3DPointingDeviceSensorNode_description.dot
    auto_ptr< SFString > description;

    auto_ptr< SFBool > isOver;

    void addGroupNode( X3DGroupingNode * n ) {
      parentNodes.push_back( n );
    }

    void removeGroupNode( X3DGroupingNode * n ) {
      vector< X3DGroupingNode * >::iterator i;
      bool found = false;
      for( i = parentNodes.begin(); i < parentNodes.end(); i++ )
        if( *i == n ) {
          found = true;
          break;
        }
      if( found )
        parentNodes.erase( i );
    }

    virtual void onIsOver( HAPI::Bounds::IntersectionInfo &result ){};

    /*virtual void initialize();*/

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  protected:
    static MouseSensor *mouseSensor;
    auto_ptr< SetIsOver > setIsOver;
    vector< X3DGroupingNode * > parentNodes;
  };
}

#endif
