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
/// \file AudioClip.cpp
/// \brief CPP file for AudioClip, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "AudioClip.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase AudioClip::database( "AudioClip", 
                                     &(newInstance<AudioClip>),
                                     typeid( AudioClip ) );

namespace AudioClipInternals {
  FIELDDB_ELEMENT( AudioClip, description, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, loop, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, metadata, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, pauseTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, pitch, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, resumeTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, startTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, stopTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, url, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, duration_changed, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, elapsedTime, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, isActive, INPUT_OUTPUT );
  FIELDDB_ELEMENT( AudioClip, isPaused, INPUT_OUTPUT );
}


AudioClip::AudioClip( 
                     Inst< SFString>  _description,
                     Inst< SFBool  >  _loop,
                     Inst< SFNode  >  _metadata,
                     Inst< SFTime  >  _pauseTime,
                     Inst< SFFloat >  _pitch,
                     Inst< SFTime  >  _resumeTime,
                     Inst< StartTime  >  _startTime,
                     Inst< StopTime  >  _stopTime,
                     Inst< MFString>  _url,
                     Inst<  SFTime  >  _duration_changed,
                     Inst<  SFTime  >  _elapsedTime,
                     Inst<  SFBool  >  _isActive,
                     Inst<  SFBool  >  _isPaused ) :
  X3DSoundSourceNode( _metadata, _description, _loop,  
                      _pauseTime, _pitch, _resumeTime, 
                      _startTime, _stopTime, _duration_changed, 
                      _elapsedTime, _isActive, _isPaused ),
  url( _url ) {

  cerr << "Warning: AudioClip unimplemented!" << endl; 

  //description =       "";
  //loop =              FALSE;
  //metadata =          NULL  [X3DMetadataObject];
  //pauseTime =         0     (-8,8);
  //pitch =             1.0   (0,8);
  //resumeTime =        0     (-8,8);
  //startTime =         0     (-8,8);
  //stopTime =          0     (-8,8);
  //url =               []    [urn];

  type_name = "AudioClip";

  database.initFields( this );
  
}


