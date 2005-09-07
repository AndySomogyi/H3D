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
/// \file X3DProgrammableShaderObject.cpp
/// \brief CPP file for X3DProgrammableShaderObject.
///
//
//
//////////////////////////////////////////////////////////////////////////////
#include "Node.h"
#include "X3DProgrammableShaderObject.h"
#include "X3DTexture2DNode.h"
#include "X3DTexture3DNode.h"

using namespace H3D;

namespace X3DProgrammableShaderObjectInternals {
  template< class Type >
  GLint *toIntArray( const vector< Type > &values ) {
    unsigned int size = values.size();
    GLint *v = new GLint[ size ];
    for( unsigned int i = 0; i < size; i++ ) 
      v[i] = (GLint) values[i];
    return v;
  }

  template< class Type >
  GLfloat *toFloatArray( const vector< Type > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size ];
    for( unsigned int i = 0; i < size; i++ ) 
      v[i] = (GLfloat) values[i];
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec2f > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 2 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec2f &a = values[i];
      v[i*2] = a.x;
      v[i*2+1] = a.y;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec3f > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 3 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec3f &a = values[i];
      v[i*3] = a.x;
      v[i*3+1] = a.y;
      v[i*3+2] = a.z;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec4f > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 4 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec4f &a = values[i];
      v[i*4] = a.x;
      v[i*4+1] = a.y;
      v[i*4+2] = a.z;
      v[i*4+3] = a.w;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec2d > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 2 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec2d &a = values[i];
      v[i*2]   = (H3DFloat) a.x;
      v[i*2+1] = (H3DFloat) a.y;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec3d > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 3 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec3d &a = values[i];
      v[i*3] =   (H3DFloat)a.x;
      v[i*3+1] = (H3DFloat)a.y;
      v[i*3+2] = (H3DFloat)a.z;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Vec4d > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 4 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Vec4d &a = values[i];
      v[i*4]   = (H3DFloat) a.x;
      v[i*4+1] = (H3DFloat) a.y;
      v[i*4+2] = (H3DFloat) a.z;
      v[i*4+3] = (H3DFloat) a.w;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Rotation > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 4 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Rotation &r = values[i];
      v[i*4]   = (H3DFloat) r.axis.x;
      v[i*4+1] = (H3DFloat) r.axis.y;
      v[i*4+2] = (H3DFloat) r.axis.z;
      v[i*4+3] = (H3DFloat) r.angle;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< RGB > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 3 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const RGB &r = values[i];
      v[i*3]   = (H3DFloat) r.r;
      v[i*3+1] = (H3DFloat) r.g;
      v[i*3+2] = (H3DFloat) r.b;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< RGBA > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 4 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const RGBA &r = values[i];
      v[i*4]   = (H3DFloat) r.r;
      v[i*4+1] = (H3DFloat) r.g;
      v[i*4+2] = (H3DFloat) r.b;
      v[i*4+3] = (H3DFloat) r.a;
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Matrix3f > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 9 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Matrix3f &m = values[i];
      v[i*9]   = (H3DFloat) m[0][0];
      v[i*9+1] = (H3DFloat) m[1][0];
      v[i*9+2] = (H3DFloat) m[2][0];
      v[i*9+3] = (H3DFloat) m[0][1];
      v[i*9+4] = (H3DFloat) m[1][1];
      v[i*9+5] = (H3DFloat) m[2][1];
      v[i*9+6] = (H3DFloat) m[0][2];
      v[i*9+7] = (H3DFloat) m[1][2];
      v[i*9+8] = (H3DFloat) m[2][2];
    }
    return v;
  }

  template<>
  GLfloat *toFloatArray( const vector< Matrix4f > &values ) {
    unsigned int size = values.size();
    GLfloat *v = new GLfloat[ size * 9 ];
    for( unsigned int i = 0; i < size; i++ ) {
      const Matrix4f &m = values[i];
      v[i*9]   = (H3DFloat) m[0][0];
      v[i*9+1] = (H3DFloat) m[1][0];
      v[i*9+2] = (H3DFloat) m[2][0];
      v[i*9+3] = (H3DFloat) m[3][0];
      v[i*9+4] = (H3DFloat) m[0][1];
      v[i*9+5] = (H3DFloat) m[1][1];
      v[i*9+6] = (H3DFloat) m[2][1];
      v[i*9+7] = (H3DFloat) m[3][1];
      v[i*9+8] = (H3DFloat) m[0][2];
      v[i*9+9] = (H3DFloat) m[1][2];
      v[i*9+10] = (H3DFloat) m[2][2];
      v[i*9+11] = (H3DFloat) m[3][2];
      v[i*9+12] = (H3DFloat) m[0][3];        
      v[i*9+13] = (H3DFloat) m[1][3];
      v[i*9+14] = (H3DFloat) m[2][3];
      v[i*9+15] = (H3DFloat) m[3][3];
    }
    return v;
  }

}

