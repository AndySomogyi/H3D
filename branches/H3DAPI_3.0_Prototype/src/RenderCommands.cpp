#include <H3D/RenderCommands.h>
#include <H3D/OGLUtil.h>

//////////////////////////////////////////////////////////////////////////

H3D::RenderCommand::~RenderCommand() {

}

//////////////////////////////////////////////////////////////////////////

void H3D::MultiDrawElementsIndirectCommand::execute() {

	/*
	TODO: Instead of a persistent command buffer, I can just use a std::vector<DrawCommand>
	*/

	static unsigned long counter = 0;
	++counter;

	//commandBuffer.BindBuffer();

	//Reserve memory in the persistently mapped command buffer
	GLvoid* commandPtr = commandBuffer.Reserve(drawcommands.size());

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//Fill the reserved memory
	memcpy(commandPtr, &*drawcommands.begin(), (sizeof(DrawCommand)*drawcommands.size()));

	/*
	In the future when we want to expand this to using more VBOs and dont want to be 
	limited by a static array size, we can do a few extra analyzing steps and split it up into several memCpys.
	We can easily find out how many VBOs there are per object, then just size the memcpy based on that.
	First memcpy the "static" data.
	Then memcpy the VBO vector or w/e. Does mean that we have to build this buffer blob on the spot...
	Or we do it earlier as a preprocessing step, that might work just as easily. Just need to set up a function for it.
	*/

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//I can access the VBO for drawcommands[0], because my layout sorting algorithm guarantees that
	//All of the drawcommands in the same drawcommand bucket have the same amount of VBOs
	glMultiDrawElementsIndirectBindlessNV(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, static_cast<GLsizei>(drawcommands.size()), 0, vboCount);

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}

	//Afterwards we let the buffer do internal stuff to clean up / handle memory locks
	commandBuffer.OnUsageComplete(drawcommands.size());

	if(LogOGLErrors("MultiDrawElementsIndirectCommand", counter)) {
		//If we've come here, something has gone wrong
	}
}

H3D::MultiDrawElementsIndirectCommand::MultiDrawElementsIndirectCommand(CircularBuffer<DrawCommand>& _commandBuffer, std::vector<DrawCommand>& _drawcommands, 
	GLenum _mode, GLenum _type, GLsizei _stride, int _vboCount) 
	: commandBuffer(_commandBuffer), drawcommands(_drawcommands), mode(_mode), type(_type), stride(_stride), vboCount(_vboCount) {
}

//////////////////////////////////////////////////////////////////////////

H3D::DrawElementsBaseVertexCommand::DrawElementsBaseVertexCommand(std::vector<ElementDrawData>& _drawcommands, GLenum _mode, GLenum _type, GLsizei _stride, GLuint _transformUniformIndex)
	: objects(_drawcommands), mode(_mode), type(_type), stride(_stride), transformUniformIndex(_transformUniformIndex) {

}

void H3D::DrawElementsBaseVertexCommand::execute() {
	for(std::vector<ElementDrawData>::iterator it = objects.begin(); it != objects.end(); ++it) {
		static unsigned int count = 0;
		count++;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->IBO);

		for(unsigned int i = 0; i < it->VBOs.size(); ++i) {
			const VertexAttributeDescription& VAD = it->VBOs[i];

			glBindBuffer(GL_ARRAY_BUFFER, VAD.bufferID);
			LogOGLErrors("DrawElementsBaseVertexCommand", count);

			//glEnableVertexAttribArray(it->VBOs[i].bufferIndex);
			//LogOGLErrors("DrawElementsBaseVertexCommand", count);

			glVertexAttribPointer(VAD.bufferIndex, VAD.elementCount, VAD.primitiveType, (VAD.normalized ? GL_TRUE : GL_FALSE), 0, 0);
			LogOGLErrors("DrawElementsBaseVertexCommand", count);
		}

		glUniformMatrix4fv(transformUniformIndex, 1, GL_TRUE, (*it->transform)[0]);

		LogOGLErrors("DrawElementsBaseVertexCommand", count);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			it->indexCount,    // count
			GL_UNSIGNED_INT,   // type
			nullptr);          // element array buffer offset

		LogOGLErrors("DrawElementsBaseVertexCommand", count);
	}
}

/************************************************************************/
/*				Generic state change command test	                    */
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

