#include <H3D/Renderer.h>

#include <H3D/X3DGeometryNode.h>
#include <H3D/OGLUtil.h>
#include <H3D/MathUtil.h>
#include <H3DUtil/Vec3f.h>

using namespace H3D;

H3D::Renderer::Renderer()
	: transformBuffer(true),
	renderCommandBuffer(128),
	lookAtVector(0.0f, 0.0f, 1.0f),
	upVector(0.0f, 0.0f, 1.0f),
	eyeVector(0.0f, 3.0f, 40.0f),
	usingBindless(true)
{
	time = time.now();
	previousTime = time;
	elapsedTime = 0.0f;
	objectCount = 128;
	rebuildCommandBuffer = true;
	maxVertexAttribSlots = 16;

	viewportHeight = 768.0f;
	viewportWidth = 1024.0f;
	setViewportSize(viewportWidth, viewportHeight);
	updateViewMatrix();


	//Default construct one renderstate to contain the default values for everything.
	totalRenderStates.resize(1);
	vertexAttributeLayouts.resize(1);

	//I have no idea
	renderables.reserve(objectCount);
	transforms.reserve(objectCount);

	//Read max amount of concurrent vertex attributes
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribSlots);

	//#ifdef GLEW_ARB_multi_draw_indirect
	usingBindless = false;
	//#else
	//	multidrawIndirectEnabled = false;
	//#endif

	if(usingBindless)
	{
		clientStatesToEnable.push_back(std::pair<GLenum, bool>(GL_ELEMENT_ARRAY_UNIFIED_NV, true));
		clientStatesToEnable.push_back(std::pair<GLenum, bool>(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV, true));
	}

	const GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
	const GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

	transformBuffer.Create(PersistentlyMappedBuffer, GL_SHADER_STORAGE_BUFFER, 3*objectCount, createFlags, mapFlags);

	glGenVertexArrays(1, &dummyVAO);
	glBindVertexArray(dummyVAO);
}


H3D::Renderer::~Renderer() {
	transformBuffer.Destroy();
	glDisableVertexAttribArray(dummyVAO);
	glDeleteVertexArrays(1, &dummyVAO);
}

