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
/// \file OggFileReader.h
/// \brief Header file for OggFileReader, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __OGGFILEREADER_H__
#define __OGGFILEREADER_H__

#include "H3DSoundFileNode.h"
#include <vorbis/vorbisfile.h>

namespace H3D {

  /// \ingroup Nodes
  /// \class OggFileReader
  ///
  /// 
  class OggFileReader : public H3DSoundFileNode {
  public:

    /// Load a sound file from the given url that will be used to
    /// generate PCM data.
    OggFileReader() {}

    unsigned int load( const string &_url );
      
    virtual H3DTime duration() {
      return ov_time_total( &ogg_file, -1 );
    }
    
    virtual void reset() {
      ov_raw_seek( &ogg_file, 0 );
    }
    
    virtual unsigned int totalDataSize() {
      return ov_raw_total( &ogg_file, -1 );
    }
      
    virtual unsigned int nrChannels() {
      return vorbis_info->channels;
    }
    
    /// Returns the number of bits for a sample.
    virtual unsigned int bitsPerSample() {
      return 16;
    }

    virtual unsigned int read( char *buffer, unsigned int size );
    
    virtual unsigned int samplesPerSecond() {
      return vorbis_info->rate;
    }
    
    static bool supportsFileType( const string &url );
    static H3DNodeDatabase database;
    static FileReaderRegistration reader_registration;

  protected:
    OggVorbis_File ogg_file;
    vorbis_info* vorbis_info;
    vorbis_comment *vorbis_comment;
    string url;
  };
}

#endif
