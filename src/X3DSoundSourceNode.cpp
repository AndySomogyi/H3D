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
#include "OggFileReader.h"
#include "AudioFileReader.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase X3DSoundSourceNode::database( 
        "X3DSoundSourceNode", 
        NULL,
        typeid( X3DSoundSourceNode ),
        &X3DTimeDependentNode::database );

namespace X3DSoundSourceNodeInternals {
  FIELDDB_ELEMENT( X3DSoundSourceNode, description, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DSoundSourceNode, pitch, INPUT_OUTPUT );
  FIELDDB_ELEMENT( X3DSoundSourceNode, duration_changed, INPUT_OUTPUT );
}

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
                                        Inst<  SFBool  >  _isPaused,
                                        Inst< TimeHandler > _time_handler ) :
  X3DTimeDependentNode( _metadata, _loop, _pauseTime, _resumeTime, _startTime,
                        _stopTime, _elapsedTime, _isActive, _isPaused,
                        _time_handler ),
  description     ( _description      ),
  pitch           ( _pitch            ),
  duration_changed( _duration_changed ),
  sound_buffer( new ALSoundBuffer ),
  traversing( false ) {
  type_name = "X3DSoundSourceNode";
  database.initFields( this );
  al_buffers[0] = 0;
  sound_buffer->setName( "soundBuffer" );
  sound_buffer->setOwner( this );

  description->setValue( "" );
  pitch->setValue( 1.0 );
  duration_changed->setValue( 0 );

  pitch->route( sound_buffer );
  loop->route( sound_buffer );
}


void X3DSoundSourceNode::initALBuffers( bool stream ) {
  if( !stream ) {
    sound_as_stream = false;
    char *buffer = new char[ reader->totalDataSize() ];
    reader->read( buffer, reader->totalDataSize() );
    alBufferData( al_buffers[0], al_format, 
                  buffer, reader->totalDataSize(), 
                  reader->samplesPerSecond() );
    delete buffer;
    for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
         i != parent_sound_nodes.end(); i++ ) {
      if( loop->getValue() )
        alSourcei( (*i)->getALSourceId(), AL_LOOPING, AL_TRUE );
      else
        alSourcei( (*i)->getALSourceId(), AL_LOOPING, AL_FALSE );
      alSourcei( (*i)->getALSourceId(), AL_BUFFER, 0 );
      alSourcei( (*i)->getALSourceId(), AL_BUFFER, al_buffers[0] );
    }
  } else {
    sound_as_stream = true;
    char *buffer = new char[ STREAM_BUFFER_SIZE ];
    for( unsigned int i = 0; i < NR_STREAM_BUFFERS; i++ ) {
      unsigned int bytes_read = reader->read( buffer, STREAM_BUFFER_SIZE );
      alBufferData( al_buffers[i], al_format, 
                    buffer, bytes_read, 
                    reader->samplesPerSecond() );
    }
    delete buffer;
    
    for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
         i != parent_sound_nodes.end(); i++ ) {
      alSourcei( (*i)->getALSourceId(), AL_LOOPING, AL_FALSE );
      alSourcei( (*i)->getALSourceId(), AL_BUFFER, 0 );
      alSourceQueueBuffers( (*i)->getALSourceId(), NR_STREAM_BUFFERS, 
                            al_buffers );  
    }
  }
}

void X3DSoundSourceNode::onStart() {
  cerr << "Start" << endl;
  X3DTimeDependentNode::onStart();
  sound_buffer->upToDate();
  if( reader.get() ) reader->reset();
  initALBuffers( sound_as_stream );

  for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
       i != parent_sound_nodes.end(); i++ ) {
    alSourcei( (*i)->getALSourceId(), AL_BUFFER, 0 );
	  if( !sound_as_stream ) {
      alSourcei( (*i)->getALSourceId(), AL_BUFFER, al_buffers[0] );
    } else {
      alSourceQueueBuffers( (*i)->getALSourceId(), NR_STREAM_BUFFERS, 
                            al_buffers );  
    }
    alSourcePlay( (*i)->getALSourceId() );  
  }
}

void X3DSoundSourceNode::onStop() {
  cerr << "Stop" << endl;
  X3DTimeDependentNode::onStop();
  for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
       i != parent_sound_nodes.end(); i++ ) {
    alSourceStop( (*i)->getALSourceId() );  
    alSourcei( (*i)->getALSourceId(), AL_BUFFER, 0 );
  }

  if( reader.get() ) reader->reset();
}

void X3DSoundSourceNode::onPause() {
  cerr << "Pause" << endl;
  X3DTimeDependentNode::onPause();
  for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
       i != parent_sound_nodes.end(); i++ ) {
    alSourcePause( (*i)->getALSourceId() );  
  }
}

