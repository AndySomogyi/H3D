#include <H3D/RenderCommands.h>
#include <H3D/OGLUtil.h>

//////////////////////////////////////////////////////////////////////////

H3D::RenderCommand::~RenderCommand() {

}

//////////////////////////////////////////////////////////////////////////

H3D::MultiDrawElementsIndirectCommand::MultiDrawElementsIndirectCommand(CircularBuffer& _commandBuffer, std::vector<VertexAttributeDescription> _VAD, BlobContainer _blob, 
	GLenum _mode, GLenum _type, GLsizei _stride, unsigned int _drawCount, unsigned int _vboCount) 
	: commandBuffer(_commandBuffer), VAD(_VAD), blob(_blob), mode(_mode), type(_type), stride(_stride), drawCount(_drawCount), vboCount(_vboCount) {
}

void H3D::MultiDrawElementsIndirectCommand::execute() {
	/*
	TODO: Instead of a persistent command buffer, I can just use a std::vector<DrawCommand> ...?
	*/

	//glVertexAttribFormatNV(layout[i].attributeIndex, layout[i].attribute_size, layout[i].primitive_type, layout[i].normalized, layout[i].stride);

	static unsigned long counter = 0;
	++counter;

	//Set up all the vertex attribute formats...
	for(unsigned int i = 0; i < VAD.size(); ++i) {
		glVertexAttribFormatNV(VAD[i].attributeIndex, VAD[i].size, VAD[i].type, VAD[i].normalized, VAD[i].stride);
	}

	unsigned int dataSizeInBytes = blob.getSize();

	//Reserve memory in the persistently mapped command buffer
	GLvoid* commandPtr = commandBuffer.Reserve(dataSizeInBytes);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//Fill the reserved memory
	memcpy(commandPtr, blob.getDataHeadPtr(), dataSizeInBytes);

	glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//I can access the VBO for drawcommands[0], because my layout sorting algorithm guarantees that
	//All of the drawcommands in the same drawcommand bucket have the same amount of VBOs
	glMultiDrawElementsIndirectBindlessNV(mode, type, (GLvoid*)0, drawCount, 0, vboCount);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//Afterwards we let the buffer do internal stuff to clean up / handle memory locks
	commandBuffer.OnUsageComplete(dataSizeInBytes);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}
}

//////////////////////////////////////////////////////////////////////////

H3D::DrawElementsBaseVertexCommand::DrawElementsBaseVertexCommand(std::vector<Renderable*> _drawcommands, GLenum _mode, GLenum _type, GLsizei _stride, GLuint _transformUniformIndex)
	: objects(_drawcommands), mode(_mode), type(_type), stride(_stride), transformUniformIndex(_transformUniformIndex) {

}

void H3D::DrawElementsBaseVertexCommand::execute() {

	//Iterate over all objects in bucket
	for(std::vector<Renderable*>::iterator it = objects.begin(); it != objects.end(); ++it) {
		static unsigned int count = 0;
		count++;

		//There's only one index buffer, so bind that now
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->IBO.getBufferID());

		//Now iterate over each VBO for each object
		for(std::vector<VertexBufferContainer>::iterator vboIt = (*it)->VBOs.begin(); vboIt < (*it)->VBOs.end(); ++vboIt) {

			unsigned int index = 0;

			//Necessary? Should be.
			glBindBuffer(GL_ARRAY_BUFFER, vboIt->getBufferID());
			LogOGLErrors("DrawElementsBaseVertexCommand", count);

			const std::vector<VertexAttributeDescription> VADs = (*vboIt).getVADContainer();

			for(unsigned int i = 0; i < VADs.size(); ++i) {

				++index;

				//Do we actually want vad.attributeIndex here or is uint i just fine?
				glVertexAttribPointer(index, VADs[i].size, VADs[i].type, VADs[i].normalized, VADs[i].stride, (GLvoid*)VADs[i].startOffset);
				LogOGLErrors("DrawElementsBaseVertexCommand", count);

				glEnableVertexAttribArray(index);
			}
		}

		glUniformMatrix4fv(transformUniformIndex, 1, GL_TRUE, (*it)->transform[0]);

		LogOGLErrors("DrawElementsBaseVertexCommand", count);

		// Draw the triangles
		glDrawElements(
			(*it)->renderMode,								// mode
			(*it)->IBO.getIndexCount(),			// count
			(*it)->IBO.getPrimitiveType(),   // type
			nullptr);												// element array buffer offset

		LogOGLErrors("DrawElementsBaseVertexCommand", count);
	}
}

/************************************************************************/
/*				Generic state change command test															*/
/************************************************************************/

H3D::GenericStateChangeCommand::GenericStateChangeCommand(auto_ptr<StateChangeValue> _stateValue)
	: stateValue(_stateValue)
{
}

