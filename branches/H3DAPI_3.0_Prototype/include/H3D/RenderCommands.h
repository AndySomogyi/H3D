#ifndef __RENDERCOMMANDS_H__
#define __RENDERCOMMANDS_H__

#include <H3D/TotalRenderState.h>
#include <H3D/RenderMetaData.h>
#include <H3D/OGLBuffer.h>
#include <H3D/BlobContainer.h> //For MultiDrawElementsIndirectCommand

namespace H3D {
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Base for all RenderCommands
	class RenderCommand {
	public:
		virtual ~RenderCommand();
		virtual void execute() = 0;
	};


	/************************************************************************/
	/*							Draw commands                               */
	/************************************************************************/

	//Command for multidraw elements indirect
	class MultiDrawElementsIndirectCommand : public RenderCommand {
	public:
		MultiDrawElementsIndirectCommand(CircularBuffer& _commandBuffer, std::vector<VertexAttributeDescription> _VAD, BlobContainer _blob, 
			GLenum _mode, GLenum _type, GLsizei _stride, unsigned int _drawCount, unsigned int _vboCount);

		virtual void execute();

	private:
		GLenum mode;
		GLenum type;
		GLsizei stride;

		/* TODO: ... A blob instead of a std::vector<DrawCommand> */
		BlobContainer blob;
		std::vector<VertexAttributeDescription> VAD;
		CircularBuffer& commandBuffer;
		unsigned int vboCount;
		unsigned int drawCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Command for normal "non-bindless" rendering
	class DrawElementsBaseVertexCommand : public RenderCommand {
	public:
		DrawElementsBaseVertexCommand(std::vector<Renderable*> _drawcommands, 
			GLenum _mode, GLenum _type, GLsizei _stride, GLuint transformUniformIndex);

		virtual void execute();

	private:
		GLuint transformUniformIndex;
		GLenum mode;
		GLenum type;
		GLsizei stride;
		std::vector<Renderable*> objects;
	};

	/************************************************************************/
	/*				Generic state change command test                       */
	/************************************************************************/

	class GenericStateChangeCommand : public RenderCommand {
	public:
		GenericStateChangeCommand(auto_ptr<StateChangeValue> _stateValue);

		virtual void execute();

	private:
		auto_ptr<StateChangeValue> stateValue;
	};

	/************************************************************************/
	/*                      Rasterizer state commands		                */
	/************************************************************************/

	class ChangeRasterizerStateCommand : public RenderCommand {
	public:
		ChangeRasterizerStateCommand(RasterizerState _rasterizerState);

		virtual void execute();

	private:
		RasterizerState rasterizerState;
	};

	/*
	class ChangeFaceCullingStateCommand : public RenderCommand {
	public:
		ChangeFaceCullingStateCommand(bool val);

		virtual void execute();

	private:
		bool enable;
	};

	class ChangeScissorTestStateCommand : public RenderCommand {
	public:
		ChangeScissorTestStateCommand(bool val);

		virtual void execute();

	private:
		bool enable;
	};

	class ChangeCullFaceCommand : public RenderCommand {
	public:
		ChangeCullFaceCommand(GLenum cullFace);

		virtual void execute();

	private:
		GLenum face;
	};
	*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ChangeBlendStateCommand : public RenderCommand {
	public:
		ChangeBlendStateCommand(BlendState _blendState);

		virtual void execute();

	private:
		BlendState blendState;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ChangeDepthStateCommand : public RenderCommand {
	public:
		ChangeDepthStateCommand(DepthState _depthState);

		virtual void execute();

	private:
		DepthState depthState;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//class ChangeVertexAttributeSetupCommand : public RenderCommand {
	//public:
	//	ChangeVertexAttributeSetupCommand(std::vector<VertexAttributeDescription> _layout, bool _bindless = false);
	//
	//	virtual void execute();
	//
	//private:
	//	//I'm assuming these were put in the right order.
	//	std::vector<VertexAttributeDescription> layout;
	//	bool bindless;
	//};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ChangeClientStateCommand : public RenderCommand {
	public:

		//************************************
		// Method:    ChangeClientStateCommand
		// FullName:  H3D::ChangeClientStateCommand::ChangeClientStateCommand
		// Access:    public 
		// Returns:   
		// Qualifier:
		// Parameter: std::vector<std::pair<GLenum, bool>> targetsAndValues. Okay, so the pairs are specified in this format because
		// of the way openGL handles "binary" settings; you either need to call // glEnableClientState or glDisableClientState. Hence we need a bool to know which one.
		//************************************
		ChangeClientStateCommand(std::vector<std::pair<GLenum, bool>> _targetsAndValues);

		virtual void execute();

	private:
		std::vector<std::pair<GLenum, bool>> targetsAndValues;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ChangeShaderCommand : public RenderCommand {
	public:
		ChangeShaderCommand(GLuint _programID);

		virtual void execute();

	private:
		GLuint programID;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class CleanupAttributeSlotsCommand : public RenderCommand {
	public:
		CleanupAttributeSlotsCommand(std::vector<GLuint> _slots);

		virtual void execute();

	private:
		std::vector<GLuint> slots;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class BindUniformCommand : public RenderCommand {
	public:
		BindUniformCommand(UniformDescription _description);

		virtual void execute();

	private:
		UniformDescription description;
	};

	//////////////////////////////////////////////////////////////////////////

	class UpdateCircularBufferCommand : public RenderCommand {
	public:
		UpdateCircularBufferCommand(CircularBuffer* _buffer, void* _objectArray, unsigned int _sizeInBytes)
		{
			buffer = _buffer;
			objectArray = _objectArray;
			sizeInBytes = _sizeInBytes;
		}

		virtual void execute()
		{
			char* dst = buffer->Reserve(sizeInBytes);
			memcpy(dst, objectArray, sizeInBytes);
			buffer->BindBufferRange(0, sizeInBytes); //BindBufferBase instead??
		}

	private:
		CircularBuffer* buffer;
		void* objectArray;
		unsigned int sizeInBytes;
	};

	//////////////////////////////////////////////////////////////////////////

	class CleanupCircularBufferCommand : public RenderCommand {
	public:
		CleanupCircularBufferCommand(CircularBuffer* _buffer, unsigned int _sizeInBytes);

		virtual void execute();

	private:
		CircularBuffer* buffer;
		unsigned int sizeInBytes;
	};
	
	//////////////////////////////////////////////////////////////////////////
}
#endif