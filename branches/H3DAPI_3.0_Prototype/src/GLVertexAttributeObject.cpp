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
/// \file GLVertexAttributeObject.cpp
/// \brief cpp file for GLVertexAttributeObject class.
//
//////////////////////////////////////////////////////////////////////////////

#include <H3D/GLVertexAttributeObject.h>
#include <H3D/OGLUtil.h>

using namespace H3D;

GLVertexAttributeObject::GLVertexAttributeObject ( VERTEXATTRIBUTE::VERTEXATTRIBUTETYPE type) :
vboFieldsUpToDate ( new Field ),
	VAD(),
	isDynamic(new SFBool(false)),
	attrib_type(type),
	attrib_data( NULL ),
	vbo_GPUaddr( 0 ),
	use_bindless( H3D::USING_BINDLESS )
{
	vboFieldsUpToDate->setName ("vboFieldsUpToDate");
	isDynamic->route (vboFieldsUpToDate);

	target = GL_ARRAY_BUFFER;

	//if(GLEW_EXT_direct_state_access && GL_NV_vertex_buffer_unified_memory)
	//{
	//	use_bindless = true;
	//}
}

GLVertexAttributeObject::~GLVertexAttributeObject() 
{
	if(vbo_ID != 0) 
	{
		if(use_bindless)
		{
			glBindBuffer(target, vbo_ID);
			glUnmapBuffer(target);
			glDeleteBuffersARB(1, &vbo_ID);
			vbo_ID = 0;
		}
		else
		{
			glDeleteBuffersARB(1, &vbo_ID);
			vbo_ID = 0;
		}
	}
}

bool GLVertexAttributeObject::preRenderCheckFail()
{
	return (!GLEW_ARB_vertex_program);
}

void GLVertexAttributeObject::updateVertexBufferObject() {
	
	if(!vboFieldsUpToDate->isUpToDate()) {
		vboFieldsUpToDate->upToDate();
		setAttributeData();

		if(vbo_ID == 0) {
			glGenBuffersARB(1, &vbo_ID);
		}

		glBindBuffer(target, vbo_ID);

		if(use_bindless)
		{
			glBufferStorage(target, attrib_size, attrib_data, GL_MAP_WRITE_BIT|GL_MAP_COHERENT_BIT|GL_MAP_PERSISTENT_BIT);
			glGetBufferParameterui64vNV(target, GL_BUFFER_GPU_ADDRESS_NV, &vbo_GPUaddr);
			glMakeBufferResidentNV(target, GL_READ_ONLY);

			LogOGLErrors("GLVertexAttributeObject::updateVertexBufferObject.");
		}
		else
		{
			if(isDynamic->getValue()) {
				glBufferData(target, attrib_size, attrib_data, GL_STREAM_DRAW);
			} else {
				glBufferData(target, attrib_size, attrib_data, GL_STATIC_DRAW);
			}

			LogOGLErrors("GLVertexAttributeObject::updateVertexBufferObject.");
		}

		glBindBuffer(target, 0);
	} 
	else
	{
		if(!use_bindless)
		{
			glBindBuffer (target, vbo_ID);
		}
	}
}

void GLVertexAttributeObject::renderVertexBufferObject()
{
	if(preRenderCheckFail())
	{ 
		return;
	}

	updateVertexBufferObject();
	renderVBO();
}

void GLVertexAttributeObject::disableVertexBufferObject()
{
	if(preRenderCheckFail()) 
	{
		return;
	}

	disableVBO();

	if(!use_bindless)
	{
		glBindBufferARB(target, 0);
	}
}


