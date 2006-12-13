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
/// \file X3DPrototypeInstance.cpp
/// \brief CPP file for X3DPrototypeInstance, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DPrototypeInstance.h"
#include "H3DDisplayListObject.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase X3DPrototypeInstance::database( 
                                               "X3DPrototypeInstance", 
                                               NULL,
                                               typeid( X3DPrototypeInstance ),
                                               &X3DNode::database );

X3DPrototypeInstance::X3DPrototypeInstance( Inst< SFNode>  _metadata ):
  X3DNode( _metadata ),
  H3DDynamicFieldsObject( &database ),
  prototyped_node( NULL ) {
  type_name = "X3DPrototypeInstance";
}

void X3DPrototypeInstance::render() {
  if( prototyped_node.get() ) {
    H3DDisplayListObject *dlo = 
      dynamic_cast< H3DDisplayListObject * >( prototyped_node.get() );
    if( dlo ) {
      dlo->displayList->callList();
    } else {
      prototyped_node.get()->render();
    }
  }
}
 
#ifdef USE_HAPTICS
void X3DPrototypeInstance::traverseSG( TraverseInfo &ti ) {
  if( prototyped_node.get() )
    prototyped_node.get()->traverseSG( ti );
}
#endif

