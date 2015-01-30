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
/// \file Renderer.h
/// \brief Header file for Renderer, Core class for everything rendering related.
///
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <GL/glew.h>
#include <H3D/Renderable.h>
#include <H3D/RenderMetaData.h>
#include <H3D/RenderCommandBuffer.h>
#include <H3D/OGLBuffer.h>
#include <vector>
#include <utility> //For std::pair

namespace H3D {
	class Renderer {
	public:
		/******************************************************************************/
		/*										Public functions					  */
		/******************************************************************************/

		Renderer();
		virtual ~Renderer();

		void SetRebuildCommandbuffer(bool _val)
		{
			rebuildCommandBuffer = _val;
		}

		/// After we've traversed the entire scene with traverseSG(), we need to sort and structure everything before rendering.
		/// We fill up the drawCommands vector with... draw commands.
		virtual void update();

		/// Render the entire scene in as few drawcalls as possible. Uses the drawCommands vector if we have support for multi indirect extension, else we just operate on the renderables vector. 
		virtual void render();


		//TODO ON WEDNESDAY: Remove the two below and replace with a single unified "insertVertexBufferContainers" thingy or insertRenderable.

		/// Should be called every frame if you want something rendered.
		void insertNewRenderable(Renderable* renderable);

		/// Inserts a total render state, compares against the others that are already inside, and returns a handle to where the state is stored for future reference.
		unsigned int insertNewTotalRenderState(TotalRenderState& totalRenderState);

		///	So we'll insert a vector describing an entire layout.
		/// Internally it'll actually do a comparison to the others. 
		/// If we find an identical one already in the vector, we just return the index to that one.
		unsigned int insertVertexAttributeLayout(std::vector<VertexAttributeDescription> layout);

		void setViewportSize(float width, float height);
		void setCurrentViewpoint(Vec3f position, Vec3f direction, Vec3f up = Vec3f(0.0f, 1.0f, 0.0f));
		
		inline bool getBindlessUsage() {
			return usingBindless;
		}

		void setWorldUniformIndex(GLuint index) {
			worldMatrixUniformLocation = index;
		}

	private:

		/******************************************************************************/
		/*							  Private structs								  */
		/******************************************************************************/
	
		//Oh well.

	private:

		/******************************************************************************/
		/*								Private functions							  */
		/******************************************************************************/
		/// This function will compare previousRenderState to the renderstate that is inside renderbucket.
		/// For each difference, we create a new state change command and insert it into the render command buffer
		void compareRenderStates(TotalRenderState& previousRenderState, const RenderBucket& renderBucket, RenderCommandBuffer& commandBuffer);
		void InsertDefaultRenderstate(RenderCommandBuffer& commandBuffer);


		void updateViewMatrix();
	private:
		/******************************************************************************/
		/*								Private members								  */
		/******************************************************************************/

		/// Changes the way we operate upon rendering data
		bool usingBindless;

		/// Something in the scene has changed so we need to rebuild the commandBuffer
		bool rebuildCommandBuffer;

		float viewportHeight, viewportWidth;

		///	Container for all the renderables used each frame
		std::vector<Renderable*> renderables;
		std::vector<Matrix4f> transforms;

		/// Wrapper for our openGL buffer for transforms
		CircularBuffer transformBuffer;

		/// Container for all of the render buckets that we will be calling each frame.
		std::vector<RenderBucket> renderBuckets;

		/// Data structure for containing all the render commands. Only needs to be cleared/rebuilt if anything has changed.
		RenderCommandBuffer renderCommandBuffer;

		/// Vector of vectors. Each vector will contain a bunch of attribute layouts 
		/// to describe what an object will look like when sent to the vertex shader.
		std::vector<std::vector<VertexAttributeDescription>> vertexAttributeLayouts;
		std::vector<GLuint> attributeSlotsToBeFreed;

		/// Container for all the different render state setups.
		std::vector<TotalRenderState> totalRenderStates;
		std::vector<std::pair<GLenum, bool>> clientStatesToEnable;

		/// This is the default, fallback state that we will compare against to see if we need to create a new "ChangeState" command.
		TotalRenderState defaultRenderState;

		/// Variable to keep track of how many objects our scene contains. 
		/// Also determines how many transforms we're saving and inserting into our transform buffer every frame.
		size_t objectCount;

		/// We query the GPU on how many attribute slots that are supported.
		GLint maxVertexAttribSlots;

		//Just putting it here and hardcoding it...
		Matrix4f view;
		Matrix4f projection;
		Matrix4f viewProjection;
		GLuint worldMatrixUniformLocation;

		GLuint dummyVAO;
		//Our own orientation vars
		Vec3f lookAtVector, upVector, eyeVector;

		float elapsedTime;
		TimeStamp time, previousTime;
	};
}

#endif