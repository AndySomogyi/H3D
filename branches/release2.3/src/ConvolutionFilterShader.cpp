//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2014, SenseGraphics AB
//
//    Any use, or distribution, of this file without permission from the
//    copyright holders is strictly prohibited. Please contact SenseGraphics,
//    www.sensegraphics.com, for more information.
//
//
/// \file ConvolutionFilterShader.cpp
/// \brief CPP file for ConvolutionFilterShader.
///
//
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/ConvolutionFilterShader.h>
#include <H3D/GeneratedTexture.h>
#include <H3D/RenderTargetTexture.h>

using namespace H3D;

// Add this node to the H3DNodeDatabase system.
H3DNodeDatabase ConvolutionFilterShader::database( 
                                   "ConvolutionFilterShader", 
                                   &newInstance< ConvolutionFilterShader >,
                                   typeid( ConvolutionFilterShader ),
                                   &H3DGeneratedFragmentShaderNode::database );

namespace ConvolutionFilterShaderInternals {
  FIELDDB_ELEMENT( ConvolutionFilterShader, texture, INPUT_OUTPUT );
  FIELDDB_ELEMENT( ConvolutionFilterShader, type, INPUT_OUTPUT );
  FIELDDB_ELEMENT( ConvolutionFilterShader, weights, INPUT_OUTPUT );
  FIELDDB_ELEMENT( ConvolutionFilterShader, kernelSize, INPUT_OUTPUT );
}

ConvolutionFilterShader::ConvolutionFilterShader( Inst< DisplayList  > _displayList,
                            Inst< SFNode          > _metadata,
                            Inst< SFBool          > _isSelected ,
                            Inst< SFBool          > _isValid,
                            Inst< SFBool          > _activate,
                            Inst< SFString        > _language,
                            Inst< MFShaderPart    > _parts,
                            Inst< SFBool          > _suppressUniformWarnings,
                            Inst< MFString        > _fragmentShaderString,
                            Inst< MFString        > _vertexShaderString,
                            Inst< SFTexture2DNode > _texture,
                            Inst< SFString        > _type,
                            Inst< MFFloat         > _weights,
                            Inst< SFInt32         > _kernelSize ) :
  H3DGeneratedFragmentShaderNode( _displayList, _metadata, _isSelected, 
                                  _isValid, _activate, _language, _parts, 
                                  _suppressUniformWarnings, _fragmentShaderString, 
                                  _vertexShaderString ),
  texture( _texture ),
  type( _type ),
  weights( _weights ),
  kernelSize( _kernelSize ),
  textureWidth( new SFInt32 ),
  textureHeight( new SFInt32 ) {
  
  type_name = "ConvolutionFilterShader";
  database.initFields( this );


  type->addValidValue( "HORIZONTAL" );
  type->addValidValue( "VERTICAL" );
  type->addValidValue( "FULL" );
  type->setValue( "FULL" );

  kernelSize->setValue( 1 );
  weights->push_back( 1 );
  textureWidth->setValue( 0 );
  textureHeight->setValue( 0 );

  kernelSize->route( rebuildShader );
  type->route( rebuildShader );
}