void H3D::GenericStateChangeCommand::execute()
{
	//Act accordingly...
	switch(stateValue->type)
	{
		//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::DepthTestingEnable: {
		if(static_cast<BoolStateValue*>(stateValue.get())->value) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		} }
																						 break;

																						 //////////////////////////////////////////////////////////////////////////
	case TotalRenderState::DepthBufferWriteEnable: {
		if(static_cast<BoolStateValue*>(stateValue.get())->value) {
			glDepthMask( GL_TRUE );
		} else {
			glDepthMask( GL_FALSE );
		} }
																								 break;

																								 //////////////////////////////////////////////////////////////////////////
	case TotalRenderState::DepthFunctionChange: {
		glDepthFunc(static_cast<UintStateValue*>(stateValue.get())->value);
		break;

		//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::BlendingEnabled:
		if(static_cast<BoolStateValue*>(stateValue.get())->value) {
			glEnable( GL_BLEND );
		} else {
			glDisable( GL_BLEND );
		} }
																							break;

																							//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::BlendFunctionChange: {
		BlendFunctionStateBundle* blendFuncPtr = static_cast<BlendFunctionStateBundle*>(stateValue.get());

		glBlendFuncSeparate(blendFuncPtr->srcRGB, blendFuncPtr->dstRGB, 
			blendFuncPtr->srcAlpha, blendFuncPtr->dstAlpha); }
																							break;

																							//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::BlendEquationChange: {
		BlendEquationStateBundle* ptr = static_cast<BlendEquationStateBundle*>(stateValue.get());
		glBlendEquationSeparate(ptr->rgbEquation, ptr->alphaEquation); }
																							break;

																							//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::AlphaFunctionChange: {
		AlphaFuncStateBundle* alphaFuncPtr = static_cast<AlphaFuncStateBundle*>(stateValue.get());
		glAlphaFunc(alphaFuncPtr->alphaTestFunc, alphaFuncPtr->testValue); }
																							break;

																							//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::ColorMaskChange: {
		ColorMaskStateValue* colorMaskPtr = static_cast<ColorMaskStateValue*>(stateValue.get());
		glColorMask(colorMaskPtr->value.mask[0],
			colorMaskPtr->value.mask[1],
			colorMaskPtr->value.mask[2],
			colorMaskPtr->value.mask[3]); }
																					break;

																					//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::BlendColorChange: {
		BlendColorStateValue* blendColorPtr = static_cast<BlendColorStateValue*>(stateValue.get());
		glBlendColor(blendColorPtr->value.color[0], blendColorPtr->value.color[1], 
			blendColorPtr->value.color[2], blendColorPtr->value.color[3]); }
																					 break;

																					 //////////////////////////////////////////////////////////////////////////
	case TotalRenderState::FaceCullingEnable: {
		if(static_cast<BoolStateValue*>(stateValue.get())->value){
			glEnable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
		} }
																						break;

																						//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::CullFaceChanged: {
		glCullFace(static_cast<UintStateValue*>(stateValue.get())->value); }
																					break;

																					//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::WindingOrderChange: {
		glFrontFace(static_cast<UintStateValue*>(stateValue.get())->value); }
																						 break;

																						 //////////////////////////////////////////////////////////////////////////
	case TotalRenderState::ScissorTestEnable: {
		if(static_cast<BoolStateValue*>(stateValue.get())->value) {
			glEnable(GL_SCISSOR_TEST);
		} else {
			glDisable(GL_SCISSOR_TEST);
		} }
																						break;

																						//////////////////////////////////////////////////////////////////////////
	case TotalRenderState::ShaderChange: {
		glUseProgram(static_cast<UintStateValue*>(stateValue.get())->value);
		break; }
	}

	//////////////////////////////////////////////////////////////////////////

	LogOGLErrors("StateChangeCommand");
}


//////////////////////////////////////////////////////////////////////////

H3D::ChangeRasterizerStateCommand::ChangeRasterizerStateCommand(RasterizerState _rasterizerState) 
	: rasterizerState(_rasterizerState) {

}

void H3D::ChangeRasterizerStateCommand::execute() {
	if(rasterizerState.faceCullingEnabled) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}

	glCullFace(rasterizerState.cullFace);
	glFrontFace(rasterizerState.windingOrder);

	if(rasterizerState.scissorTestEnabled) {
		glEnable(GL_SCISSOR_TEST);
	} else {
		glDisable(GL_SCISSOR_TEST);
	}

	LogOGLErrors("ChangeRasterizerStateCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::ChangeBlendStateCommand::ChangeBlendStateCommand(BlendState _blendState) : RenderCommand(), blendState(_blendState) {

}

