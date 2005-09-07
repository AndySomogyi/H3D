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
/// \file X3DSoundSourceNode.h
/// \brief Header file for X3DSoundSourceNode, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __X3DSOUNDSOURCENODE_H__
#define __X3DSOUNDSOURCENODE_H__

#include "X3DTimeDependentNode.h"

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DSoundSourceNode
  /// \brief This abstract node type is used to derive node types that can emit
  /// audio data. 
  ///
  /// 
  class H3DAPI_API X3DSoundSourceNode : public X3DTimeDependentNode {
  public:
    
    /// Constructor.
    X3DSoundSourceNode( Inst< SFNode   > _metadata         = 0,
                        Inst< SFString > _description      = 0,
                        Inst< SFBool   > _loop             = 0,
                        Inst< SFTime   > _pauseTime        = 0,
                        Inst< SFFloat  > _pitch            = 0,
                        Inst< SFTime   > _resumeTime       = 0,
                        Inst< StartTime   > _startTime        = 0,
                        Inst< StopTime   > _stopTime         = 0,
                        Inst< SFTime   > _duration_changed = 0,
                        Inst< SFTime   > _elapsedTime      = 0,
                        Inst< SFBool   > _isActive         = 0,
                        Inst< SFBool   > _isPaused         = 0 );

    auto_ptr< SFString > description;
    auto_ptr< SFFloat  > pitch;
    auto_ptr< SFTime   > duration_changed;

  };
}

#endif
