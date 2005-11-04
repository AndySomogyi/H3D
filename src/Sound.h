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
/// \file Sound.h
/// \brief Header file for Sound, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SOUND_H__
#define __SOUND_H__


#include "X3DSoundNode.h"
#include "X3DSoundSourceNode.h"
#include "al.h"
#include "alc.h"
#include "alut.h"

namespace H3D {

  /// \ingroup Nodes
  /// \class Sound
  /// \brief The Sound node specifies the spatial presentation of a sound in
  /// a X3D scene. 
  ///
  /// The sound is located at a point in the local coordinate system
  /// and emits sound in an elliptical pattern (defined by two
  /// ellipsoids). The ellipsoids are oriented in a direction specified by
  /// the direction field. The shape of the ellipsoids may be modified to
  /// provide more or less directional focus from the location of the
  /// sound. 
  class Sound : public X3DSoundNode {
  public:
    
    class SFSoundSourceNode: public TypedSFNode< X3DSoundSourceNode > {
    public:
      /// Destructor.
      ~SFSoundSourceNode() { value = NULL; }
    protected:
      virtual void onAdd( Node *n );
      virtual void onRemove( Node *n );
    };
    
    class ALSoundSetup: public PeriodicUpdate< Field > {
    public:
      virtual void update();
    };


    Sound( Inst< SFNode >  _metadata   = 0,
           Inst< SFVec3f>  _direction  = 0,
           Inst< SFFloat>  _intensity  = 0,
           Inst< SFVec3f>  _location   = 0,
           Inst< SFFloat>  _maxBack    = 0,
           Inst< SFFloat>  _maxFront   = 0,
           Inst< SFFloat>  _minBack    = 0,
           Inst< SFFloat>  _minFront   = 0,
           Inst< SFFloat>  _priority   = 0,
           Inst< SFSoundSourceNode >  _source     = 0,
           Inst< SFBool >  _spatialize = 0,
           Inst< ALSoundSetup > _soundSetup = 0 );

    virtual void traverseSG( TraverseInfo &ti );

    auto_ptr< SFVec3f >  direction;
    auto_ptr< SFFloat >  intensity;
    auto_ptr< SFVec3f >  location;
    auto_ptr< SFFloat >  maxBack;
    auto_ptr< SFFloat >  maxFront;
    auto_ptr< SFFloat >  minBack;
    auto_ptr< SFFloat >  minFront;
    auto_ptr< SFFloat >  priority;
    auto_ptr< SFSoundSourceNode  >  source;
    auto_ptr< SFBool  >  spatialize;
    auto_ptr< ALSoundSetup > soundSetup;

    static H3DNodeDatabase database;
  protected:
    public:
    static bool alut_initialized;
    // make static ??
    static ALCdevice *al_device;
    static ALCcontext *al_context;

  };
}

#endif
