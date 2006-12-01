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
/// \file X3DTouchSensorNode.h
/// \brief Header file for X3DTouchSensorNode, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __X3DTOUCHSENSORNODE_H__
#define __X3DTOUCHSENSORNODE_H__

#include "X3DPointingDeviceSensorNode.h"
#include "SFTime.h"

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DTouchSensorNode
  /// \brief This abstract node type is the base type for all 
  /// touch-style pointing device sensors.
  ///
  
  class H3DAPI_API X3DTouchSensorNode : 
    public X3DPointingDeviceSensorNode {
  public:

    /// Constructor.
    X3DTouchSensorNode( Inst< SFString > _description = 0,
                        Inst< SFBool >  _enabled  = 0,
                        Inst< SFNode >  _metadata = 0,
                        Inst< SFBool >  _isActive = 0,
                        Inst< SFBool > _isOver = 0,
                        Inst< SFTime > _touchTime = 0 );

    // Fields
    auto_ptr< SFTime > touchTime;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