void  H3D::Renderer::update() {

	time = TimeStamp::now();
	elapsedTime += (time - previousTime);
	previousTime = time;

	if(elapsedTime >= 10.0f) {
		elapsedTime = 0.0f;
	}

	/*
	So this function will look something like this:

	At the beginning of update, we'll have a big bunch of renderables, containing stuff like VBOs, VAOs, Renderstates(?), a transform, possibly other things

	Then we will go through all of this data and sort it and put everything in render buckets if they have the same format and settings.

	When everything is sorted, we will build up a series of draw commands (if this is supported), else we'll just move on to the render function
	*/

	//Copy construct into a raw float[4][4] format that can be sent to the GPU without problems.

	if(rebuildCommandBuffer) {
		//Turn on all enabled client states
		renderCommandBuffer.InsertNewCommand(new ChangeClientStateCommand(clientStatesToEnable));

		/************************************************************************/
		/* Pre-process all the data so that we can sort it properly.	        */
		/************************************************************************/
		DrawCommand drawCommand;
		bool drawCmdIsUnique = true;

		//At the moment, these vars don't change on a per-object basis anyway, so just init them once.
		drawCommand.cmd.baseInstance = 0;
		drawCommand.cmd.baseVertex = 0;
		drawCommand.cmd.firstIndex = 0;
		drawCommand.cmd.instanceCount = 1;
		drawCommand.reserved = 0;

		//Stride through all of the renderables and gather everything interesting and put it in the right place
		for(unsigned int i = 0; i < renderables.size(); ++i) {

			//Insert the transform for this particular renderable
			transforms.push_back(renderables[i]->modelTransform);

			//Save ptr to this transform for element renderable
			elementRenderables[i]->transform = &transforms[transforms.size()-1];

			//Fill up the draw command parameters
			drawCommand.cmd.count = static_cast<GLuint>(renderables[i]->IBO.length);
			drawCommand.indexBuffer = renderables[i]->IBO;
			drawCommand.vertexBuffers[0] = renderables[i]->VBOs[0];
			drawCommand.vertexBuffers[1] = renderables[i]->VBOs[1];
			//drawCommand.vertexBuffers = &(*renderables[i]->VBOs.begin());

			//Go through each render bucket and see if any of them has a total render state that matches the one of this renderable.
			//If we don't find a render bucket that matches, we create another bucket and insert the renderable there instead.
			for(unsigned int k = 0; k < renderBuckets.size(); ++k) {

				//If this renderable's render state and attribute layout matches this renderbucket, insert the renderable and break inner loop
				if(	renderables[i]->renderState == renderBuckets[k].renderState && 
					renderables[i]->vertexAttributeLayout == renderBuckets[k].vertexAttributeLayout) {
						drawCmdIsUnique = false;
						renderBuckets[k].renderables.push_back(drawCommand);
						renderBuckets[k].elementRenderables.push_back(*(elementRenderables[i]));
						break;
				}
			}

			//The bunch of code below this point might be the source of a problem. The renderbucket with 4 renderables has the vertexAttributeLayout == 0 which is totally wrong.

			//If none of the renderbuckets match, we create a new renderbucket and insert it.
			if(drawCmdIsUnique) {
				//Create a new renderbucket with the right state and vertex layout
				RenderBucket newBucket;
				newBucket.renderState = renderables[i]->renderState;
				newBucket.vertexAttributeLayout = renderables[i]->vertexAttributeLayout;

				//Sort of a clunky way to figure out the number of VBOs we have for this particular render bucket, but it works and isn't super slow
				std::vector<VertexAttributeDescription>& vboVec = vertexAttributeLayouts[newBucket.vertexAttributeLayout].first;
				newBucket.numVBOs = vboVec.size();

				//Insert it into renderbuckets.... inefficient copy construct, but don't think there's any way around it.
				renderBuckets.push_back(newBucket);

				// Get furthest out renderbucket, aka the one we just inserted. Then into that renderbucket, 
				// insert the newest drawcommand that we tried to insert into the other renderbuckets (but failed because of no match)
				renderBuckets[renderBuckets.size()-1].renderables.push_back(drawCommand);
				renderBuckets[renderBuckets.size()-1].elementRenderables.push_back(*elementRenderables[i]);
			}
		}

		/************************************************************************/
		/*	This is where we're supposed to process and insert all the		
		necessary commands to renderCommandBuffer							*/
		/************************************************************************/

		//Set up "previousState" to initially be completely standard.
		TotalRenderState& previousState = totalRenderStates[0];

		if(usingBindless) {
			//We need to transpose because of the way we send in the matrices (it doesn't automatically transpose like glUniform does with the right flags)
			for(unsigned int i = 0; i < transforms.size(); ++i) {
				transforms[i] = transforms[i].transpose();
			}
		}

		InsertDefaultRenderstate(renderCommandBuffer);

		//Go through all render buckets and queue up state changes + render calls for each one.
		for(unsigned int i = 0; i < renderBuckets.size(); ++i) {

			//First of all we'll just insert the default states to get things going.
			compareRenderStates(previousState, renderBuckets[i], renderCommandBuffer);

			//Insert the actual render command
			if(usingBindless) {		
				//renderCommandBuffer.InsertNewCommand(new UpdateCircularBufferCommand<Matrix4f>(&transformBuffer, &transforms));

				renderCommandBuffer.InsertNewCommand(new MultiDrawElementsIndirectCommand(renderCommandBuffer.GetBuffer(), 
					renderBuckets[i].renderables, GL_TRIANGLES, GL_UNSIGNED_INT, 0, renderBuckets[i].elementRenderables[0].VBOs.size()));

				//renderCommandBuffer.InsertNewCommand(new CleanupCircularBufferCommand<Matrix4f>(&transformBuffer, transforms.size()));

			} else {
				renderCommandBuffer.InsertNewCommand(new DrawElementsBaseVertexCommand(renderBuckets[i].elementRenderables, GL_TRIANGLES, GL_UNSIGNED_INT, 0, worldMatrixUniformLocation));
			}

			//And insert cleanup command afterwards
			if(!attributeSlotsToBeFreed.empty()) {
				renderCommandBuffer.InsertNewCommand(new CleanupAttributeSlotsCommand(attributeSlotsToBeFreed));
			}
		}

		////Turn off all the enabled client states....
		for(unsigned int i = 0; i < clientStatesToEnable.size(); ++i) {
			clientStatesToEnable[i].second = false;
		}

		renderCommandBuffer.InsertNewCommand(new ChangeClientStateCommand(clientStatesToEnable));

		//Important, set flag so that we don't rebuild next update unless it's changed externally
		rebuildCommandBuffer = false;
	} else {
		for(unsigned int i = 0; i < renderables.size(); ++i) {
			transforms.push_back(renderables[i]->modelTransform);
		}
	}
}

