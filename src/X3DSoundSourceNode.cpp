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
/// \file X3DSoundSourceNode.cpp
/// \brief CPP file for X3DSoundSourceNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DSoundSourceNode.h"

using namespace H3D;


X3DSoundSourceNode::X3DSoundSourceNode( Inst< SFNode  >  _metadata,
                                        Inst< SFString>  _description,
                                        Inst< SFBool  >  _loop,
                                        Inst< SFTime  >  _pauseTime,
                                        Inst< SFFloat >  _pitch,
                                        Inst< SFTime  >  _resumeTime,
                                        Inst< StartTime  >  _startTime,
                                        Inst< StopTime  >  _stopTime,
                                        Inst<  SFTime  >  _duration_changed,
                                        Inst<  SFTime  >  _elapsedTime,
                                        Inst<  SFBool  >  _isActive,
                                        Inst<  SFBool  >  _isPaused ) :
  X3DTimeDependentNode( _metadata, _loop, _pauseTime, _resumeTime, _startTime,
                        _stopTime, _elapsedTime, _isActive, _isPaused ),
  description     ( _description      ),
  pitch           ( _pitch            ),
  duration_changed( _duration_changed ) {

  type_name = "X3DSoundSourceNode";
  description->setName( "X3DSoundSourceNode.description" );
  pitch->setName( "X3DSoundSourceNode.pitch" );
  duration_changed->setName( "X3DSoundSourceNode.duration_changed" );

  description->setValue( "" );
  pitch->setValue( 1.0 );
  duration_changed->setValue( 0 );
}