using namespace X3DProgrammableShaderObjectInternals;

/// Set the value of a uniform variable in the current GLSL shader.
/// The name of the uniform variable is the same as the name of the field. 
bool X3DProgrammableShaderObject::setUniformVariableValue( 
                                       GLhandleARB program_handle,
                                       Field *field ) {
  const string &name = field->getName();
  GLint location = glGetUniformLocationARB( program_handle,
                                            name.c_str() );
  if( location == -1 ) return false;
      
  // clear the error flag
  glGetError();
  if( SFBool *f = dynamic_cast< SFBool * >( field ) ) {
    glUniform1iARB( location, f->getValue() );
  } else if( MFBool *f = dynamic_cast< MFBool * >( field ) ) {
    GLint *v = toIntArray( f->getValue() );
    glUniform1ivARB( location, f->size(), v );
    delete v;
  } else if( SFInt32 *f = dynamic_cast< SFInt32 * >( field ) ) {
    glUniform1iARB( location, f->getValue() );
  } else if( MFInt32 *f = dynamic_cast< MFInt32 * >( field ) ) {
    glUniform1ivARB( location, f->size(), &(f->getValue()[0]) );
  } else if( SFFloat *f = dynamic_cast< SFFloat * >( field ) ) {
    glUniform1fARB( location, f->getValue() );
  } else if( MFFloat *f = dynamic_cast< MFFloat * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform1fvARB( location, f->size(), v );
    delete v;
  } else if( SFDouble *f = dynamic_cast< SFDouble * >( field ) ) {
    glUniform1fARB( location, (GLfloat)f->getValue() );
  } else if( MFDouble *f = dynamic_cast< MFDouble * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform1fvARB( location, f->size(), v );
    delete v;
  } else if( SFTime *f = dynamic_cast< SFTime * >( field ) ) {
    glUniform1fARB( location, (GLfloat)f->getValue() );
  } else if( MFTime *f = dynamic_cast< MFTime * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform1fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec2f *f = dynamic_cast< SFVec2f * >( field ) ) {
    const Vec2f &v = f->getValue(); 
    glUniform2fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y );
  } else if( MFVec2f *f = dynamic_cast< MFVec2f * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform2fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec3f *f = dynamic_cast< SFVec3f * >( field ) ) {
    const Vec3f &v = f->getValue(); 
    glUniform3fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y,
                    (GLfloat)v.z );
  } else if( MFVec3f *f = dynamic_cast< MFVec3f * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform3fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec4f *f = dynamic_cast< SFVec4f * >( field ) ) {
    const Vec4f &v = f->getValue(); 
    glUniform4fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y,
                    (GLfloat)v.z,
                    (GLfloat)v.w  );
  } else if( MFVec4f *f = dynamic_cast< MFVec4f * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform4fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec2d *f = dynamic_cast< SFVec2d * >( field ) ) {
    const Vec2d &v = f->getValue(); 
    glUniform2fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y );
  } else if( MFVec2d *f = dynamic_cast< MFVec2d * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform2fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec3d *f = dynamic_cast< SFVec3d * >( field ) ) {
    const Vec3d &v = f->getValue(); 
    glUniform3fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y,
                    (GLfloat)v.z );
  } else if( MFVec3d *f = dynamic_cast< MFVec3d * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform3fvARB( location, f->size(), v );
    delete v;
  } else if( SFVec4d *f = dynamic_cast< SFVec4d * >( field ) ) {
    const Vec4d &v = f->getValue(); 
    glUniform4fARB( location, 
                    (GLfloat)v.x,
                    (GLfloat)v.y,
                    (GLfloat)v.z,
                    (GLfloat)v.w  );
  } else if( MFVec4d *f = dynamic_cast< MFVec4d * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform4fvARB( location, f->size(), v );
    delete v;
  } else if( SFRotation *f = dynamic_cast< SFRotation * >( field ) ) {
    const Rotation &r = f->getValue(); 
    glUniform4fARB( location, 
                    (GLfloat)r.axis.x,
                    (GLfloat)r.axis.y,
                    (GLfloat)r.axis.z,
                    (GLfloat)r.angle  );
  } else if( MFRotation *f = dynamic_cast< MFRotation * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform4fvARB( location, f->size(), v );
    delete v;
  } else if( SFColor *f = dynamic_cast< SFColor * >( field ) ) {
    const RGB &r = f->getValue(); 
    glUniform3fARB( location, 
                    (GLfloat)r.r,
                    (GLfloat)r.g,
                    (GLfloat)r.b );
  } else if( MFColor *f = dynamic_cast< MFColor * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform4fvARB( location, f->size(), v );
    delete v;
  } else if( SFColorRGBA *f = dynamic_cast< SFColorRGBA * >( field ) ) {
    const RGBA &r = f->getValue(); 
    glUniform4fARB( location, 
                    (GLfloat)r.r,
                    (GLfloat)r.g,
                    (GLfloat)r.b,
                    (GLfloat)r.a );
  } else if( MFColorRGBA *f = dynamic_cast< MFColorRGBA * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniform4fvARB( location, f->size(), v );
    delete v;
  } else if( SFMatrix3f *f = dynamic_cast< SFMatrix3f * >( field ) ) {
    const Matrix3f &m = f->getValue(); 
    glUniformMatrix3fvARB( location, 
                           1,
                           true,
                           m[0] );
  } else if( MFMatrix3f *f = dynamic_cast< MFMatrix3f * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniformMatrix3fvARB( location, f->size(), false, v );
    delete v;
  } else if( SFMatrix4f *f = dynamic_cast< SFMatrix4f * >( field ) ) {
    const Matrix4f &m = f->getValue(); 
    glUniformMatrix3fvARB( location, 
                           1,
                           true,
                           m[0] );
  } else if( MFMatrix4f *f = dynamic_cast< MFMatrix4f * >( field ) ) {
    GLfloat *v = toFloatArray( f->getValue() );
    glUniformMatrix4fvARB( location, f->size(), false, v );
    delete v;
  } else if( SFNode *f = dynamic_cast< SFNode * >( field ) ) {
    Node *n = f->getValue(); 
    if( X3DTexture2DNode *t = dynamic_cast< X3DTexture2DNode *>( n ) ) {
      glUniform1iARB( location, t->getTextureUnit() - GL_TEXTURE0_ARB );
    } else if( X3DTexture3DNode *t = 
               dynamic_cast< X3DTexture3DNode *>( n ) ) {
      glUniform1iARB( location, t->getTextureUnit() - GL_TEXTURE0_ARB );
    } else {
      return false;
    }
                                                                           
  } else if( MFNode *f = dynamic_cast< MFNode * >( field ) ) {
    unsigned int size = f->size();
    GLint *v = new GLint[ size ];
    for( unsigned int i = 0; i < size; i++ ) {
      Node *n = f->getValueByIndex( i ); 
      if( X3DTexture2DNode *t = dynamic_cast< X3DTexture2DNode *>( n ) ) {
        v[i] = t->getTextureUnit() - GL_TEXTURE0_ARB;
      } else if( X3DTexture3DNode *t = 
                 dynamic_cast< X3DTexture3DNode *>( n ) ) {
        v[i] = t->getTextureUnit() - GL_TEXTURE0_ARB;
      } else {
        delete v;
        return false;
      }
    }
    glUniform1ivARB( location, size, v );
  } else {
    return false;
  }
  
  // ignore any errors that occurs when setting uniform variables.
  return glGetError() == GL_NO_ERROR;
}
