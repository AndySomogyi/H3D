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
/// \file AudioFileReader.h
/// \brief Header file for AudioFileReader
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __AUDIOFILEREADER_H__
#define __AUDIOFILEREADER_H__

#include "H3DSoundFileNode.h"
#include <audiofile.h>

namespace H3D {

  /// \ingroup Nodes
  /// \class AudioFileReader
  ///
  /// 
  class AudioFileReader : public H3DSoundFileNode {
  public:

    /// Load a sound file from the given url that will be used to
    /// generate PCM data.
    AudioFileReader() {}

    unsigned int load( const string &_url );
    
    virtual void reset() {
      afSeekFrame( file, AF_DEFAULT_TRACK, 0 );
    }
      
    virtual unsigned int totalDataSize() {
      return afGetTrackBytes( file, AF_DEFAULT_TRACK );
    }
    
    virtual unsigned int nrChannels() {
      return afGetChannels( file, AF_DEFAULT_TRACK );
    }
    
    virtual unsigned int samplesPerSecond() {
      return afGetRate( file, AF_DEFAULT_TRACK );
    }
    
    /// Returns the number of bits for a sample.
    virtual unsigned int bitsPerSample() {
      int sample_format, sample_width;
      afGetVirtualSampleFormat( file, AF_DEFAULT_TRACK, 
                                &sample_format, &sample_width );
      return sample_width;
    }

    virtual H3DTime duration() {
      return afGetFrameCount(file, AF_DEFAULT_TRACK) /
        afGetRate(file, AF_DEFAULT_TRACK);
    }

    virtual unsigned int read( char *buffer, unsigned int size );

    static bool supportsFileType( const string &url );
    
    static H3DNodeDatabase database;
    static FileReaderRegistration reader_registration;

  protected:
    AFfilehandle file;
    string url;
  };
}

#endif
