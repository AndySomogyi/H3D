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
/// \file H3DVideoClipDecoderNode.h
/// \brief Header file for H3DVideoClipDecoderNode, the abstract base
/// class for all nodes the decodes video clips.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __H3DVIDEOCLIPDECODERNODE_H__
#define __H3DVIDEOCLIPDECODERNODE_H__

#include "Node.h"
#include "Image.h"
#include <list>

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class H3DVideoClipDecoderNode
  /// \brief H3DVideoClipDecoderNode is a virtual base class for classes
  /// decoding video clips making new frames available to render.
  class H3DAPI_API H3DVideoClipDecoderNode : public Node {
  protected:
    typedef enum {
      PLAYING = 0,
      STOPPED = 1,
      PAUSED = 2 
    } PlayStatus;

  public:
    typedef H3DVideoClipDecoderNode*( *CreateNodeFunc)(); 

    /// Function ptr type for  
    typedef bool ( *SupportsFileFunc)( const string &url ); 
    
    template< class N >
    static H3DVideoClipDecoderNode *newImageLoaderNode() { return new N; };

    /// Class used to register a class to the registered file readers.
    struct H3DAPI_API DecoderRegistration{
    public:
      /// Constructor.
      DecoderRegistration( const string &_name,
                           CreateNodeFunc _create, 
                           SupportsFileFunc _supports ):
      name( _name ),
      create_func( _create ),
      supports_func( _supports ) {
		  
        if( !H3DVideoClipDecoderNode::initialized ) {
          H3DVideoClipDecoderNode::registered_decoders = 
            new list< DecoderRegistration >;
          initialized = true;
        }
        H3DVideoClipDecoderNode::registerDecoder( *this );
      }

      string name;
      CreateNodeFunc create_func;
      SupportsFileFunc supports_func;
    };

    /// Constructor.
    H3DVideoClipDecoderNode():
      status( STOPPED ) {
      type_name = "H3DVideoClipDecoderNode";
    }

    /// Pure virtual function to load an video clip from a url.
    /// Returns true on success.
    virtual bool loadClip( const string &url ) = 0;

    /// Returns true when a new frame is available.
    virtual bool haveNewFrame() = 0;

    /// Get the new frame. The buffer must be at least getFrameSize() bytes. 
    virtual void getNewFrame( unsigned char *buffer ) = 0;

    /// The size in bytes of the current frame.
    virtual unsigned int getFrameSize() = 0;

    /// The width in pixels of the current frame.
    virtual unsigned int getFrameWidth() = 0;

    /// The height in pixels of the current frame.
    virtual unsigned int getFrameHeight() = 0;

    /// The pixel type of the current frame.
    virtual Image::PixelType getFramePixelType() = 0;

    /// The pixel component type of the current frame.
    virtual Image::PixelComponentType getFramePixelComponentType() = 0;

    /// Get the number of bits per pixel in the current frame.
    virtual unsigned int getFrameBitsPerPixel() = 0;

    /// Start decoding the video clip.
    virtual void startPlaying() = 0;

    /// Stop decoding the video clip and set the position to the start
    /// position.
    virtual void stopPlaying() = 0;

    /// Pause the decoding of the video clip.
    virtual void pausePlaying() = 0;

    /// Set whether the clip should loop and start from the start again
    /// when the end has been reached.
    virtual void setLooping( bool on ) = 0;

    /// Get the current position in the clip (in seconds from start position)
    virtual H3DTime getPosition() = 0;
    
    /// Set the current position in the clip(in seconds from start position)
    virtual void setPosition( H3DTime pos ) = 0;

    /// Set the playback rate. A rate of 1 means normal playback speed, 2 double.
    /// Negative values means playing backwards. 
    /// \returns true if the new rate is supported by the decoder.
    virtual bool setRate( double r ) = 0;

    /// Returns the duration in seconds at normal play rate of the currently
    /// loaded video clip
    virtual H3DTime getDuration() = 0;

    /// Returns the current play status.
    inline PlayStatus getPlayStatus() { return status; }

    /// Returns the default xml containerField attribute value.
    /// For this node it is "imageLoader".
    ///
    virtual string defaultXMLContainerField() {
      return "decoder";
    }
    
    /// Given an url to a file, it returns an instance of a 
    /// H3DVideoClipDecoderNode
    /// class that can handle that file type. If no such class is registered
    /// NULL is returns.
    static H3DVideoClipDecoderNode *getSupportedDecoder( const string &url );

    /// Register a decoder that can then be returned by 
    /// getSupportedDecoder().
    /// \param name The name of the class
    /// \param create A function for creating an instance of that class.
    /// \param supports A function to determine if the class supports a
    /// given file type.
    static void registerDecoder( const string &name,
                                    CreateNodeFunc create, 
                                    SupportsFileFunc supports ) {
      registerDecoder( DecoderRegistration( name, create, supports ) );
    }

    /// Register a file reader that can then be returned by 
    /// getSupportedDecoder().
    static void registerDecoder( const DecoderRegistration &fr ) {
      registered_decoders->push_back( fr );
    }

  protected:
    static list< DecoderRegistration > *registered_decoders;
    static bool initialized;
    PlayStatus status;
  };
}

#endif
