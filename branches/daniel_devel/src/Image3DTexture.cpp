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
/// \file Image3DTexture.cpp
/// \brief CPP file for Image3DTexture, X3D scene-graph node
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include "Image3DTexture.h"

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase Image3DTexture::database( "Image3DTexture", 
                                        &(newInstance<Image3DTexture>), 
                                        typeid( Image3DTexture ),
                                        &X3DTexture3DNode::database );

namespace Image3DTextureInternals {
  FIELDDB_ELEMENT( Image3DTexture, url, INPUT_OUTPUT );
  FIELDDB_ELEMENT( Image3DTexture, imageLoader, INPUT_OUTPUT );
}


Image3DTexture::Image3DTexture( 
                           Inst< DisplayList > _displayList,
                           Inst< SFNode        > _metadata,
                           Inst< MFString      > _url,
                           Inst< SFBool        > _repeatS,
                           Inst< SFBool        > _repeatT,
                           Inst< SFBool        > _repeatR,
                           Inst< SFBool        > _scaleToP2,
                           Inst< SFImage       > _image,
                           Inst< MFImageLoader > _imageLoader,
                           Inst< SFTextureProperties > _textureProperties  ) :
  X3DTexture3DNode( _displayList, _metadata, _repeatS, _repeatT, _repeatR,
                    _scaleToP2, _image, _textureProperties ),
  X3DUrlObject( _url ),
  imageLoader( _imageLoader ) {

  type_name = "Image3DTexture";
  database.initFields( this );

  url->route( image );
  imageLoader->route( image );
}

Image* Image3DTexture::SFImage::loadImage( Image3DTexture *texture,
                                           const vector< string > &urls,
                                           const NodeVector &image_loaders ) {

  if( image_loaders.size() ) { 
    for( vector<string>::const_iterator i = urls.begin(); 
         i != urls.end(); ++i ) {
      for( NodeVector::const_iterator il = image_loaders.begin();
           il != image_loaders.end();
           il++ ) {
        string url = texture->resolveURLAsFile( *i );
        Image *image = 
          static_cast< H3DImageLoaderNode * >(*il)->loadImage( url );
        if( image ) {
          texture->setURLUsed( *i );
          return image;
        }
      }
    }
  }
  
  for( vector<string>::const_iterator i = urls.begin(); 
       i != urls.end(); ++i ) {
    string url = texture->resolveURLAsFile( *i );
	  H3DImageLoaderNode *il = H3DImageLoaderNode::getSupportedFileReader( url );
    if( il ) {
      texture->setURLUsed( *i );
      return il->loadImage( url );
    }
  }

  Console(4) << "Warning: None of the urls in Image3DTexture with url [";
  for( vector<string>::const_iterator i = urls.begin(); 
       i != urls.end(); ++i ) {  
    Console(4) << " \"" << *i << "\"";
  }
  Console(4) << "] could be loaded. Either they don't exist or the file format "
             << "is not supported by any H3DImageLoaderNode that is available "
             << "(in " << getOwner()->getName() << ")" << endl;

  texture->setURLUsed( "" );
  return( NULL );
}

Scene::CallbackCode Image3DTexture::SFImage::loadImageCB( void *data ) {
  CBData *input = static_cast< CBData * >( data );
  input->texture->image->setValue( input->image );
  return Scene::CALLBACK_DONE;
}

void *Image3DTexture::SFImage::loadImageThreadFunc( void * data ) {
  ThreadFuncData *input = static_cast< ThreadFuncData * >( data );
  SFImage *sfimage = static_cast< SFImage * >( input->texture->image.get() );
  Image *image = sfimage->loadImage( input->texture,
                                     input->urls,
                                     input->image_loaders );
                                  
  sfimage->cb_data.texture = input->texture;
  sfimage->cb_data.image = image;
  Scene::addCallback( loadImageCB, &sfimage->cb_data );
  return NULL;
}

void Image3DTexture::SFImage::update() {
  Image3DTexture *texture = static_cast< Image3DTexture * >( getOwner() );
  MFImageLoader *image_loaders = static_cast< MFImageLoader * >( routes_in[1] );
  MFString *urls = static_cast< MFString * >( routes_in[0] );

  if( Image3DTexture::load_images_in_separate_thread ) {
    value = NULL;
    // delete the old thread
    texture->load_thread.reset( NULL );
    
    thread_data.texture = texture;
    thread_data.urls = urls->getValue();
    thread_data.image_loaders = image_loaders->getValue();

    texture->load_thread.reset( new H3DUtil::SimpleThread( &loadImageThreadFunc, 
                                                   (void *)&thread_data ) );
  } else {
    value = loadImage( texture, urls->getValue(), image_loaders->getValue() );
  }
}

void Image3DTexture::render() {
  if( url->size() > 0 ) {
    try {
      X3DTexture3DNode::render();
    } catch( InvalidTextureDimensions &e ) {
      stringstream s;
      s << e.message << " with url [" ;
      for( MFString::const_iterator i = url->begin(); i != url->end(); ++i ) { 
        s << " \"" << *i << "\"";
      }
      s << "].";
      e.message = s.str();
      throw e;
    } catch( OpenGLTextureError &e ) {
      stringstream s;
      s << e.message << " with url [" ;
    for( MFString::const_iterator i = url->begin(); i != url->end(); ++i ) {  
      s << " \"" << *i << "\"";
    }
    s << "].";
    e.message = s.str();
    throw e;
    }
  } else {
    disableTexturing();
  }
}


