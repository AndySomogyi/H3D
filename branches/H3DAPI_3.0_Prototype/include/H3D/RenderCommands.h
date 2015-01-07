#ifndef __RENDERCOMMANDS_H__
#define __RENDERCOMMANDS_H__

#include <H3D/OGLRenderStates.h>
#include <H3D/RenderMetaData.h>
#include <H3D/OGLBuffer.h>

namespace H3D {
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Base for all RenderCommands
	class RenderCommand {
	public:
		virtual ~RenderCommand();
		virtual void execute() = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Command for multidraw elements indirect
	class MultiDrawElementsIndirectCommand : public RenderCommand {
	public:
		MultiDrawElementsIndirectCommand(CircularBuffer<DrawCommand>& _commandBuffer, std::vector<DrawCommand>& _drawcommands, 
			GLenum _mode, GLenum _type, GLsizei _stride, int _vboCount);

		virtual void execute();

	private:
		GLenum mode;
		GLenum type;
		GLsizei stride;

		/* TODO: ... A blob instead of a std::vector<DrawCommand> */
		std::vector<DrawCommand>&  drawcommands;
		CircularBuffer<DrawCommand>& commandBuffer;
		int vboCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Command for normal "non-bindless" rendering
	class DrawElementsBaseVertexCommand : public RenderCommand {
	public:
		DrawElementsBaseVertexCommand(std::vector<ElementDrawData>& _drawcommands, 
			GLenum _mode, GLenum _type, GLsizei _stride, GLuint transformUniformIndex);

		virtual void execute();

	private:
		GLuint transformUniformIndex;
		GLenum mode;
		GLenum type;
		GLsizei stride;
		std::vector<ElementDrawData> objects;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class ChangeRasterizerStateCommand : public RenderCommand {
	public:
		ChangeRasterizerStateCommand(RasterizerState _rasterizerState);

		virtual void execute();

	private:
		RasterizerState rasterizerState;
	};

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

	class ChangeVertexAttributeSetupCommand : public RenderCommand {
	public:
		ChangeVertexAttributeSetupCommand(std::vector<VertexAttributeDescription> _layout, bool _bindless = false);

		virtual void execute();

	private:
		//I'm assuming these were put in the right order.
		std::vector<VertexAttributeDescription> layout;
		bool bindless;
	};

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
		// of the way openGL handles "binary" settings; you either need to call glEnableClientState or glDisableClientState. Hence we need a bool to know which one.
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

	template<class T>
	class UpdateCircularBufferCommand : public RenderCommand {
	public:
		UpdateCircularBufferCommand(CircularBuffer<T>* _buffer, std::vector<T>* _objects);

		virtual void execute();

	private:
		CircularBuffer<T>* buffer;
		std::vector<T>* objects;
	};

	//////////////////////////////////////////////////////////////////////////

	template<class T>
	class CleanupCircularBufferCommand : public RenderCommand {
	public:
		CleanupCircularBufferCommand(CircularBuffer<T>* _buffer, unsigned int _size);

		virtual void execute();

	private:
		CircularBuffer<T>* buffer;
		unsigned int size;
	};
	
	//////////////////////////////////////////////////////////////////////////
}
#endif