#include <H3D/RenderCommandBuffer.h>
#include <H3D/H3DApi.h>

using namespace H3D;

H3D::RenderCommandBuffer::RenderCommandBuffer(unsigned int numCommands)
: commandQueue(), commandBuffer(USING_BINDLESS)
{
	const GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
	const GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

	//Command buffer needs to be accessible from inside the DrawCommands struct. Also idk if need several, might be dumb.
	commandBuffer.Create(BufferStorage::PersistentlyMappedBuffer, GL_DRAW_INDIRECT_BUFFER, numCommands, createFlags, mapFlags);

	commandQueue.reserve(numCommands);
}

H3D::RenderCommandBuffer::~RenderCommandBuffer()
{
	Clear();
}

void H3D::RenderCommandBuffer::Clear()
{
	for(std::vector<RenderCommand*>::const_iterator it = commandQueue.cbegin(); it != commandQueue.cend(); ++it)
	{
		delete *it;
	}

	commandQueue.clear();

	commandBuffer.Destroy();
}

void H3D::RenderCommandBuffer::InsertNewCommand(RenderCommand* command)
{
	commandQueue.push_back(command);
}

void H3D::RenderCommandBuffer::Render()
{
	for(unsigned int i = 0; i < commandQueue.size(); ++i)
	{
		commandQueue[i]->execute();
	}
}