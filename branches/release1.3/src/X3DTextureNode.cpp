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
/// \file X3DTextureNode.cpp
/// \brief CPP file for X3DTextureNode, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "X3DTextureNode.h"
#include <assert.h>

using namespace H3D;

H3D_API_EXCEPTION( UnsupportedPixelComponentType );
H3D_VALUE_EXCEPTION( Image::PixelType, UnsupportedPixelType );

X3DTextureNode *X3DTextureNode::active_texture = NULL;

GLint X3DTextureNode::glInternalFormat( Image *i ) {
  switch( i->pixelType() ) {
  case Image::LUMINANCE: 
    switch( i->bitsPerPixel() ) {
    case 4:  return GL_LUMINANCE4;
    case 8:  return GL_LUMINANCE8;
    case 12: return GL_LUMINANCE12;
    case 16: return GL_LUMINANCE16;
    default: return GL_LUMINANCE;
    }
  case Image::LUMINANCE_ALPHA:
    switch( i->bitsPerPixel() ) {
    case 8:  return GL_LUMINANCE4_ALPHA4;
    case 16: return GL_LUMINANCE8_ALPHA8;
    case 32: return GL_LUMINANCE16_ALPHA16;
    default: return GL_LUMINANCE_ALPHA;
    }
  case Image::RGB:
  case Image::BGR:
    switch( i->bitsPerPixel() ) {
    case 12: return GL_RGB4;
    case 15: return GL_RGB5;
    case 24: return GL_RGB8;
    case 30: return GL_RGB10;
    case 36: return GL_RGB12;
    case 48: return GL_RGB16;
    default: return GL_RGB;
    }
  case Image::RGBA:
  case Image::BGRA:
    switch( i->bitsPerPixel() ) {
    case 16: return GL_RGBA4;
    case 32: return GL_RGBA8;
    case 48: return GL_RGBA12;
    case 64: return GL_RGBA16;
    default: return GL_RGBA;
    }
  default:
    throw UnsupportedPixelType( i->pixelType() );
  }
    
}

GLenum X3DTextureNode::glPixelFormat( Image *i ) {
  switch( i->pixelType() ) {
  case Image::LUMINANCE:       return GL_LUMINANCE;
  case Image::LUMINANCE_ALPHA: return GL_LUMINANCE_ALPHA;
  case Image::RGB:             return GL_RGB;
  case Image::BGR:             return GL_BGR;
  case Image::RGBA:            return GL_RGBA;
  case Image::BGRA:            return GL_BGRA;
  default: throw UnsupportedPixelType( i->pixelType() ); 
  }
}

GLenum X3DTextureNode::glPixelComponentType( Image *i ) {
  switch( i->pixelType() ) {
  case Image::LUMINANCE: 
    switch( i->pixelComponentType() ) {
    case Image::UNSIGNED: 
      switch( i->bitsPerPixel() ) {
      case 8:  return GL_UNSIGNED_BYTE;
      case 16: return GL_UNSIGNED_SHORT;
      case 32: return GL_UNSIGNED_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit UNSIGNED LUMINANCE value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION ); 
      }
    case Image::SIGNED:
      switch( i->bitsPerPixel() ) {
      case 8:  return GL_BYTE;
      case 16: return GL_SHORT;
      case 32: return GL_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit SIGNED LUMINANCE value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION ); 
      }
    case Image::RATIONAL:
      switch( i->bitsPerPixel() ) {
      case 32: return GL_FLOAT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit RATIONAL LUMINANCE value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION ); 
      }
    }
  case Image::LUMINANCE_ALPHA:
    switch( i->pixelComponentType() ) {
    case Image::UNSIGNED: 
      switch( i->bitsPerPixel() ) {
      case 16:  return GL_UNSIGNED_BYTE;
      case 32: return GL_UNSIGNED_SHORT;
      case 64: return GL_UNSIGNED_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit UNSIGNED LUMINANCE_ALPHA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    case Image::SIGNED:
      switch( i->bitsPerPixel() ) {
      case 16:  return GL_BYTE;
      case 32: return GL_SHORT;
      case 64: return GL_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit SIGNED LUMINANCE_ALPHA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    case Image::RATIONAL:
      switch( i->bitsPerPixel() ) {
      case 64: return GL_FLOAT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit RATIONAL LUMINANCE_ALPHA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );

      }
    }
  case Image::RGB:
  case Image::BGR:
    switch( i->pixelComponentType() ) {
    case Image::UNSIGNED: 
      switch( i->bitsPerPixel() ) {
      case 24:  return GL_UNSIGNED_BYTE;
      case 48: return GL_UNSIGNED_SHORT;
      case 96: return GL_UNSIGNED_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit UNSIGNED RGB value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    case Image::SIGNED:
      switch( i->bitsPerPixel() ) {
      case 24:  return GL_BYTE;
      case 48: return GL_SHORT;
      case 96: return GL_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit SIGNED RGB value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );

      }
    case Image::RATIONAL:
      switch( i->bitsPerPixel() ) {
      case 96: return GL_FLOAT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit RATIONAL RGB value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );

      }
    }       

  case Image::RGBA:
  case Image::BGRA:
    switch( i->pixelComponentType() ) {
    case Image::UNSIGNED: 
      switch( i->bitsPerPixel() ) {
      case 32:  return GL_UNSIGNED_BYTE;
      case 64:  return GL_UNSIGNED_SHORT;
      case 128: return GL_UNSIGNED_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit UNSIGNED RGBA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    case Image::SIGNED:
      switch( i->bitsPerPixel() ) {
      case 32:  return GL_BYTE;
      case 64: return GL_SHORT;
      case 128: return GL_INT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit SIGNED RGBA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    case Image::RATIONAL:
      switch( i->bitsPerPixel() ) {
      case 128: return GL_FLOAT;
      default: 
        stringstream s;
        s << "Cannot find OpenGL type for "
          << i->bitsPerPixel() 
          << " bit RATIONAL RGBA value."
          << ends;
        throw UnsupportedPixelComponentType( s.str(), H3D_FULL_LOCATION );
      }
    }
  default:
    throw UnsupportedPixelType( i->pixelType() );
  }

}