void ConvolutionFilterShader::traverseSG( TraverseInfo &ti ) {
  H3DGeneratedFragmentShaderNode::traverseSG( ti );
  //TODO: better to have a general way to handle the texture width and height
  X3DTexture2DNode *t = dynamic_cast<X3DTexture2DNode*> (texture->getValue());
  RenderTargetTexture *rtt = dynamic_cast< RenderTargetTexture* >( texture->getValue() );
  Image *image = NULL;
  if( t ){
	image = t->image->getValue();

  }

  if( image ) {
	t->repeatS->setValue(false);
	t->repeatT->setValue(false);
    if( textureWidth->getValue() != image->width() ) {
      textureWidth->setValue( image->width() );
    }
    
    if( textureHeight->getValue() != image->height() ) {
      textureHeight->setValue( image->height() );
    }
  } else {
    GeneratedTexture *gen_tex = dynamic_cast< GeneratedTexture * >( t );
    if( gen_tex &&
        gen_tex->textureIdIsInitialized()) {
      GLuint tex_id = gen_tex->getTextureId();
      glPushAttrib( GL_TEXTURE_BIT );
      glBindTexture( gen_tex->getTextureTarget(), tex_id );
      GLint h,w;
      glGetTexLevelParameteriv(gen_tex->getTextureTarget(), 0, GL_TEXTURE_WIDTH, &w);
      glGetTexLevelParameteriv(gen_tex->getTextureTarget(), 0, GL_TEXTURE_HEIGHT, &h);
      glPopAttrib();
      textureWidth->setValue( w );
      textureHeight->setValue( h );
    } else {
      textureWidth->setValue( 0 );
      textureHeight->setValue( 0 );
    }
  }
  if( rtt ) {// if texutre is a renderTargetTexture
    GLuint tex_id = rtt->getTextureId();
    glPushAttrib( GL_TEXTURE_BIT );
    glBindTexture( rtt->getTextureTarget(), tex_id );
    GLint h,w;
    glGetTexLevelParameteriv(rtt->getTextureTarget(), 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(rtt->getTextureTarget(), 0, GL_TEXTURE_HEIGHT, &h);
    glPopAttrib();
    textureWidth->setValue( w );
    textureHeight->setValue( h );
  }
}

bool ConvolutionFilterShader::canBuildShader() {
  H3DInt32 kernel_size = kernelSize->getValue();
  // have texture and kernel size is an odd number larger than 0.
  return kernel_size > 0 && kernel_size % 2 == 1;
}

void ConvolutionFilterShader::buildShader(){
  if( canBuildShader() ) {
    H3DGeneratedShaderNode::buildShader();
  }
}

string ConvolutionFilterShader::addUniformFields( ComposedShader *shader ) {
  // add dynamic fields for uniform variables 
  stringstream s;
  
    s << addUniformToFragmentShader( shader,
                                     uniqueShaderName( "texture" ), 
                                     "sampler2D",
                                     H3D::Field::INPUT_OUTPUT,
                                     copyAndRouteField( texture ) );

    s << addUniformToFragmentShader( shader,
                                     uniqueShaderName( "textureWidth" ), 
                                     "int",
                                     H3D::Field::INPUT_OUTPUT,
                                     copyAndRouteField( textureWidth ) );

    s << addUniformToFragmentShader( shader,
                                     uniqueShaderName( "textureHeight" ), 
                                     "int",
                                     H3D::Field::INPUT_OUTPUT,
                                     copyAndRouteField( textureHeight ) );

    
    int nr_weights;
    if( type->getValue() == "FULL" ) {
      nr_weights = kernelSize->getValue()  * kernelSize->getValue();
    } else {
      nr_weights = kernelSize->getValue();
    }

    s << addUniformToFragmentShader( shader,
                                     uniqueShaderName( "weights" ), 
                                     "float",
                                     H3D::Field::INPUT_OUTPUT,
                                     copyAndRouteField( weights ),
				     nr_weights );

  return s.str();
}

string ConvolutionFilterShader::getFragmentShaderString() {
  if( canBuildShader() ) {
    stringstream s;
    s << "  const int KERNEL_SIZE = " << kernelSize->getValue() << "; " << endl;
    s << "  // the step in texture coordinates between each pixel " << endl;
    s << "  vec2 pixel_step = vec2( 1.0 / float( " << uniqueShaderName( "textureWidth" ) << " ), 1.0 / float( " << uniqueShaderName( "textureHeight" ) <<" ) ); " << endl;
    const string &t = type->getValue(); 

    if( t == "VERTICAL" ) {
      s << "  int min_index_h = 0; " << endl;
      s << "  int max_index_h = 0; " << endl;
      s << "  int min_index_v = -(KERNEL_SIZE - 1)/2; " << endl;
      s << "  int max_index_v = -min_index_v; " << endl;
    } else if( t == "HORIZONTAL" ) {
      s << "  int min_index_h = -(KERNEL_SIZE - 1)/2; " << endl;
      s << "  int max_index_h = -min_index_h; " << endl;
      s << "  int min_index_v = 0; " << endl;
      s << "  int max_index_v = 0; " << endl;
    } else {
      if( t != "FULL" ) {
	// print error message
      }
      s << "  int min_index_h = -(KERNEL_SIZE - 1)/2; " << endl;
      s << "  int max_index_h = -min_index_h; " << endl;
      s << "  int min_index_v = min_index_h; " << endl;
      s << "  int max_index_v = max_index_h; " << endl;
    } 

    s << "  int index = 0; " << endl;
    s << "  vec4 color = vec4( 0.0, 0.0, 0.0, 0.0 ); " << endl;
    s << "  for( int v = min_index_v; v <= max_index_v; v++ ) { " << endl;
    s << "    for( int h = min_index_h; h <= max_index_h; h++ ) { " << endl;
    s << "       vec2 offset = vec2( float(h) * pixel_step.x, " << endl;
    s << "                           float(v) * pixel_step.y ); " << endl; 
    s << "       vec2 tc = gl_TexCoord[0].st + offset; " << endl;
	s << "       tc.x = clamp(tc.x,0.0,1.0); " << endl;
	s << "       tc.y = clamp(tc.y,0.0,1.0); " << endl;
    s << "       color = color + texture2D( " << uniqueShaderName("texture") << ", tc ) * " << uniqueShaderName( "weights" ) << "[index]; " << endl;
    s << "       index++; " << endl;
    s << "    } " << endl;
    s << "  } " << endl;
    s << "  generated_color = color; " << endl;//texture2D( " << uniqueShaderName("texture") << ", tc ); " << endl;
    return s.str();
  } else {
    return "";
  }
}


