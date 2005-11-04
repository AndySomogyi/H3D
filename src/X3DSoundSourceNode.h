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
#include "X3DSoundNode.h"
#include "PeriodicUpdate.h"
#include <list>
#include <fstream>
#include "al.h"
#include "alc.h"
#include "alut.h"
#include "H3DSoundStreamNode.h"
#include <vorbis/vorbisfile.h>
#include <audiofile.h>

#define NR_STREAM_BUFFERS 3
#define STREAM_BUFFER_SIZE (4096*8)

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class X3DSoundSourceNode
  /// \brief This abstract node type is used to derive node types that can emit
  /// audio data. 
  ///
  /// 
  class X3DSoundSourceNode : public X3DTimeDependentNode {
  public:
    
    class TimeHandler: public X3DTimeDependentNode::TimeHandler {
    public:
      TimeHandler() : was_playing( false ) {}
    protected:
      virtual void update();

      bool was_playing;
    };

    class ALSoundBuffer: 
      public PeriodicUpdate< Field > {
    protected:
      virtual void update() {
        X3DSoundSourceNode *s = 
          static_cast< X3DSoundSourceNode * >( getOwner());
        s->ALrender();
        event_fields.clear();
      }

      /// When the event is propagated the display list is invalidated.
      virtual void propagateEvent( Event e ) {
        Field::propagateEvent( e );
        event_fields.insert( e.ptr );
      }
    public:

      /// Returns true if the Field given has generated an event to this
      /// field since the last call to the update() function.
      inline bool hasCausedEvent( Field *f ) {
        return event_fields.find( f ) != event_fields.end();
      }

      /// Returns true if the Field given has generated an event to this
      /// field since the last call to the update() function.
      template< class FieldType >
      inline bool hasCausedEvent( auto_ptr< FieldType > &f ) {
        return hasCausedEvent( f.get() );
      }

    protected:
      /// The fields that has generated an event since the last call to
      /// update()
      set< Field * > event_fields;
    };


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
                        Inst< SFBool   > _isPaused         = 0,
                        Inst< TimeHandler > _timeHandler   = 0 );

    virtual void initALBuffers( bool stream );

    virtual void ALrender();

    /// Returns the default xml containerField attribute value.
    /// For this node it is "children".
    virtual string defaultXMLContainerField() {
      return "source";
    }

    virtual void traverseSG( TraverseInfo &ti ) {
      X3DTimeDependentNode::traverseSG( ti );
      traversing = true;
    }

    virtual void registerSoundNode( X3DSoundNode *n ) {
      sound_buffer->upToDate();
      if( !sound_as_stream ) {
        alSourcei( n->getALSourceId(), AL_BUFFER, al_buffers[0] );
        if( loop->getValue() )
          alSourcei( n->getALSourceId(), AL_LOOPING, AL_TRUE );
        else
          alSourcei( n->getALSourceId(), AL_LOOPING, AL_FALSE );
      } else {
        alSourceQueueBuffers( n->getALSourceId(), NR_STREAM_BUFFERS, 
                             al_buffers );  
      }
      alSourcef( n->getALSourceId(), AL_PITCH, 
                 pitch->getValue() );
      if( isActive->getValue() && !isPaused->getValue() )
        alSourcePlay( n->getALSourceId() );
      parent_sound_nodes.push_front( n );
    }

    virtual void unregisterSoundNode( X3DSoundNode *n ) {
      parent_sound_nodes.remove( n );
    }

    virtual void onPause();
    virtual void onResume();
    virtual void onStart();
    virtual void onStop();

    auto_ptr< SFString > description;
    auto_ptr< SFFloat  > pitch;
    auto_ptr< SFTime   > duration_changed;

    auto_ptr< ALSoundBuffer > sound_buffer;

    static H3DNodeDatabase database;
  protected:
    list< X3DSoundNode * > parent_sound_nodes;
    ALuint al_buffers[NR_STREAM_BUFFERS];
    ALenum al_format;
    bool traversing;
    AutoRef< H3DSoundStreamNode > reader;
    bool sound_as_stream;
  };
}

#endif
