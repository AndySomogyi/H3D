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
/// \file X3DScriptNode.cpp
/// \brief cpp file for X3DScriptNode
///
//
//////////////////////////////////////////////////////////////////////////////
#include <X3DScriptNode.h>

using namespace H3D;

H3DNodeDatabase X3DScriptNode::database( 
        "X3DScriptNode", 
        NULL,
        typeid( X3DScriptNode ),
        &X3DChildNode::database 
        );

namespace X3DScriptNodeInternals {
  FIELDDB_ELEMENT( X3DScriptNode, url, INITIALIZE_ONLY );
}

X3DScriptNode::X3DScriptNode( Inst< SFNode>  _metadata,
                              Inst< MFString > _url ) : 
  X3DChildNode( _metadata ),
  X3DUrlObject( _url ) {
  type_name = "X3DScriptNode";
  database.initFields( this );
}
