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
/// \file OggFileReader.cpp
/// \brief CPP file for OggFileReader, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "OggFileReader.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase OggFileReader::database( 
                                        "OggFileReader", 
                                        &(newInstance< OggFileReader >),
                                          typeid( OggFileReader ),
                                          &H3DSoundFileNode::database );

H3DSoundFileNode::FileReaderRegistration OggFileReader::reader_registration(
                            "OggFileReader",
                            &(newSoundFileNode< OggFileReader >),
                            &OggFileReader::supportsFileType 
                            );


unsigned int OggFileReader::read( char *buffer, unsigned int size ) {
      
  int section;
  int bytes_read = 0;
  
  while( bytes_read < size ) {
    int res = ov_read(&ogg_file, buffer + bytes_read, 
                      size - bytes_read, 0, 2, 1, & section);
    if( res <= 0 ) break;
    
    else 
      bytes_read += res;
    //  cerr << oggString( res );
  }
  return bytes_read;
}

unsigned int OggFileReader::load( const string &_url ) {
  url = _url;
  //        ov_clear( &ogg_file );
  FILE *f = fopen( url.c_str(), "rb" );
  if( f ) {
    if( ov_open( f, &ogg_file, NULL, 0 ) < 0 ) {
      cerr << "Invalid ogg file" << endl;
      fclose( f );
    } else {
      vorbis_info = ov_info(&ogg_file, -1);
      vorbis_comment = ov_comment(&ogg_file, -1);
    }
  } else {
    cerr << "Could not find file" << endl;
  }
  return totalDataSize();
}

bool OggFileReader::supportsFileType( const string &url ) {
  FILE *f = fopen( url.c_str(), "rb" );
  OggVorbis_File ogg_file;
  if( f ) {
    if( ov_open( f, &ogg_file, NULL, 0 ) < 0 ) {
      fclose( f );
      return false;
    } else {
      ov_clear( &ogg_file );
      return true;
    }
  } else {
    return false;
  }
}