void  H3D::Renderer::render() {
	/*
	Set up necessary basic settings and do a clear
	*/
	glViewport(0, 0, viewportWidth, viewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Clear to a beautiful, relaxing purple color...?
	glClearColor(0.4f, 0.35f, 0.65f, 1.0f);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	Fill up our persistent buffer matrix
	*/

	if(usingBindless) {
		Matrix4f* dst = transformBuffer.Reserve(transforms.size());
		memcpy(dst, &*transforms.begin(), sizeof(Matrix4f) * transforms.size());

		transformBuffer.BindBufferRange(0, transforms.size());
	}

	renderCommandBuffer.Render();

	if(usingBindless) {
		transformBuffer.OnUsageComplete(transforms.size());
	}

	//Clear these out at the end of each frame...
	renderables.clear();
	transforms.clear();
	elementRenderables.clear();
}

void  H3D::Renderer::insertNewRenderData(RenderData* const renderData) {
	renderables.push_back(renderData);
}


void H3D::Renderer::insertNewElementData(ElementDrawData* const elementData) {
	elementRenderables.push_back(elementData);
}


unsigned int H3D::Renderer::insertNewTotalRenderState(const Appearance* const appearance, X3DGeometryNode* const geometry) {
	unsigned int retVal = 0;
	RenderProperties* rp = appearance->renderProperties->getValue();

	//Nonstandard render properties
	if(geometry != NULL) //rp != NULL || 
	{
		//Set up a depth state to compare against
		DepthState depthState(rp);

		//Set up blend state to compare against
		BlendState blendState(rp);

		RasterizerState rasterizerState(geometry->usingCulling(), false, geometry->getCullFace(), GL_CCW);

		bool isUnique = false;

		//Compare render properties to each render state apart from the first (the first IS standard)
		for(unsigned int i = 1; i < totalRenderStates.size(); ++i) {
			if(rp != NULL && (depthState != totalRenderStates[i].depthState)) {
				isUnique = true;
				break;
			}

			if(rp != NULL && (blendState != totalRenderStates[i].blendState)) {
				isUnique = true;
				break;
			}

			if(rasterizerState != totalRenderStates[i].rasterizerState) {
				isUnique = true;
				break;
			}
		}

		if(isUnique) {
			TotalRenderState newState;
			newState.blendState = blendState;
			newState.depthState = depthState;
			newState.rasterizerState = rasterizerState;

			totalRenderStates.push_back(newState);

			//Get furthest out index because that's where the newly inserted state is.
			retVal = (totalRenderStates.size()-1);
		}
	}

	//If renderproperties were standard we just return 0, because 
	//totalRenderStates[0] contains a totally standard renderstate
	return retVal;
}


unsigned int H3D::Renderer::insertNewTotalRenderState(TotalRenderState& newRenderState) {

	//Special case if we haven't inserted any other render states than the startup system default.
	//It's an optimization thing to skip on a bunch of comparisons.
	if(totalRenderStates.size() > 1)
	{
		//Compare render properties to each render state apart from the first (the first IS standard)
		for(unsigned int i = 1; i < totalRenderStates.size(); ++i)
		{
			if(newRenderState == totalRenderStates[i]) {
				//Else, if they're identical, retVal should be == i
				return i;
			}
		}
	}

	//So if we've reached this point, there's either no custom totalRenderState and we want to insert it, 
	// OR we've found no matching custom renderState and we want to insert this one.
	totalRenderStates.push_back(newRenderState);

	//Get furthest out index because that's where the newly inserted state is.
	return (totalRenderStates.size()-1);
}


void H3D::Renderer::compareRenderStates(TotalRenderState& previousRenderState, const RenderBucket& renderBucket, RenderCommandBuffer& commandBuffer) {

	TotalRenderState& rs = totalRenderStates[renderBucket.renderState];

	if(previousRenderState.shader.programID != rs.shader.programID) {
		rs.shader.uniforms[0].ptr = viewProjection[0];
		rs.shader.uniforms[1].ptr = &elapsedTime;

		commandBuffer.InsertNewCommand(new ChangeShaderCommand(rs.shader.programID));

		for(unsigned int i = 0; i < rs.shader.uniforms.size(); ++i) {
			commandBuffer.InsertNewCommand(new BindUniformCommand(rs.shader.uniforms[i]));
		}
	}

	if(previousRenderState.blendState != rs.blendState) {
		commandBuffer.InsertNewCommand(new ChangeBlendStateCommand(rs.blendState));
		previousRenderState.blendState = rs.blendState; //Copy value to make sure comparison for next CompareRenderStates call will work as intended.
	}

	if(previousRenderState.depthState != rs.depthState) {
		commandBuffer.InsertNewCommand(new ChangeDepthStateCommand(rs.depthState));
		previousRenderState.depthState = rs.depthState; //Copy value to make sure comparison for next CompareRenderStates call will work as intended.
	}

	if(previousRenderState.rasterizerState != rs.rasterizerState) {
		commandBuffer.InsertNewCommand(new ChangeRasterizerStateCommand(rs.rasterizerState));
		previousRenderState.rasterizerState = rs.rasterizerState; //Copy value to make sure comparison for next CompareRenderStates call will work as intended.
	}

	if(previousRenderState.vertexLayoutDescription != rs.vertexLayoutDescription) {

		std::vector<VertexAttributeDescription>& layout = vertexAttributeLayouts[rs.vertexLayoutDescription].first;

		//This code is just so that I can keep track of what vertex attribute slots will be bound / unbound between state changes, so that I know which to clean up.
		if(previousRenderState.vertexLayoutDescription == 0) {
			attributeSlotsToBeFreed.clear();
			for(unsigned int i = 0; i < layout.size(); ++i) {
				attributeSlotsToBeFreed.push_back(i);
			}
		} else {
			std::vector<VertexAttributeDescription>& previousLayout = vertexAttributeLayouts[previousRenderState.vertexLayoutDescription].first;

			//If the previous layout left extra attribute slots enabled, we want to queue those slots up for disabling
			if(previousLayout.size() > layout.size()) {
				for(unsigned int i = layout.size()-1; i < previousLayout.size(); ++i) {
					attributeSlotsToBeFreed.push_back(i);
				}
			}
		}

		//, false, vertexAttributeLayouts[rs.vertexLayoutDescription].second));
		commandBuffer.InsertNewCommand(new ChangeVertexAttributeSetupCommand(layout, usingBindless));
		previousRenderState.vertexLayoutDescription = rs.vertexLayoutDescription; //Copy value to make sure comparison for next CompareRenderStates call will work as intended.
	}

	//Last of all, we overwrite previousRenderState for next time we want to do a comparison
	previousRenderState = rs;
}


void H3D::Renderer::InsertDefaultRenderstate(RenderCommandBuffer& commandBuffer) {
	TotalRenderState& rs = totalRenderStates[0];

	commandBuffer.InsertNewCommand(new ChangeBlendStateCommand(rs.blendState));
	commandBuffer.InsertNewCommand(new ChangeDepthStateCommand(rs.depthState));
	commandBuffer.InsertNewCommand(new ChangeRasterizerStateCommand(rs.rasterizerState));

	std::vector<VertexAttributeDescription>& layout = vertexAttributeLayouts[rs.vertexLayoutDescription].first;

	attributeSlotsToBeFreed.clear();
	for(unsigned int i = 0; i < layout.size(); ++i) {
		attributeSlotsToBeFreed.push_back(i);
	}
}


unsigned int H3D::Renderer::insertVertexAttributeLayout(std::vector<VertexAttributeDescription> layout, GLuint IBO) {

	// Normal for-loop instead of iterator because I need the index...
	// Loop through all our layouts and see if any of them match the one we just sent in
	for(unsigned int i = 0; i < vertexAttributeLayouts.size(); ++i) {

		// First do size comparison to break early
		if(vertexAttributeLayouts[i].first.size() == layout.size()) {
			std::vector<VertexAttributeDescription>& vec = vertexAttributeLayouts[i].first;
			bool isUnique = false;

			// For each attribute element in the layout
			for(unsigned int k = 0; k < vec.size(); ++k) {
				// Compare each element. Break early if any of the checks fail.
				if(!(vec[k] == layout[k])) {
					isUnique = true;
					break;
				}
			}

			// If isUnique is false here it means that all of the 
			// comparison checks against the previous vector came out true.
			// So we return the index value to that vector instead of adding this one.
			if(isUnique == false) {
				return i;
			}
		}	
	}

	// If we've reached this point, it means that none of the layouts have matched,
	// meaning that we have a new, unique layout to insert.
	vertexAttributeLayouts.push_back(std::pair<std::vector<VertexAttributeDescription>, GLuint>(layout, IBO));

	// Return the furthest out index, because that's 
	// where the newest layout is stored.
	return (vertexAttributeLayouts.size()-1);
}

void H3D::Renderer::setViewportSize(float width, float height)
{
	viewportWidth = width;
	viewportHeight = height;

	projection = MathUtil::CreatePerspectiveProjection(MathUtil::Radians(45.0f), (viewportWidth/viewportHeight), 0.01f, 500.0f);
}

void H3D::Renderer::updateViewMatrix()
{
	view = MathUtil::CreateLookAt(eyeVector, lookAtVector, upVector);
	viewProjection = (view * projection);
}

void H3D::Renderer::setCurrentViewpoint(Vec3f position, Vec3f direction, Vec3f up)
{
	lookAtVector = direction;
	upVector = up;
	eyeVector = position;

	//lookAtVector = Vec3f(0.0f, 0.0f, 0.0f);
	//upVector = Vec3f(0.0f, 1.0f, 0.0f);
	//eyeVector = Vec3f(0.0f, 3.0f, 8.0f);

	updateViewMatrix();
}