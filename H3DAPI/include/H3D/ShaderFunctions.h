//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2014, SenseGraphics AB
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
/// \file ShaderFunctions.h
/// \brief Header file for help functions used by shader nodes.
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SHADERFUNCTIONS_H__
#define __SHADERFUNCTIONS_H__

#include <H3D/H3DDynamicFieldsObject.h>
#include <GL/glew.h>
#include <H3D/H3DSingleTextureNode.h>

#ifdef HAVE_CG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif

namespace H3D {


  /// template class to provide value change checking after update for SField
  template< class SF >
  class SFUniform : public SF {
  public:
    bool actualChanged;

    SFUniform(){
        actualChanged = true;
        SF();
            }
    virtual void setValue( const typename SF::value_type &v, int id = 0 ){
      typename SF::value_type old_value = this->value;
      SF::setValue( v, id );
      if( this->value == old_value ) {
        actualChanged = false;
      }else {
        actualChanged = true;
      }
    }
    virtual string getTypeName() { return "SFUniform"; }
  protected:
    virtual void update(){
      typename SF::value_type old_value = this->value;
      SF::update();
      if( this->value == old_value ) {
        //Console(LogLevel::Error)<<"monitored value actually changed"<<endl;
        actualChanged = false;
      }else {
        actualChanged = true;
      }
    }
  };

  namespace Shaders {
#ifdef HAVE_CG
    CGprofile H3DAPI_API cgProfileFromString( const string &profile, 
                                              const string &type );

    /// Set the value of a uniform variable in the given CG shader.
    /// The name of the uniform variable is the same as the name of the field. 
    bool H3DAPI_API setCGUniformVariableValue( CGprogram program_handle,
                                               Field *field );
#endif
    // struct contains the uniform value changed tag and GLSL uniform location
    struct UniformInfo 
    {
      Field* field;   // associated field for the uniform
      // uniform field location in shader program, need update after re-link
      GLint location; 
    };
    /// Set the value of a uniform variable in the given GLSL shader.
    /// The name of the uniform variable is the same as the name of the field. 
    ///
    /// \param force If true, then the uniform value is always set even if the field
    ///              value has not changed.
    ///
    bool H3DAPI_API setGLSLUniformVariableValue( GLhandleARB program_handle,
                                                 Field *field, UniformInfo* ui = NULL, bool force= false );

    /// \deprecated will be removed, use renderTextures( list<H3DSingleTextureNode*>*, H3DInt32* )
    void H3DAPI_API renderTextures( H3DDynamicFieldsObject *, H3DInt32* max_texture = NULL, H3DInt32* max_image = NULL );
    void H3DAPI_API renderTextures( list<H3DSingleTextureNode*>*, H3DInt32* );
    void H3DAPI_API renderShaderResources( H3DDynamicFieldsObject * );
    /// \deprecated will be removed, use postRenderTextures( list<H3DSingleTextureNode*>* , H3DInt32*) instead
    void H3DAPI_API postRenderTextures( H3DDynamicFieldsObject *, H3DInt32* max_texture = NULL );
    void H3DAPI_API postRenderTextures( list<H3DSingleTextureNode*>* , H3DInt32*);
    /// \deprecated will be removed, use preRenderTextures( list<H3DSingleTextureNode*>*, H3DInt32* ) instead
    void H3DAPI_API preRenderTextures( H3DDynamicFieldsObject *, H3DInt32* max_texture = NULL );
    void H3DAPI_API preRenderTextures( list<H3DSingleTextureNode*>*, H3DInt32* );
    void H3DAPI_API preRenderShaderResources( H3DDynamicFieldsObject *, const GLhandleARB program );
    void H3DAPI_API postRenderShaderResources( H3DDynamicFieldsObject*, const GLhandleARB program );
    GLbitfield H3DAPI_API getAffectedGLAttribs( H3DDynamicFieldsObject * );

  }
}

#endif
