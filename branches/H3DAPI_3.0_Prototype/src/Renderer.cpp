#include <H3D/Renderer.h>

#include <H3D/X3DGeometryNode.h>
#include <H3D/OGLUtil.h>
#include <H3D/MathUtil.h>

using namespace H3D;

H3D::Renderer::Renderer()
	: transformBuffer(USING_BINDLESS),
	renderCommandBuffer(128),
	lookAtVector(0.0f, 0.0f, 1.0f),
	upVector(0.0f, 0.0f, 1.0f),
	eyeVector(0.0f, 3.0f, 10.0f),
	usingBindless(USING_BINDLESS) {
	time = time.now();
	previousTime = time;
	elapsedTime = 0.0f;
	objectCount = 64;
	rebuildCommandBuffer = true;
	maxVertexAttribSlots = 16;

	viewportWidth = 1024.0f;
	viewportHeight = 768.0f;
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

	if(usingBindless) {
		clientStatesToEnable.push_back(std::pair<GLenum, bool>(GL_ELEMENT_ARRAY_UNIFIED_NV, true));
		clientStatesToEnable.push_back(std::pair<GLenum, bool>(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV, true));
	}

																																							//64*64 bytes = 4096 bytes = 4kB
	transformBuffer.Create(PersistentlyMappedBuffer, GL_SHADER_STORAGE_BUFFER, objectCount*sizeof(Matrix4f));

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
	elapsedTime += static_cast<float>(time - previousTime);
	previousTime = time;

	if(elapsedTime >= 10.0f) {
		elapsedTime = 0.0f;
	}

	//So when this happens, we do a big re-sort and re-build of the entire RenderCommandBuffer.
	if(rebuildCommandBuffer) {
		//Turn on all enabled client states
		if(clientStatesToEnable.size() > 0) {
			renderCommandBuffer.InsertNewCommand(new ChangeClientStateCommand(clientStatesToEnable));
		}

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

		/************************************************************************/
		/*  
			Go through each renderable. 
			Construct a draw command and insert it into a render bucket.
			If the renderable has a unique render state or vertex 
			attribute layout, we create a new render bucket to hold it.                                                                                                                   
		*/
		/************************************************************************/
		for(unsigned int i = 0; i < renderables.size(); ++i) {

			if(usingBindless) {
				transforms.push_back(renderables[i]->transform);
			}

			//Go through each render bucket and see if any of them has a total render state that matches the one of this renderable.
			//If we don't find a render bucket that matches, we create another bucket and insert the renderable there instead.
			for(unsigned int k = 0; k < renderBuckets.size(); ++k) {
				//If this renderable's render state and attribute layout matches this renderbucket, insert the renderable and break inner loop
				if(	renderables[i]->totalRenderStateHandle == renderBuckets[k].renderState && 
						renderables[i]->vertexAttributeLayout == renderBuckets[k].vertexAttributeLayout) {
						drawCmdIsUnique = false;
						renderBuckets[k].renderables.push_back(renderables[i]);
						break;
				}
			}

			//If none of the renderbuckets match, we create a new renderbucket and insert it.
			if(drawCmdIsUnique) {
				//Create a new renderbucket with the right state and vertex layout
				RenderBucket newBucket;
				newBucket.renderState = renderables[i]->totalRenderStateHandle;
				newBucket.vertexAttributeLayout = renderables[i]->vertexAttributeLayout;

				//Sort of a clunky way to figure out the number of VBOs we have for this particular render bucket, but it works and isn't super slow
				std::vector<VertexAttributeDescription>& vboVec = vertexAttributeLayouts[newBucket.vertexAttributeLayout];
				newBucket.numVBOs = vboVec.size();

				//Insert it into renderbuckets.... inefficient copy construct, but don't think there's any way around it.
				renderBuckets.push_back(newBucket);

				// Get furthest out renderbucket, aka the one we just inserted. Then into that renderbucket, 
				// insert the newest drawcommand that we tried to insert into the other renderbuckets (but failed because of no match)
				renderBuckets[renderBuckets.size()-1].renderables.push_back(renderables[i]);
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

		//We need to make sure that there is a base layout of render states set up.
		//InsertDefaultRenderstate(renderCommandBuffer);

		//Go through all render buckets and queue up state changes + render calls for each one.
		for(unsigned int i = 0; i < renderBuckets.size(); ++i) {

			//First of all we'll just insert the default states to get things going.
			compareRenderStates(previousState, renderBuckets[i], renderCommandBuffer);

			//Insert the actual render command
			if(usingBindless) {

				//Measure up all of the sizes
				unsigned int staticObjectSize = sizeof(DrawElementsIndirectCommand) + sizeof(GLuint) + sizeof(IndirectRenderData);
				unsigned int dynamicObjectSize = renderBuckets[i].numVBOs * sizeof(IndirectRenderData);
				unsigned int totalBlobSizeInBytes = renderBuckets[i].renderables.size() * (staticObjectSize + dynamicObjectSize);

				std::vector<IndirectRenderData> vboDrawCommands(renderBuckets[i].numVBOs);

				BlobContainer blob(0, totalBlobSizeInBytes);
				void* blobPtr = blob.getDataHeadPtr();
				unsigned int index = 0;

				//Now we step through the entire blob data and insert stuff with memcpy
				for(std::vector<Renderable*>::iterator renderableIter = renderBuckets[i].renderables.begin(); 
					renderableIter != renderBuckets[i].renderables.end(); ++renderableIter) {

					//Fill static part of drawCommand with data
					drawCommand.indexBuffer.index = 0;
					drawCommand.indexBuffer.address = (*renderableIter)->IBO.getBufferGPUPtr();
					drawCommand.indexBuffer.length = (*renderableIter)->IBO.getBufferSize();
					
					//And insert into blob
					memcpy(((char*)blobPtr+index), &drawCommand, staticObjectSize);
					index += staticObjectSize;

					//Now for the tricky part, let's do the dynamic data
					for(unsigned int k = 0; k < vboDrawCommands.size(); ++k) {
						vboDrawCommands[k].address = (*renderableIter)->VBOs[k].getBufferGPUPointer();
						vboDrawCommands[k].length = (*renderableIter)->VBOs[k].getBufferSize();
						vboDrawCommands[k].index = (*renderableIter)->VBOs[k].getVAD().attributeIndex; //I think this is right.
					}

					//And insert into the blob
					memcpy(((char*)blobPtr+index), &vboDrawCommands[0], dynamicObjectSize);

					index += dynamicObjectSize;
				}

				//Now we're done. The blob should have been filled with all of the data in the correct order.

				renderCommandBuffer.InsertNewCommand(new UpdateCircularBufferCommand(&transformBuffer, &transforms[0], transforms.size()*sizeof(Matrix4f)));

				renderCommandBuffer.InsertNewCommand(new MultiDrawElementsIndirectCommand(renderCommandBuffer.GetBuffer(),
					vertexAttributeLayouts[renderBuckets[i].vertexAttributeLayout], blob, GL_TRIANGLES, GL_UNSIGNED_INT, 
					0, renderBuckets[i].renderables.size(), renderBuckets[i].renderables[0]->VBOs.size()));

				renderCommandBuffer.InsertNewCommand(new CleanupCircularBufferCommand(&transformBuffer, transforms.size()*sizeof(Matrix4f)));

			} else {
				renderCommandBuffer.InsertNewCommand(new DrawElementsBaseVertexCommand(renderBuckets[i].renderables, GL_TRIANGLES, GL_UNSIGNED_INT, 0, worldMatrixUniformLocation));
			}

			//And insert cleanup command afterwards
			if(!attributeSlotsToBeFreed.empty()) {
				renderCommandBuffer.InsertNewCommand(new CleanupAttributeSlotsCommand(attributeSlotsToBeFreed));
			}
		}

		//If we have enabled any client states, we want to make sure that we also disable them at the end of the frame.
		if(clientStatesToEnable.size() > 0) {
			for(unsigned int i = 0; i < clientStatesToEnable.size(); ++i) {
				clientStatesToEnable[i].second = false;
			}
			renderCommandBuffer.InsertNewCommand(new ChangeClientStateCommand(clientStatesToEnable));
		}

		//Important, set flag so that we don't rebuild next update unless it's changed externally
		rebuildCommandBuffer = false;
	}
}

void  H3D::Renderer::render() {
	/*
	Set up necessary basic settings and do a clear
	*/
	glViewport(0, 0, static_cast<GLsizei>(viewportWidth), static_cast<GLsizei>(viewportHeight));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Clear to a beautiful, relaxing purple color...?
	glClearColor(0.4f, 0.35f, 0.65f, 1.0f);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	Fill up our persistent buffer matrix
	*/
	//if(usingBindless) {
	//	Matrix4f* dst = transformBuffer.Reserve(transforms.size());
	//	memcpy(dst, &*transforms.begin(), sizeof(Matrix4f) * transforms.size());

	//	transformBuffer.BindBufferRange(0, transforms.size());
	//}

	renderCommandBuffer.Render();

	//if(usingBindless) {
	//	transformBuffer.OnUsageComplete(transforms.size());
	//}

	//Clear these out at the end of each frame...
	renderables.clear();
	transforms.clear();
}

void H3D::Renderer::insertNewRenderable(Renderable* renderData) {
	renderables.push_back(renderData);
}

/*
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
			if(rp != NULL) {
				if(depthState != totalRenderStates[i].getDepthState()) {
					isUnique = true;
					break;
				}

				if(blendState != totalRenderStates[i].getBlendState()) {
					isUnique = true;
					break;
				}
			}

			if(rasterizerState != totalRenderStates[i].getRasterizerState()) {
				isUnique = true;
				break;
			}
		}

		if(isUnique) {
			TotalRenderState newState;
			newState.setBlendState(blendState);
			newState.setDepthState(depthState);
			newState.setRasterizerState(rasterizerState);

			totalRenderStates.push_back(newState);

			//Get furthest out index because that's where the newly inserted state is.
			retVal = (totalRenderStates.size()-1);
		}
	}

	//If renderproperties were standard we just return 0, because 
	//totalRenderStates[0] contains a totally standard renderstate
	return retVal;
}
*/

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

	/************************************************************************/
	/* 
	* Här ska TotalRenderState::InsertRenderCommands användas ist för det nedanför. TODO.
	*/ 
	/************************************************************************/

	TotalRenderState& rs = totalRenderStates[renderBucket.renderState];
	rs.getShaderData().uniforms[0].ptr = viewProjection[0];
	rs.getShaderData().uniforms[1].ptr = &elapsedTime;

	rs.InsertRenderCommands(&commandBuffer, &previousRenderState);

	if(previousRenderState.getVertexLayoutDescription() != rs.getVertexLayoutDescription()) {

		std::vector<VertexAttributeDescription>& layout = vertexAttributeLayouts[rs.getVertexLayoutDescription()];

		//This code is just so that I can keep track of what vertex attribute slots will be bound / unbound between shader changes, so that I know which to clean up and which to keep open.
		if(previousRenderState.getVertexLayoutDescription() == 0) {
			attributeSlotsToBeFreed.clear();
			for(unsigned int i = 0; i < layout.size(); ++i) {
				attributeSlotsToBeFreed.push_back(i);
			}
		} else {
			std::vector<VertexAttributeDescription>& previousLayout = vertexAttributeLayouts[previousRenderState.getVertexLayoutDescription()];

			//If the previous layout left extra attribute slots enabled, we want to queue those slots up for disabling
			if(previousLayout.size() > layout.size()) {
				for(unsigned int i = layout.size()-1; i < previousLayout.size(); ++i) {
					attributeSlotsToBeFreed.push_back(i);
				}
			}
		}

		//, false, vertexAttributeLayouts[rs.vertexLayoutDescription].second));
		//commandBuffer.InsertNewCommand(new ChangeVertexAttributeSetupCommand(layout, usingBindless));
		previousRenderState.setVertexLayoutDescription(rs.getVertexLayoutDescription()); //Copy value to make sure comparison for next CompareRenderStates call will work as intended.
	}

	//Last of all, we overwrite previousRenderState for next time we want to do a comparison
	previousRenderState = rs;
}


void H3D::Renderer::InsertDefaultRenderstate(RenderCommandBuffer& commandBuffer) {
	TotalRenderState& rs = totalRenderStates[0];

	commandBuffer.InsertNewCommand(new ChangeBlendStateCommand(rs.getBlendState()));
	commandBuffer.InsertNewCommand(new ChangeDepthStateCommand(rs.getDepthState()));
	commandBuffer.InsertNewCommand(new ChangeRasterizerStateCommand(rs.getRasterizerState()));

	std::vector<VertexAttributeDescription>& layout = vertexAttributeLayouts[rs.getVertexLayoutDescription()];

	attributeSlotsToBeFreed.clear();
	for(unsigned int i = 0; i < layout.size(); ++i) {
		attributeSlotsToBeFreed.push_back(i);
	}
}


unsigned int H3D::Renderer::insertVertexAttributeLayout(std::vector<VertexAttributeDescription> layout) {

	// Normal for-loop instead of iterator because I need the index...
	// Loop through all our layouts and see if any of them match the one we just sent in
	for(unsigned int i = 0; i < vertexAttributeLayouts.size(); ++i) {
		// First do size comparison to break early
		if(vertexAttributeLayouts[i].size() == layout.size()) {
			std::vector<VertexAttributeDescription>& vec = vertexAttributeLayouts[i];
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
	vertexAttributeLayouts.push_back(layout);

	// Return the furthest out index, because that's 
	// where the newest layout is stored.
	return (vertexAttributeLayouts.size()-1);
}

void H3D::Renderer::setViewportSize(float width, float height)
{
	viewportWidth = width;
	viewportHeight = height;

	projection = MathUtil::CreatePerspectiveProjection(45.0f, (viewportWidth/viewportHeight), 0.001f, 100.0f);
}

void H3D::Renderer::updateViewMatrix()
{
	view = MathUtil::CreateLookAt(eyeVector, lookAtVector, upVector);
	viewProjection = (projection * view);
}

void H3D::Renderer::setCurrentViewpoint(Vec3f position, Vec3f direction, Vec3f up)
{
	lookAtVector = direction;
	upVector = up;
	eyeVector = position;

	updateViewMatrix();
}