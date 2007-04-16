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
/// \file X3DEnvironmentalSensorNode.h
/// \brief Header file for X3DEnvironmentalSensorNode, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __X3DENVIRONMENTALSENSORNODE_H__
#define __X3DENVIRONMENTALSENSORNODE_H__

#include "X3DSensorNode.h"
#include "SFBool.h"
#include "SFVec3f.h"
#include "SFTime.h"


namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DEnvironmentalSensorNode
  /// \brief This abstract node type is the base type for all sensors. 
  ///
  /// 
  class H3DAPI_API X3DEnvironmentalSensorNode : public X3DSensorNode {
  public:

	/// The CheckVolume class is specialize to check whether any 
    /// value of the size is zeor or not, if any it disables the 
    /// node .
    class H3DAPI_API CheckVolume: public AutoUpdate< SFVec3f > {
    public:
      virtual void setValue( const Vec3f &b, int id = 0 ) {
		  SFVec3f::setValue( b );
        X3DEnvironmentalSensorNode *esn = 
          static_cast< X3DEnvironmentalSensorNode * >( getOwner() );
		if(  value.x <= 0.0 ||
			 value.y <= 0.0 ||
			 value.z <= 0.0 )
			 esn->enabled->setValue( false, esn->id );
        else esn->enabled->setValue( true, esn->id );
      }
    protected:
      virtual void update() {
        SFVec3f::update();
        X3DEnvironmentalSensorNode *esn = 
          static_cast< X3DEnvironmentalSensorNode * >( getOwner() );
        if(  value.x <= 0.0 ||
			 value.y <= 0.0 ||
			 value.z <= 0.0 )
			 esn->enabled->setValue( false, esn->id );
        else esn->enabled->setValue( true, esn->id );
      }
    };
    
    /// Constructor.
    X3DEnvironmentalSensorNode( 
                Inst< SFNode >  _metadata   = 0,
								Inst< SFVec3f > _center     = 0,
								Inst< SFBool >  _enabled    = 0,
								Inst< SFVec3f > _size       = 0,
								Inst< SFTime >  _enterTime  = 0,
								Inst< SFTime >  _exitTime   = 0,
								Inst< SFBool >  _isActive   = 0 );


    // Fields
    /// Center gives the space location of the box
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> SFVec3f = 0,0,0 \n
    /// 
    /// \dotfile X3DEnvironmentalSensorNode_center.dot
    auto_ptr< SFVec3f >  center;

    /// Size gives the size of the box (width=x, height=y, depth=z) 
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> SFVec3f = 0,0,0 \n
    /// 
    /// \dotfile X3DEnvironmentalSensorNode_size.dot
    auto_ptr< SFVec3f >  size;

    /// enterTime
    /// <b>Access type:</b> OutputOnly \n
    /// <b>Default value:</b> SFTime \n
    /// 
    /// \dotfile X3DEnvironmentalSensorNode_enterTime.dot
	  auto_ptr< SFTime >  enterTime;
    
    /// exitTime
    /// <b>Access type:</b> OutputOnly \n
    /// <b>Default value:</b> SFTime \n
    /// 
    /// \dotfile X3DEnvironmentalSensorNode_exitTime.dot
	  auto_ptr< SFTime >  exitTime;

	/// The field checking the size
    /// 
    /// <b>Access type:</b> inputOnly \n
    /// 
    /// \dotfile X3DEnvironmentalSensorNode_checkVolume.dot

  	  auto_ptr< CheckVolume >  checkVolume;



    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  };
}

#endif