H3D::ChangeRasterizerStateCommand::ChangeRasterizerStateCommand(RasterizerState _rasterizerState) 
	: rasterizerState(_rasterizerState) {

}

//////////////////////////////////////////////////////////////////////////

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

H3D::ChangeBlendStateCommand::ChangeBlendStateCommand(BlendState _blendState) : RenderCommand(), blendState(_blendState)
{

}

//////////////////////////////////////////////////////////////////////////

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

H3D::ChangeDepthStateCommand::ChangeDepthStateCommand(DepthState _depthState) : RenderCommand(), depthState(_depthState)
{

}

//////////////////////////////////////////////////////////////////////////

H3D::ChangeVertexAttributeSetupCommand::ChangeVertexAttributeSetupCommand(std::vector<VertexAttributeDescription> _layout, bool _bindless)
	: bindless(_bindless) {
		layout.resize(_layout.size());

		//Temp.
		bindless = false;

		for(unsigned int i = 0; i < _layout.size(); ++i) {
			//Insert this specific attribute description in the right index
			layout[_layout[i].bufferIndex] = _layout[i];
		}
}

void H3D::ChangeVertexAttributeSetupCommand::execute() {
	if(bindless) {

		for(unsigned int i = 0; i < layout.size(); ++i) {
			//glVertexAttribFormat(layout[i].index, layout[i].size, layout[i].primitiveType, (layout[i].normalized ? GL_TRUE : GL_FALSE), layout[i].stride);
			glVertexAttribFormatNV(layout[i].bufferIndex, layout[i].attributeSize, layout[i].primitiveType, (layout[i].normalized ? GL_TRUE : GL_FALSE), layout[i].stride);
			//glVertexAttribPointer(layout[i].index, layout[i].size, layout[i].primitiveType, (layout[i].normalized ? GL_TRUE : GL_FALSE), layout[i].stride, (GLvoid*)0);
			glEnableVertexAttribArray(layout[i].bufferIndex);

			LogOGLErrors("ChangeVertexAttributeSetupCommand in bindless: "+i);
		}
	} else {
		/*
		gl.glBindBuffer(GL::GL_ARRAY_BUFFER, @vertexVBO[0])

		vertexLocation = @dummyShader.attribLocation(gl, "vertex")
		gl.glVertexAttribPointer(vertexLocation, 3, GL::GL_FLOAT, false, 0, 0)
		gl.glEnableVertexAttribArray(vertexLocation)

		gl.glBindBuffer(GL::GL_ARRAY_BUFFER, @normalVBO[0])	
		normalLocation = @dummyShader.attribLocation(gl, "normal")
		gl.glVertexAttribPointer(normalLocation, 3, GL::GL_FLOAT, false, 0, 0)
		gl.glEnableVertexAttribArray(normalLocation)
		*/

		for(unsigned int i = 0; i < layout.size(); ++i) {
			//glVertexAttribPointer(layout[i].index, layout[i].size, layout[i].primitiveType, layout[i].normalized, layout[i].stride, 0);
			glEnableVertexAttribArray(layout[i].bufferIndex);

			LogOGLErrors("ChangeVertexAttributeSetupCommand in non-bindless: "+i);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

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

H3D::ChangeClientStateCommand::ChangeClientStateCommand(std::vector<std::pair<GLenum, bool>> _targetsAndValues) 
	: targetsAndValues(_targetsAndValues) {

}

//////////////////////////////////////////////////////////////////////////

void H3D::ChangeShaderCommand::execute() {
	glUseProgram(programID);
	LogOGLErrors("ChangeShaderCommand");
}

H3D::ChangeShaderCommand::ChangeShaderCommand(GLuint _programID) 
	: programID(_programID) {

}

//////////////////////////////////////////////////////////////////////////

void H3D::CleanupAttributeSlotsCommand::execute() {
	for(unsigned int i = 0; i < slots.size(); ++i) {
		glDisableVertexAttribArray(slots[i]);
	}

	LogOGLErrors("CleanupAttributeSlotsCommand");
}

H3D::CleanupAttributeSlotsCommand::CleanupAttributeSlotsCommand(std::vector<GLuint> _slots) 
	: slots(_slots) {

}

//////////////////////////////////////////////////////////////////////////

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

H3D::BindUniformCommand::BindUniformCommand(UniformDescription _description) 
	: description(_description) {

}

//////////////////////////////////////////////////////////////////////////
