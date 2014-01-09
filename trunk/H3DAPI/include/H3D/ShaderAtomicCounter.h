//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
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
/// \file ShaderAtomicCounter.h
/// \brief Header file for ShaderAtomicCounter, X3D scene-graph node
///
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __SHADERATOMICCOUNTER_H__
#define __SHADERATOMICCOUNTER_H__

#include <H3D/ShaderChildNode.h>
#include <H3D/SFInt32.h>
#include <H3D/SFString.h>

namespace H3D {

  /// \ingroup AbstractNodes
  /// \class ShaderAtomicCounter
  /// \brief This is the base node type for the child nodes of the
  ///  ComposedShader node.
  ///
  /// 
  class H3DAPI_API ShaderAtomicCounter : 
    public ShaderChildNode {
  public:
  
    /// Constructor.
    ShaderAtomicCounter( Inst< DisplayList  >  _displayList = 0,
                         Inst< SFNode       >  _metadata = 0,
                         Inst< SFInt32      >  _initialValue = 0
                         );


    auto_ptr<SFInt32> initialValue;

    /// specify the texture_unit will be used for rendering this shader image
    virtual void preRender( unsigned int program );


    virtual void postRender();

    /// update texture and image if necessary, then bind it to shader program
    virtual void render ( );


    virtual void prepareAtomicCounterBuffer ();


    virtual void initialize ( );

    ~ShaderAtomicCounter ( );

    // width, height and depth is used to set the size
    // of the buffer data. It should be just one value, as shader storage
    // buffer is used in shader, to separate it into three dimension
    // will make it more intuitive


    /// The H3DNodeDatabase for this node
    static H3DNodeDatabase database;

  protected:

    /// shader program that this shader storage buffer will be attached
    unsigned int program_handle;

    /// buffer id, generated by openGL
    int buffer_id;


    /// the atomic counter binding point for atomic counter buffer
    /// will be used by the shader to access the data in buffer
    /// this value can be directly set through the binding layout in shader
    /// and inquired through glGetActiveAtomicCounterBufferiv function
    unsigned int atomic_counter_binding;

  };
}

#endif
