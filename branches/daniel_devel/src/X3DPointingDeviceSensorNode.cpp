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
/// \file X3DPointingDeviceSensorNode.cpp
/// \brief CPP file for X3DPointingDeviceSensorNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DPointingDeviceSensorNode.h"

using namespace H3D;

MouseSensor * X3DPointingDeviceSensorNode::mouseSensor = NULL;
Vec2f X3DPointingDeviceSensorNode::posDevice2D;
Vec3f X3DPointingDeviceSensorNode::nearPlanePos;
Vec3f X3DPointingDeviceSensorNode::farPlanePos;
vector< X3DPointingDeviceSensorNode * > X3DPointingDeviceSensorNode::instances=
vector< X3DPointingDeviceSensorNode * >();
int X3DPointingDeviceSensorNode::someAreActive = 0;
H3DInt32 X3DPointingDeviceSensorNode::geometryNodeIndex = -1;

H3DNodeDatabase X3DPointingDeviceSensorNode::database( 
        "X3DPointingDeviceSensorNode", 
        NULL,
        typeid( X3DPointingDeviceSensorNode ),
        &X3DSensorNode::database 
        );

namespace X3DPointingDeviceSensorNodeInternals {
  FIELDDB_ELEMENT( X3DPointingDeviceSensorNode, description, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DPointingDeviceSensorNode, isOver, OUTPUT_ONLY );
}

X3DPointingDeviceSensorNode::X3DPointingDeviceSensorNode( 
                              Inst< SFString > _description,
                              Inst< SFBool >  _enabled,
                              Inst< SFNode >  _metadata,
                              Inst< SFBool >  _isActive,
                              Inst< SFBool >  _isOver ) :
  X3DSensorNode( _enabled, _metadata, _isActive ),
  description ( _description  ),
  isOver( _isOver ),
  setIsEnabled( new SetIsEnabled ),
  setIsActive( new SetIsActive ) {

  type_name = "X3DPointingDeviceSensorNode";
  database.initFields( this );

  description->setValue( "" );

  if( !mouseSensor ) {
    mouseSensor = new MouseSensor();
    posDevice2D = mouseSensor->position->getValue();
  }    

  isEnabled = true;
  setIsEnabled->setOwner( this );
  setIsEnabled->setValue( true );
  enabled->routeNoEvent( setIsEnabled );
  mouseSensor->leftButton->routeNoEvent( setIsEnabled );

  setIsActive->setOwner(this);
  mouseSensor->leftButton->routeNoEvent( setIsActive );
  isOver->routeNoEvent( setIsActive );

  instances.push_back( this );
}

/// Destructor. 
X3DPointingDeviceSensorNode::~X3DPointingDeviceSensorNode() {
  instances.erase( find( instances.begin(), instances.end(), this ) );
  if( instances.empty() )
    mouseSensor = NULL;
}

bool X3DPointingDeviceSensorNode::has2DPointingDeviceMoved( Vec2f & pos ) {
  Vec2f tempPos = mouseSensor->position->getValue();
  if( ( tempPos - posDevice2D ).lengthSqr() > Constants::f_epsilon ) {
    posDevice2D.x = tempPos.x;
    posDevice2D.y = tempPos.y;
    pos = tempPos;
    return true;
  }
  return false;
}

int X3DPointingDeviceSensorNode::addGeometryNode( X3DGeometryNode * n, bool newIndex ) {
  if( newIndex ) {
    geometryNodeIndex++;
  }
  geometryNodes[geometryNodeIndex] = n;
  geometryMatrices[geometryNodeIndex] = currentMatrix;
  return geometryNodeIndex;
}

void X3DPointingDeviceSensorNode::clearGeometryNodes() {
  for( int i = 0; i < instances.size(); i++ ) {
    instances[i]->geometryNodes.clear();
    instances[i]->geometryMatrices.clear();
    geometryNodeIndex = -1;
  }
}

void X3DPointingDeviceSensorNode::updateX3DPointingDeviceSensors( Node * n ) {
  if( !instances.empty() ) {
    Vec2f theMousePos;
    if( has2DPointingDeviceMoved( theMousePos ) ) {
      GLint viewport[4];
      GLdouble mvmatrix[16], projmatrix[16];
      GLdouble wx, wy, wz;
      glGetIntegerv( GL_VIEWPORT, viewport );
      glGetDoublev( GL_MODELVIEW_MATRIX, mvmatrix );
      glGetDoublev( GL_PROJECTION_MATRIX, projmatrix );

      theMousePos.y = viewport[3] - (GLint ) theMousePos.y - 1;
      gluUnProject( (GLdouble) theMousePos.x, (GLdouble) theMousePos.y,
        0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
      nearPlanePos = Vec3f( wx, wy, wz );
      gluUnProject( (GLdouble) theMousePos.x, (GLdouble) theMousePos.y,
        1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
      farPlanePos = Vec3f( wx, wy, wz );

      for( int i = 0; i < instances.size(); i++ ) {
        for( map< H3DInt32, X3DGeometryNode * >::iterator j = instances[i]->geometryNodes.begin();
          j != instances[i]->geometryNodes.end();
          j++ )
          (*j).second->resetPtDevIndication( false );
      }  

      vector< HAPI::Bounds::IntersectionInfo > result;
      vector< X3DGeometryNode * > theGeometry;
      vector< H3DInt32 > theGeometryIndex;
      if( n->lineIntersect( nearPlanePos, 
                            farPlanePos,
                            result,
                            true,
                            theGeometry,
                            theGeometryIndex ) ) {
        int closest = 0;
        if( theGeometry.size() > 1 ) {
          H3DFloat closestDistance = 
            (result[closest].point - nearPlanePos).lengthSqr();
          for( unsigned int kl = 1; kl < theGeometry.size(); kl++ ) {
            H3DFloat tempClose = (result[kl].point - nearPlanePos).lengthSqr();
            if( tempClose < closestDistance ) {
              closestDistance = tempClose;
              closest = kl;
            }
          }
        }

        for( int i = 0; i < instances.size(); i++ ) {
          int geometryIndex =
            instances[i]->findGeometry( theGeometry[closest], 
                                        theGeometryIndex[closest] );
          if( geometryIndex != -1 ){
            instances[i]->onIsOver( true, result[closest], geometryIndex );
          }
          else
            instances[i]->onIsOver( false, result[closest], geometryIndex );
        }
      }
      else {
        for( unsigned int i = 0; i < instances.size(); i++ ) {
          HAPI::Bounds::IntersectionInfo result;
          int geometryIndex = -1;
          instances[i]->onIsOver( false, result, geometryIndex );
        }
      }
    }
  }
  for( int i = 0; i < instances.size(); i++ ) {
    for( map< H3DInt32, X3DGeometryNode * >::iterator j = instances[i]->geometryNodes.begin();
         j != instances[i]->geometryNodes.end();
         j++ )
      (*j).second->resetPtDevIndication( true );
  }  
}

int X3DPointingDeviceSensorNode::findGeometry( X3DGeometryNode * n, H3DInt32 theIndex ) {
  if( isEnabled ) {
    if( geometryNodes.find( theIndex ) != geometryNodes.end()
        && geometryNodes[theIndex] == n ) {
      return theIndex;
    }

    /*for( map< int, X3DGeometryNode * >::iterator i = geometryNodes.begin();
      i != geometryNodes.end(); i++ ) {
      if( (*i).second == n )
        return (*i).first;
    }*/
    
    /*for( unsigned int i = 0; i < geometryNodes.size(); i++ )
      if( geometryNodes[i] == n )
        return i;*/
  }
  return -1;
}
