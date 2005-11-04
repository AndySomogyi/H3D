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
#include "AudioClip.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Sound::database( 
                                "Sound", 
                                &(newInstance<Sound>), 
                                typeid( Sound ),
                                &X3DSoundNode::database );

bool Sound::alut_initialized = false;

namespace SoundInternals {
  FIELDDB_ELEMENT( Sound, direction, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, intensity, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, location, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, maxBack, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, maxFront, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, minBack, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, minFront, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, priority, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, source, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Sound, spatialize, INITIALIZE_ONLY );
}

ALCdevice *Sound::al_device = NULL;
ALCcontext *Sound::al_context = NULL;

Sound::Sound( 
             Inst< SFNode  > _metadata,
             Inst< SFVec3f > _direction,
             Inst< SFFloat > _intensity,
             Inst< SFVec3f > _location,
             Inst< SFFloat > _maxBack,
             Inst< SFFloat > _maxFront,
             Inst< SFFloat > _minBack,
             Inst< SFFloat > _minFront,
             Inst< SFFloat > _priority,
             Inst< SFSoundSourceNode  > _source,
             Inst< SFBool  > _spatialize,
             Inst< ALSoundSetup > _soundSetup ) :
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
  spatialize( _spatialize ),
  soundSetup( _soundSetup ) {

  database.initFields( this );
  type_name = "Sound";

  soundSetup->setName( "soundSetup" );
  soundSetup->setOwner( this );

  direction->setValue( Vec3f( 0, 0, 1 ) );
  intensity->setValue( 1 );
  location->setValue( Vec3f( 0, 0, 0 ) );
  maxBack->setValue( 10 );
  maxFront->setValue( 10 );
  minBack->setValue( 1 );
  minFront->setValue( 1 );
  priority->setValue( 0 );
  spatialize->setValue( true );

  direction->route( soundSetup );
  location->route( soundSetup );

  if( !al_device )
    al_device = alcOpenDevice(NULL);
  if( !al_device ) {
    cerr << "Warning: Could not find audio device for use with Sound node."
         << endl;
  }
  if( !al_context )
	  al_context = alcCreateContext( al_device, NULL);
  
	if( !al_context ) {
    cerr << "Warning: Could not create OpenAL context for use with Sound node."
         << endl;
	}
  alcMakeContextCurrent(al_context);
  if( !al_source ) {
    alGenSources( 1, &al_source );
    }
  alSourcef( al_source, AL_GAIN, 0 );
}


void Sound::traverseSG( TraverseInfo &ti ) {
  X3DSoundSourceNode *sound_source = source->getValue();
  if( sound_source ) {
    sound_source->traverseSG( ti );
  }
}

void Sound::ALSoundSetup::update() {
  Sound *sound_node = static_cast< Sound * >( getOwner() );

  if( !sound_node->al_context || !sound_node->al_device ) return;
  
  alDistanceModel( AL_LINEAR_DISTANCE_CLAMPED );
  const Vec3f &pos = sound_node->location->getValue(); 
  alSource3f( sound_node->al_source, AL_POSITION, pos.x, pos.y, pos.z );
  alSourcef( sound_node->al_source, AL_GAIN, sound_node->intensity->getValue() );
  alSourcef( sound_node->al_source, AL_REFERENCE_DISTANCE, 
             sound_node->minFront->getValue() );
  alSourcef( sound_node->al_source, AL_MAX_DISTANCE, 
             sound_node->maxFront->getValue() );
}

void Sound::SFSoundSourceNode::onAdd( Node *n ) {
  TypedSFNode< X3DSoundSourceNode >::onAdd( n );
  X3DSoundSourceNode *sound_source = static_cast< X3DSoundSourceNode * >( n );
  X3DSoundNode *sound_node = static_cast< X3DSoundNode * >( getOwner() );
  if( sound_source ) {
    sound_source->registerSoundNode( sound_node );
  }
}

void Sound::SFSoundSourceNode::onRemove( Node *n ) {
  X3DSoundSourceNode *sound_source = static_cast< X3DSoundSourceNode * >( n );
  X3DSoundNode *sound_node = static_cast< X3DSoundNode * >( getOwner() );
  if( sound_source ) {
    sound_source->unregisterSoundNode( sound_node );
  }
  TypedSFNode< X3DSoundSourceNode >::onRemove( n );
}
