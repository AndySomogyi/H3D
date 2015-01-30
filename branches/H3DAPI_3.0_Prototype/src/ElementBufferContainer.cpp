#include <H3D/ElementBufferContainer.h>

H3D::ElementBufferContainer::ElementBufferContainer(bool usePersistent)
{
	if(usePersistent && (GLEW_EXT_direct_state_access && GL_NV_vertex_buffer_unified_memory)) {
		use_persistent = true;
	} else {
		use_persistent = false;
	}

	refCount = 1;

	buffer_id = 0;
	buffer_gpu_ptr = 0;
	buffer_size = 0;
	primitive_type = 0;

	//Will this ever change...?
	buffer_target = GL_ELEMENT_ARRAY_BUFFER;
}

H3D::ElementBufferContainer::ElementBufferContainer(const ElementBufferContainer& rhs)
{
	buffer_gpu_ptr = rhs.buffer_gpu_ptr;
	buffer_target = rhs.buffer_target;
	primitive_type = rhs.primitive_type;
	buffer_id = rhs.buffer_id;
	index_count = rhs.index_count;
	buffer_size = rhs.buffer_size;
	use_persistent = rhs.use_persistent;
	
	//Need to be able to change ref count.
	const_cast<ElementBufferContainer&>(rhs).incrementRefCount();
	refCount = rhs.refCount;
}

H3D::ElementBufferContainer& H3D::ElementBufferContainer::operator=(const ElementBufferContainer& rhs)
{
	buffer_gpu_ptr = rhs.buffer_gpu_ptr;
	buffer_target = rhs.buffer_target;
	primitive_type = rhs.primitive_type;
	buffer_id = rhs.buffer_id;
	index_count = rhs.index_count;
	buffer_size = rhs.buffer_size;
	use_persistent = rhs.use_persistent;

	//Need to be able to change ref count.
	const_cast<ElementBufferContainer&>(rhs).incrementRefCount();
	refCount = rhs.refCount;

	return *this;
}


H3D::ElementBufferContainer::~ElementBufferContainer()
{
	if((--refCount) <= 0) {
		releaseBuffer();
	}
}

void H3D::ElementBufferContainer::releaseBuffer()
{
	if(buffer_id != 0) {
		if(use_persistent) {
			glBindBuffer(buffer_target, buffer_id);
			glUnmapBuffer(buffer_target);
			glDeleteBuffers(1, &buffer_id);
			buffer_id = 0;
			buffer_gpu_ptr = 0;
		} else {
			glDeleteBuffers(1, &buffer_id);
			buffer_id = 0;
			buffer_gpu_ptr = 0;
		}
	}
}

void H3D::ElementBufferContainer::buildBufferObject(const void* elementArray, unsigned int arraySize, unsigned int indexCount, GLenum primitiveType)
{
	//First clean up any old data...
	releaseBuffer();

	//Save some stuff
	primitive_type = primitiveType;
	buffer_size = arraySize;
	index_count = indexCount;

	//Get OGL to return a free buffer ID
	if(!buffer_id) {
		glGenBuffers(1, &buffer_id);
	}

	glBindBuffer(buffer_target, buffer_id);

	if(use_persistent) {
		glBufferStorage(buffer_target, buffer_size, elementArray, 0);
		glGetBufferParameterui64vNV(buffer_target, GL_BUFFER_GPU_ADDRESS_NV, &buffer_gpu_ptr);
		glMakeBufferResidentNV(buffer_target, GL_READ_ONLY);
	} else {
		glBufferData(buffer_target, buffer_size, elementArray, GL_STATIC_DRAW);
	}
}



GLuint H3D::ElementBufferContainer::getIndexCount()
{
	return index_count;
}

GLenum H3D::ElementBufferContainer::getPrimitiveType()
{
	return primitive_type;
}

GLsizei H3D::ElementBufferContainer::getBufferSize()
{
	return buffer_size;
}

GLuint64 H3D::ElementBufferContainer::getBufferGPUPtr()
{
	return buffer_gpu_ptr;
}

GLuint H3D::ElementBufferContainer::getBufferID()
{
	return buffer_id;
}