X3DTextureNode::X3DTextureNode( 
                               Inst< DisplayList > _displayList,
                               Inst< SFNode>  _metadata ):
  X3DAppearanceChildNode( _displayList, _metadata ) {

  type_name = "X3DTextureNode";
}

GLuint X3DTextureNode::renderImage( Image *image, 
                                    GLenum texture_target, 
                                    bool scale_to_power_of_two ) {
  if( image ) {
    GLuint texture_id;
    glGenTextures( 1, &texture_id );
    glBindTexture( texture_target, texture_id );

    while( glGetError() != GL_NO_ERROR )
      ;
    glTexImage( image, texture_target, scale_to_power_of_two );

    GLenum error = glGetError();
    if( error != GL_NO_ERROR ) {

      stringstream msg;

      // check that the dimensions of the image is a power of 2.
      bool invalid_dimensions = false;
      if( !isPowerOfTwo( image->width() ) ) {
        msg <<" Width " <<image->width() 
            <<" (expecting power of 2) ";
        invalid_dimensions = true;
      }
      if( !isPowerOfTwo( image->height() ) ) {
        msg <<" Height " <<image->height() 
            <<" (expecting power of 2)";
        invalid_dimensions = true;
      }
      if( !isPowerOfTwo( image->depth() ) ) {
        msg <<" Depth " <<image->depth() 
            <<" (expecting power of 2))";
        invalid_dimensions = true;
      }
      if( invalid_dimensions ) {
        msg << " in " << name << ends;
        throw InvalidTextureDimensions( msg.str(),
                                        H3D_FULL_LOCATION );
      }
      msg << (char*) gluErrorString( error ) << " in " << name << ends;
      throw OpenGLTextureError( (char*) gluErrorString( error ),
                                H3D_FULL_LOCATION );
    } 

    return texture_id;
  } else { 
    return 0;
    cerr << "No image" << endl;
  }

}

// The glTexImage2D function needs each line of image data to be 4 byte 
// aligned. This function takes a pointer to image data and width,height
// and bits per pixel for that data and returns a pointer to new image
// data where the data has been padded with dummy values in order to
// align it to the closest 4 byte position.
void *X3DTextureNode::padTo4ByteAlignment( const void *data,
                                           unsigned int width,
                                           unsigned int height,
                                           unsigned int depth,
                                           unsigned int bits_per_pixel ) {
  unsigned int bits_per_line = width * bits_per_pixel;
  assert( bits_per_line % 8 == 0 );
  unsigned int bytes_per_line = bits_per_line / 8;
  unsigned int bytes_to_pad = 4 - bytes_per_line % 4;

  unsigned int new_bytes_per_line = bytes_per_line + bytes_to_pad;

  void *new_data = malloc( depth * height * new_bytes_per_line );
 
  for( unsigned int d = 0; d < depth; d++ )
    for( unsigned int h = 0; h < height; h++ ) {
      memcpy( (unsigned char*)new_data + d*height*new_bytes_per_line + h*new_bytes_per_line,
              (const unsigned char*)data + d*height*bytes_per_line + h * bytes_per_line,
              bytes_per_line );
    }
  return new_data;
}







