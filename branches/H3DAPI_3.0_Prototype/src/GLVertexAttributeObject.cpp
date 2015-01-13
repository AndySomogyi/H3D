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
	isDynamic(new SFBool),
	attrib_type(type),
	attrib_data( NULL ),
	vbo_GPUaddr( 0 ),
	use_bindless( H3D::USING_BINDLESS )
{
	vboFieldsUpToDate->setName ("vboFieldsUpToDate");
	isDynamic->route (vboFieldsUpToDate);
	isDynamic->setValue (false);

	VAD.target = GL_ARRAY_BUFFER;
	VAD.normalized = false;

	//if(GLEW_EXT_direct_state_access && GL_NV_vertex_buffer_unified_memory)
	//{
	//	use_bindless = true;
	//}
}

GLVertexAttributeObject::~GLVertexAttributeObject() 
{
	if(VAD.bufferID != 0) 
	{
		if(use_bindless)
		{
			glBindBuffer(GL_ARRAY_BUFFER, VAD.bufferID);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glDeleteBuffersARB(1, &VAD.bufferID);
			VAD.bufferID = 0;
		}
		else
		{
			glDeleteBuffersARB(1, &VAD.bufferID);
			VAD.bufferID = 0;
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

		if(VAD.bufferID == 0) {
			glGenBuffersARB(1, &VAD.bufferID);
		}

		glBindBuffer(GL_ARRAY_BUFFER, VAD.bufferID);

		if(use_bindless)
		{
			//glBufferStorage(GL_ARRAY_BUFFER, VAD.attributeSize, attrib_data, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT);
			glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &vbo_GPUaddr);
			glMakeBufferResidentNV(GL_ARRAY_BUFFER, GL_READ_ONLY);

			LogOGLErrors("GLVertexAttributeObject::updateVertexBufferObject.");
		}
		else
		{
			if(isDynamic->getValue())
			{
				glBufferData(GL_ARRAY_BUFFER, VAD.attributeSize, attrib_data, GL_STREAM_DRAW);
			} 
			else
			{
				glBufferData(GL_ARRAY_BUFFER, VAD.attributeSize, attrib_data, GL_STATIC_DRAW);
			}

			LogOGLErrors("GLVertexAttributeObject::updateVertexBufferObject.");
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} 
	else
	{
		if(!use_bindless)
		{
			glBindBuffer (GL_ARRAY_BUFFER, VAD.bufferID);
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
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
}