void H3D::ChangeBlendStateCommand::execute()
{
	glColorMask(	blendState.colorMask.mask[0],
		blendState.colorMask.mask[1],
		blendState.colorMask.mask[2],
		blendState.colorMask.mask[3]);

	LogOGLErrors("ChangeBlendStateCommand");

	if( blendState.blendingEnabled) {
		glEnable( GL_BLEND );
	} else {
		glDisable( GL_BLEND );
	}

	LogOGLErrors("ChangeBlendStateCommand");

	glBlendFuncSeparate(	blendState.srcFactorRGB, blendState.dstFactorRGB, 
		blendState.srcFactorAlpha, blendState.dstFactorAlpha);

	LogOGLErrors("ChangeBlendStateCommand");

	glBlendEquationSeparate(blendState.blendEquationRGB, blendState.blendEquationAlpha);

	LogOGLErrors("ChangeBlendStateCommand");

	glBlendColor(	blendState.blendColor.color[0], 
		blendState.blendColor.color[1], 
		blendState.blendColor.color[2], 
		blendState.blendColor.color[3]);

	LogOGLErrors("ChangeBlendStateCommand");

	if(blendState.alphaTestFunc != GL_ALWAYS) {
		glEnable( GL_ALPHA_TEST );
		glAlphaFunc( blendState.alphaTestFunc, blendState.alphaFuncValue);
	}

	LogOGLErrors("ChangeBlendStateCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::ChangeDepthStateCommand::ChangeDepthStateCommand(DepthState _depthState) : RenderCommand(), depthState(_depthState) {

}

void H3D::ChangeDepthStateCommand::execute()
{
	if( depthState.depthTestingEnabled) {
		glEnable( GL_DEPTH_TEST );
	} else {
		glDisable( GL_DEPTH_TEST );
	}

	glDepthFunc(depthState.depthFunction);

	if( depthState.depthBufferWriteEnabled) {
		glDepthMask( GL_TRUE );
	} else {
		glDepthMask( GL_FALSE );
	}

	LogOGLErrors("ChangeDepthStateCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::ChangeClientStateCommand::ChangeClientStateCommand(std::vector<std::pair<GLenum, bool>> _targetsAndValues) 
	: targetsAndValues(_targetsAndValues) {

}

void H3D::ChangeClientStateCommand::execute() {

	for(unsigned int i = 0; i < targetsAndValues.size(); ++i) {

		//If the bool is set to true, it means we want to ENABLE something
		if(targetsAndValues[i].second) {
			glEnableClientState(targetsAndValues[i].first);
			LogOGLErrors("ChangeShaderCommand");
		} else //Else we want to DISABLE something
		{
			glDisableClientState(targetsAndValues[i].first);
			LogOGLErrors("ChangeShaderCommand");
		}
	}

	LogOGLErrors("ChangeClientStateCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::ChangeShaderCommand::ChangeShaderCommand(GLuint _programID) 
	: programID(_programID) {

}

void H3D::ChangeShaderCommand::execute() {
	glUseProgram(programID);
	LogOGLErrors("ChangeShaderCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::CleanupAttributeSlotsCommand::CleanupAttributeSlotsCommand(std::vector<GLuint> _slots) 
	: slots(_slots) {

}

void H3D::CleanupAttributeSlotsCommand::execute() {
	for(unsigned int i = 0; i < slots.size(); ++i) {
		glDisableVertexAttribArray(slots[i]);
	}

	LogOGLErrors("CleanupAttributeSlotsCommand");
}

//////////////////////////////////////////////////////////////////////////

H3D::BindUniformCommand::BindUniformCommand(UniformDescription _description) 
	: description(_description) {

}

void H3D::BindUniformCommand::execute() {

	switch(description.type) {

	case UniformDescription::f1:
		glUniform1f(description.location, *static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::i1:
		glUniform1i(description.location, *static_cast<GLint*>(description.ptr));
		break;

	case UniformDescription::fv1:
		glUniform1fv(description.location, description.count, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::fv2:
		glUniform2fv(description.location, description.count, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::fv3:
		glUniform3fv(description.location, description.count, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::fv4:
		glUniform4fv(description.location, description.count, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::iv1:
		glUniform1iv(description.location, description.count, static_cast<GLint*>(description.ptr));
		break;

	case UniformDescription::iv2:
		glUniform2iv(description.location, description.count, static_cast<GLint*>(description.ptr));
		break;

	case UniformDescription::iv3:
		glUniform3iv(description.location, description.count, static_cast<GLint*>(description.ptr));
		break;

	case UniformDescription::iv4:
		glUniform4iv(description.location, description.count, static_cast<GLint*>(description.ptr));
		break;

	case UniformDescription::fmat3x3:
		glUniformMatrix3fv(description.location, description.count, GL_TRUE, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::fmat4x4:
		glUniformMatrix4fv(description.location, description.count, GL_TRUE, static_cast<GLfloat*>(description.ptr));
		break;

	case UniformDescription::dmat3x3:
		glUniformMatrix3dv(description.location, description.count, GL_TRUE, static_cast<GLdouble*>(description.ptr));
		break;

	case UniformDescription::dmat4x4:
		glUniformMatrix4dv(description.location, description.count, GL_TRUE, static_cast<GLdouble*>(description.ptr));
		break;

	default:
		break;
	}

	LogOGLErrors("Error in BindUniformCommand: " + description.name);
}

//////////////////////////////////////////////////////////////////////////

H3D::CleanupCircularBufferCommand::CleanupCircularBufferCommand(CircularBuffer* _buffer, unsigned int _sizeInBytes)
{
	buffer = _buffer;
	sizeInBytes = _sizeInBytes;
}


void H3D::CleanupCircularBufferCommand::execute()
{
	buffer->OnUsageComplete(sizeInBytes);
}

//////////////////////////////////////////////////////////////////////////