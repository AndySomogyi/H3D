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
/// \file Sound.cpp
/// \brief CPP file for Sound, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "Sound.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Sound::database( 
                                "Sound", 
                                &(newInstance<Sound>), 
                                typeid( Sound ) );

namespace SoundInternals {
  FIELDDB_ELEMENT( Sound, direction, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, intensity, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, location, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, maxBack, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, maxFront, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, metadata, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, minBack, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, minFront, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, priority, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, source, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, spatialize, INPUT_OUTPUT );
}


Sound::Sound( 
             Inst< SFVec3f > _direction,
             Inst< SFFloat > _intensity,
             Inst< SFVec3f > _location,
             Inst< SFFloat > _maxBack,
             Inst< SFFloat > _maxFront,
             Inst< SFNode  > _metadata,
             Inst< SFFloat > _minBack,
             Inst< SFFloat > _minFront,
             Inst< SFFloat > _priority,
             Inst< SFNode  > _source,
             Inst< SFBool  > _spatialize ) :
  X3DSoundNode( _metadata ),
  direction ( _direction  ),
  intensity ( _intensity  ),
  location  ( _location   ),
  maxBack   ( _maxBack    ),
  maxFront  ( _maxFront   ),
  minBack   ( _minBack    ),
  minFront  ( _minFront   ),
  priority  ( _priority   ),
  source    ( _source     ),
  spatialize( _spatialize ) {

  cerr << "Warning: Sound unimplemented!" << endl; 

  //direction =   0 0 1 (-8,8);
  //intensity =   1     [0,1];
  //location =    0 0 0 (-8,8);
  //maxBack =     10    [0,8);
  //maxFront =    10    [0,8);
  //metadata =    NULL  [X3DMetadataObject];
  //minBack =     1     [0,8);
  //minFront =    1     [0,8);
  //priority =    0     [0,1];
  //source =      NULL  [X3DSoundSourceNode];
  //spatialize =  TRUE;


  type_name = "Sound";
  direction->setName( "Sound.direction" );
  intensity->setName( "Sound.intensity" );
  location->setName( "Sound.location" );
  maxBack->setName( "Sound.maxBack" );
  maxFront->setName( "Sound.maxFront" );
  minBack->setName( "Sound.minBack" );
  minFront->setName( "Sound.minFront" );
  priority->setName( "Sound.priority" );
  source->setName( "Sound.source" );
  spatialize->setName( "Sound.spatialize" );

}


