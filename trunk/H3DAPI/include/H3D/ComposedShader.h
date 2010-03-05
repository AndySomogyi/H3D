//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
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
/// \file ComposedShader.h
/// \brief Header file for ComposedShader, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __COMPOSEDSHADER_H__
#define __COMPOSEDSHADER_H__

#include <H3D/X3DShaderNode.h>
#include <H3D/ShaderPart.h>
#include <H3D/X3DProgrammableShaderObject.h>
#include <H3D/DependentNodeFields.h>
#include<string>

namespace H3D {

  /// \ingroup X3DNodes
  /// \class ComposedShader
  /// \brief The ComposedShader node defines a shader where the individual 
  /// source files are not individually programmable. All access to the
  /// shading capabilities is defined through a single interface that 
  /// applies to all parts.
  ///
  /// In H3D API this is the shader node to use if you want to use the
  /// GLSL shading language. If you want to use CG, use either the 
  /// ProgramShader or PackagedShader nodes.
  ///
  /// The isValid field adds an additional semantic indicating whether 
  /// the current shader parts can be linked together to form a complete
  /// valid shader program.
  /// 
  /// The activate field forces the shader to activate the contained 
  /// objects. 
  ///
  /// \par Internal routes:
  /// \dotfile ComposedShader.dot
  class ComposedShader : 
    public X3DShaderNode, 
    public X3DProgrammableShaderObject {
  public:
  
    /// The MFShaderPart is dependent on the url field of the
    /// containing ShaderPart node.
    typedef DependentMFNode< ShaderPart,
                             FieldRef< X3DUrlObject, 
                                       MFString, 
                                       &ShaderPart::url >,
                             true >
    MFShaderPart;

    /// Constructor.
    ComposedShader( Inst< DisplayList  > _displayList = 0,
                    Inst< SFNode       > _metadata    = 0,
                    Inst< SFBool       > _isSelected  = 0,
                    Inst< SFBool       > _isValid     = 0,
                    Inst< SFBool       > _activate    = 0,
                    Inst< SFString     > _language    = 0,
                    Inst< MFShaderPart > _parts       = 0,
                    Inst< SFBool       > _suppressUniformWarnings = 0 );

    /// The addField method is specialized to add a route from the field
    /// added to the displayList field.
    virtual bool addField( const string &name,
                           const Field::AccessType &access,
                           Field *field );
    
    /// Sets up the shader program and sets uniform variable values.
    virtual void render();

    /// Enables the shader program.
    virtual void preRender();

    /// Disables the shader program.
    virtual void postRender();

    /// The shader parts to use in the ComposedShader 
    ///
    /// <b>Access type:</b> outputOnly \n
    /// 
    /// \dotfile ComposedShader_parts.dot
    auto_ptr< MFShaderPart > parts;

    /// Suppress the warnings printed to the console about setting uniform fields
    /// that do not exist in the shader. These warnings are printed when you have
    /// added a field to your shader which is not defined in the glsl shader code,
    /// if the uniform variable is defined in the shader code but not used anywhere,
    /// or when the variable is used in the code but optimized away by the glsl 
    /// compiler. Usually we would only like to print a warning in the first case
    /// and not in the others, but there is no way to distinguish between them.
    /// We therefore leave it to the setting of this field if they should be
    /// printed or not.
    ///
    /// <b>Access type:</b> inputOutput \n
    /// <b>Default value:</b> false \n
    /// 
    /// \dotfile ComposedShader_parts.dot
    auto_ptr< SFBool > suppressUniformWarnings;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;
  protected:
    static bool shader_support_checked;  

    /// The handle to the program object used for the shader in OpenGL.
    GLhandleARB program_handle;

    /// A vector of the handles to all shader objects that are currently
    /// linked into the program object.
    vector< GLhandleARB > current_shaders;

    // list of program handles separated by key
    static map<string, GLhandleARB> phandles_map;
    static map<GLhandleARB, int> phandle_counts;

    // create program handle for a shader
    static GLhandleARB createHandle(const ComposedShader* shader);

    // try to find existing handle (with the same signature), if not, go create one
    static std::string genKeyFromShader(const ComposedShader* shader);

  };
}

#endif
