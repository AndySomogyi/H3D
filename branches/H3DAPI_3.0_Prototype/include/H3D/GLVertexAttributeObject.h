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
/// \file GLVertexAttributeObject.h
/// \brief Contains the GLVertexAttributeObject class.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __GLVERTEXATTRIBUTE_H__
#define __GLVERTEXATTRIBUTE_H__

#include <H3D/H3DApi.h>
#include <H3D/Node.h>
#include <H3D/SFBool.h>
#include <GL/glew.h>
#include <H3D/VertexAttributeDescription.h>

namespace H3D 
{
	namespace VERTEXATTRIBUTE {
		enum VERTEXATTRIBUTETYPE {
			VERTEX = 0,
			NORMAL,
			TEXCOORD,
			FOGCOORD,
			COLOR,
			GENERIC
		};
	}

	/// \ingroup AbstractInterface
	/// \class GLVertexAttributeObject
	/// \brief This abstract interface is inherited by all nodes that 
	/// could be used as vertex attributes in a glsl shader.
	///
	/// Example of such nodes are Color, Coordinate and Normal nodes.
	class H3DAPI_API GLVertexAttributeObject
	{
	public:
		/// Constructor
		GLVertexAttributeObject ( VERTEXATTRIBUTE::VERTEXATTRIBUTETYPE type );

		/// Destructor
		virtual ~GLVertexAttributeObject();

		/// pure virtual function has to be implemented by inherited class
		/// to specify necessay data and format for this vertex attrib
		virtual void setAttributeData ( ) = 0;

		/// pure virtual function has to be implemented by inherited class
		/// to specify the actual render function
		virtual void renderVBO ( ) = 0;

		/// pure virtual function has to be implemented by inherited class
		/// to specify the actual render disable function
		virtual void disableVBO ( ) = 0;

		/// pre-render check to determine if need to render this vertex attribute
		virtual bool preRenderCheckFail( );

		/// Perform the OpenGL commands to render all vertices as a vertex
		/// buffer object.
		void renderVertexBufferObject ( );

		/// Disable the vertex buffer object enabled in renderVertexBufferObject().
		void disableVertexBufferObject ( );

		/// Perform the OpenGL commands to update vertex attribute data/format
		virtual void updateVertexBufferObject ( );

		/// Option to indicate whether this vertex attribute is dynamic or not
		/// <b>Access type:</b> inputOutput \n
		/// <b>Default value:</b> false
		auto_ptr<SFBool> isDynamic;

		/// Returns whether this attribute is bindless or not
		inline bool usesBindless() {
			return use_bindless;
		}

		/// Based on whether this attribute is bindless or not
		/// it either returns the 64bit unsigned gpu addr or the normal vbo_id as a 32bit uint.
		inline GLuint64 getVBOPtr() {
			return vbo_GPUaddr;
		}
		
		inline GLuint getVBO(){
			return VAD.bufferID;
		}

		inline GLsizei getAttribSize() {
			return VAD.attributeSize;
		}

		inline GLint getElementCount() {
			return VAD.elementCount;
		}

		inline GLenum getPrimitiveType() {
			return VAD.primitiveType;
		}

		//Size of each individual element
		inline GLsizei getStride() {
			return VAD.stride;
		}

		inline bool isNormalized() {
			return VAD.normalized;
		}

		inline VertexAttributeDescription getVAD(){
			return VAD;
		}

	protected:
		/// Internal field used to know if vertex buffer object can be created.
		auto_ptr<Field> vboFieldsUpToDate;

		//Maybe just make a "build vertex attribute description" function...
		VertexAttributeDescription VAD;

		/// Do not be alarmed that we don't delete this object in our destructor. Its' lifetime is governed elsewhere.
		GLvoid* attrib_data;

		/// If we use bindless rendering, this variable will contain the actual pointer into the GPU memory where this buffer is stored.
		GLuint64EXT vbo_GPUaddr;

		/// the type of vertex attribute
		VERTEXATTRIBUTE::VERTEXATTRIBUTETYPE attrib_type;

		bool use_bindless;
	};
}

#endif