void X3DSoundSourceNode::onResume() {
  cerr << "Resume" << endl;
  X3DTimeDependentNode::onResume();
  for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
       i != parent_sound_nodes.end(); i++ ) {
    alSourcePlay( (*i)->getALSourceId() );  
  }
}


void X3DSoundSourceNode::TimeHandler::update() {
  H3DTime time = static_cast< SFTime * >( event.ptr )->getValue();
  X3DSoundSourceNode *sound_source = 
    static_cast< X3DSoundSourceNode * >( getOwner() );
  X3DTimeDependentNode::TimeHandler::update();

  if( sound_source->isActive->getValue() &&
      !sound_source->isPaused->getValue() ) { 
    if( !sound_source->sound_as_stream ) {
      ALint state;
      bool playing = false;
      for( list< X3DSoundNode * >::iterator i = sound_source->parent_sound_nodes.begin();
           i != sound_source->parent_sound_nodes.end(); i++ ) {
        alGetSourcei( (*i)->getALSourceId(), AL_SOURCE_STATE, &state );
        if( state == AL_PLAYING ) playing = true;
      }
      if( !playing ) {
        deactivate( time );
      }
    } else {
      ALint processed = 0;
      if( sound_source->parent_sound_nodes.size() > 0 ) {
        alGetSourcei( sound_source->parent_sound_nodes.front()->getALSourceId(), 
                      AL_BUFFERS_PROCESSED, &processed );
      }

      bool have_buffers = false;
      for( list< X3DSoundNode * >::iterator i = sound_source->parent_sound_nodes.begin();
           i != sound_source->parent_sound_nodes.end(); i++ ) {
        ALint processed_in_src;
        ALint in_queue;
        alGetSourcei( (*i)->getALSourceId(), AL_BUFFERS_PROCESSED, &processed_in_src );
        alGetSourcei( (*i)->getALSourceId(), AL_BUFFERS_QUEUED, &in_queue );
        if( processed_in_src < processed )
          processed = processed_in_src;
        
        if( in_queue > 0 ) have_buffers = true;
      }
      
      if( !have_buffers ) {
        deactivate( time );
      } else {
        if( sound_source->reader.get() && processed > 0 ) {
          // All sound souces have processed at least one buffer
          bool buffer_updated = false;
          ALuint buffer_id;
        
          // Unqueue from all sound sources
          for( list< X3DSoundNode * >::iterator i =
                 sound_source->parent_sound_nodes.begin();
               i != sound_source->parent_sound_nodes.end(); i++ ) {
            alSourceUnqueueBuffers( (*i)->getALSourceId(), 1, &buffer_id);
          }
        
          // Update buffer.
          char buffer[ STREAM_BUFFER_SIZE ];
          unsigned int bytes_read = sound_source->reader->read( buffer, STREAM_BUFFER_SIZE );
          if( bytes_read < STREAM_BUFFER_SIZE ) {
            alBufferData( buffer_id, sound_source->al_format, 
                          buffer, bytes_read , 
                          sound_source->reader->samplesPerSecond() );
            if( sound_source->loop->getValue() ) {
              sound_source->reader->reset();
            } 
          } else {
            alBufferData( buffer_id, sound_source->al_format, 
                          buffer, STREAM_BUFFER_SIZE, 
                          sound_source->reader->samplesPerSecond() );
          }
          
          ALint bsize;
          alGetBufferi( buffer_id, AL_SIZE, &bsize );

          if( bsize > 0 ) {
            // Queue the buffer on all sound sources.
            for( list< X3DSoundNode * >::iterator i =
                   sound_source->parent_sound_nodes.begin();
                 i != sound_source->parent_sound_nodes.end(); i++ ) {
              alSourceQueueBuffers((*i)->getALSourceId(), 1, 
                                   &buffer_id );  
              ALint state;
              alGetSourcei( (*i)->getALSourceId(), AL_SOURCE_STATE, &state );
              if( sound_source->isActive->getValue() &&
                  !sound_source->isPaused->getValue() &&
                  state != AL_PLAYING ) {
                alSourcePlay( (*i)->getALSourceId() );
              }
            }
          }
        }
      }
    }
  }
  sound_source->traversing = false;
}

void X3DSoundSourceNode::ALrender() {
  if( !al_buffers[0] ) {
    // Generate Buffers
    alGenBuffers(NR_STREAM_BUFFERS, al_buffers );
  }

  for( list< X3DSoundNode * >::iterator i = parent_sound_nodes.begin();
       i != parent_sound_nodes.end(); i++ ) {
    if( !sound_as_stream ) {
      if( loop->getValue() )
        alSourcei( (*i)->getALSourceId(), AL_LOOPING, AL_TRUE );
      else
        alSourcei( (*i)->getALSourceId(), AL_LOOPING, AL_FALSE );
    }
    alSourcef( (*i)->getALSourceId(), AL_PITCH, 
               pitch->getValue() );
  }
}
