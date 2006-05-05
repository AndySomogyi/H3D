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
/// \file BooleanSequencer.cpp
/// \brief CPP file for BooleanSequencer, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "BooleanSequencer.h"

using namespace H3D;

H3DNodeDatabase BooleanSequencer::database( 
        "BooleanSequencer", 
        &(newInstance< BooleanSequencer > ),
        typeid( BooleanSequencer ),
        &X3DSequencerNode::database 
        );

namespace BooleanSequencerInternals {
  FIELDDB_ELEMENT( BooleanSequencer, keyValue, INPUT_OUTPUT );
  FIELDDB_ELEMENT( BooleanSequencer, value_changed, OUTPUT_ONLY );
}

BooleanSequencer::BooleanSequencer( 
										Inst< SFNode > _metadata,
										Inst< KeyValues1 < MFBool > > _keyValue,
										Inst< ValueChanged < SFBool, MFBool > > _value_changed ) :
  X3DSequencerNode( _metadata      ),
  keyValue        ( _keyValue      ),
  value_changed		( _value_changed ) {

  type_name = "BooleanSequencer";

	next->route( value_changed, id );
	previous->route( value_changed, id );
	set_fraction->route( value_changed, id );
	key->route( value_changed, id );
	keyValue->route( value_changed, id );

  database.initFields( this );
}