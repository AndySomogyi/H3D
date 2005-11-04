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
/// \file AudioFileReader.cpp
/// \brief CPP file for AudioFileReader, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "AudioFileReader.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase AudioFileReader::database( 
                                        "AudioFileReader", 
                                        &(newInstance< AudioFileReader >),
                                          typeid( AudioFileReader ),
                                          &H3DSoundFileNode::database );

H3DSoundFileNode::FileReaderRegistration AudioFileReader::reader_registration(
                            "AudioFileReader",
                            &(newSoundFileNode< AudioFileReader >),
                            &AudioFileReader::supportsFileType 
                            );


unsigned int AudioFileReader::read( char *buffer, unsigned int size ) {
  int sample_format, sample_width;
  afGetVirtualSampleFormat( file, AF_DEFAULT_TRACK, 
                            &sample_format, &sample_width );
  unsigned int sample_size = nrChannels() * (sample_width /8 ); 
  
  //if( sample_format == AF_SAMPFMT_TWOSCOMP ) {
  //cerr << "Yoment" << endl;
  //}
  
  unsigned int nr_samples = size / sample_size;
  int samples_read = afReadFrames( file, AF_DEFAULT_TRACK, buffer, 
                                   nr_samples );
  return samples_read * sample_size;
}

unsigned int AudioFileReader::load( const string &_url ) {
   url = _url;
   file = afOpenFile( url.c_str(), "r", NULL );
   if( file ) {
     int fileformat = afGetFileFormat( file, NULL );
     if( fileformat == AF_FILE_UNKNOWN  ) {
       cerr << "unsupported file" << endl;
       afCloseFile( file );
     } else {
       cerr << afSetVirtualByteOrder( file, AF_DEFAULT_TRACK, 
                                      AF_BYTEORDER_LITTLEENDIAN );
       // afSetVirtualSampleFormat( file, AF_DEFAULT_TRACK, AF_SAMPFMT_UNSIGNED, 16 );
     }
   } else {
     cerr << "Could not find file" << endl;
   }
   return totalDataSize();
}

bool AudioFileReader::supportsFileType( const string &url ) {
  AFfilehandle file;
  file = afOpenFile( url.c_str(), "r", NULL );
  if( file ) {
    int fileformat = afGetFileFormat( file, NULL );
    afCloseFile( file );
    return fileformat != AF_FILE_UNKNOWN;
  } else {
    return false;
  }
}
