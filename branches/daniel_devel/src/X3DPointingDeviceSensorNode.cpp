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

MouseSensor * X3DPointingDeviceSensorNode::mouseSensor = new MouseSensor();

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
                              Inst< SFBool >  _isOver/*,
                         Inst< MFGroupingNode > _parentNodes*/ ) :
  X3DSensorNode( _enabled, _metadata, _isActive ),
  description ( _description  ),
  isOver( _isOver ),
  setIsOver( new SetIsOver ) {

  type_name = "X3DPointingDeviceSensorNode";
  database.initFields( this );

  description->setValue( "" );

  setIsOver->setOwner(this);
  mouseSensor->position->routeNoEvent( setIsOver );
}

//void X3DPointingDeviceSensorNode::initialize() {
//  mouseSensor = new MouseSenor()
//}


