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
#include <H3D/VertexAttributeDescription.h>

namespace H3D {
	/// Struct used to draw one object consisting of several VBOs, one IBO and some other stuff
	struct ElementDrawData
	{
		ElementDrawData();

		Matrix4f* transform;
		GLuint indexCount;
		GLuint IBO;
		std::vector<VertexAttributeDescription> VBOs;
	};

	/// Struct used for indirect rendering. 
	/// Contains all the data needed for the GPU to parse a draw call.
	struct IndirectRenderData {
		IndirectRenderData()
		:	index(0), reserved(0), 
			address(0), length(0)
		{}
		GLuint   index;
		GLuint   reserved; 
		GLuint64 address;
		GLuint64 length;
	};

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

	struct DrawCommand {
		DrawCommand()
		: cmd(), reserved(0), indexBuffer(), vertexBuffers()
		{}

		DrawElementsIndirectCommand		cmd;
		GLuint							reserved; 
		IndirectRenderData              indexBuffer;
		IndirectRenderData				vertexBuffers[2]; //vector ptr from Renderable
	};

	/// Temporary object that should hold everything relevant for rendering an object.
	/// Each object that you want to render will have to create one of these during traverseSG 
	/// and send it to the Renderer. Use Renderer::insertNewRenderable
	struct RenderData {

		RenderData();

		/// One per VBO. One for coordinates, one for normals, one for texcoords etc.
		/// We have no guarantees of what this is, so it needs to be as flexible as possible
		std::vector<IndirectRenderData> VBOs;

		/// Index buffer object.
		IndirectRenderData IBO;

		/// Transform for this renderable. At the moment I think it will need to be copied by value. A ptr would be subject to change.
		Matrix4f modelTransform;

		/// Vertex Attribute Object... Sort of. Will be used when sorting objects.
		unsigned int vertexAttributeLayout;

		/// Renderstate for this Renderable. We'll sort based on this.
		unsigned int renderState;

	};

	//These are the ones we will send in for rendering
	struct RenderBucket {
		RenderBucket();

		std::vector<DrawCommand> renderables;
		std::vector<ElementDrawData> elementRenderables;

		unsigned int numVBOs;

		//Handle into vector of vertex attribute layouts in Renderer
		unsigned int vertexAttributeLayout;

		//Handle into the vector of renderstates in Renderer
		unsigned int renderState;
	};
}
#endif