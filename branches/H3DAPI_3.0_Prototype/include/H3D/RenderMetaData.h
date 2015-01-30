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
// \file RenderData.h
// \brief Header file for RenderData, collection class for a bunch of 
// different structs necessary for rendering to work. 
// Kept in a separate file from Renderer to make compile times faster.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __RENDERDATA_H__
#define __RENDERDATA_H__

#include <vector>
#include <string>
#include <GL/glew.h>
#include <H3D/H3DTypes.h>
#include <H3D/Renderable.h>

namespace H3D {
	//Struct used to queue up draw commands.
	//Part of the multidraw indirect technique
	//Might be referred to as DEICmd in some places because long name
	struct DrawElementsIndirectCommand {
		DrawElementsIndirectCommand()
		:	count(0), instanceCount(0), firstIndex(0),
			baseVertex(0), baseInstance(0)
		{}

		GLuint count;
		GLuint instanceCount;
		GLuint firstIndex;
		GLuint baseVertex;
		GLuint baseInstance;
	};

	/// Struct used for indirect rendering. 
	/// Contains all the data needed for the GPU to parse a draw call.
	struct IndirectRenderData {
		IndirectRenderData(GLuint _index = 0, GLuint _reserved = 0, 
			GLuint64 _address = 0, GLuint _length = 0)
			:	index(_index), reserved(_reserved), 
			address(_address), length(_length)
		{}

		GLuint   index; //Attribute index I think?
		GLuint   reserved;  // .. Should be 0
		GLuint64 address; // 64bit buffer ptr
		GLuint64 length; // size of the persistent buffer in bytes
	};

	struct DrawCommand {
		DrawCommand()
		: cmd(), reserved(0), indexBuffer(), vertexBuffers()
		{}

		DrawElementsIndirectCommand	cmd;
		GLuint											reserved; 
		IndirectRenderData					indexBuffer;
		IndirectRenderData					vertexBuffers[2]; //vector ptr from Renderable
	};

	//These are the ones we will send in for rendering
	struct RenderBucket {
		RenderBucket();

		std::vector<Renderable*> renderables;

		unsigned int numVBOs;

		//Handle into vector of vertex attribute layouts in Renderer
		unsigned int vertexAttributeLayout;

		//Handle into the vector of renderstates in Renderer
		unsigned int renderState;
	};
}
#endif