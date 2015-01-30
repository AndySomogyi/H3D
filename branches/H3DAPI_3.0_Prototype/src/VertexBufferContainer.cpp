#include <H3D/VertexBufferContainer.h>

using namespace H3D;

H3D::VertexBufferContainer::VertexBufferContainer(bool usingBindless /* = false */) :
refCount(1) {
	if(usingBindless && (GLEW_EXT_direct_state_access && GL_NV_vertex_buffer_unified_memory)) {
		use_persistent = true;
	} else {
		use_persistent = false;
	}

	buffer_usage = GL_VERTEX_ARRAY;
	buffer_gpu_ptr = 0;
	buffer_ID = 0;
	buffer_size = 0;
	buffer_target = 0;
	vertexAttributeLayout = 0;

	vadContainer.clear();
}

H3D::VertexBufferContainer::VertexBufferContainer(const VertexBufferContainer& rhs) {
	use_persistent = rhs.use_persistent;
	buffer_usage = rhs.buffer_usage;
	buffer_ID = rhs.buffer_ID;
	buffer_gpu_ptr = rhs.buffer_gpu_ptr;
	buffer_target = rhs.buffer_target;
	vadContainer = rhs.vadContainer;
	vertexAttributeLayout = rhs.vertexAttributeLayout;

	//Need to be able to change ref count.
	const_cast<VertexBufferContainer&>(rhs).incrementRefCount();
	refCount = rhs.refCount;
}

H3D::VertexBufferContainer& H3D::VertexBufferContainer::operator=(const VertexBufferContainer& rhs) {
	//First make sure we clean up any potential old stuff to avoid leaks
	if((--refCount) <= 0) {
		releaseBuffer();
	}

	use_persistent = rhs.use_persistent;
	buffer_usage = rhs.buffer_usage;
	buffer_gpu_ptr = rhs.buffer_gpu_ptr;
	buffer_ID = rhs.buffer_ID;
	buffer_target = rhs.buffer_target;
	vadContainer = rhs.vadContainer;
	vertexAttributeLayout = rhs.vertexAttributeLayout;

	//Need to be able to change ref count.
	const_cast<VertexBufferContainer&>(rhs).incrementRefCount();
	refCount = rhs.refCount;

	return (*this);
}

H3D::VertexBufferContainer::~VertexBufferContainer() {
	if((--refCount) <= 0) {
		releaseBuffer();
	}
}

void H3D::VertexBufferContainer::releaseBuffer() {
	if(buffer_ID != 0) {
		if(use_persistent) {
			glBindBuffer(buffer_target, buffer_ID);
			glUnmapBuffer(buffer_target);
			glDeleteBuffers(1, &buffer_ID);
		} else {
			glDeleteBuffers(1, &buffer_ID);
		}

		buffer_ID = 0;
		buffer_gpu_ptr = 0;
	}
}

void H3D::VertexBufferContainer::buildBufferObjectSingleAttribute(GLenum primitiveType, void* vertexArray, unsigned int vertexArraySize, unsigned int numVertices, 
	bool normalized /* = GL_FALSE */, GLuint attributeIndex /* = 0 */, GLenum target /* = GL_ARRAY_BUFFER */, 
	unsigned int stride /*= 0*/, unsigned int startOffset /*= 0*/, bool dynamic /*= false*/) {

		//First clean up any old data... ..
		//Do we need to do this? Reuse buffer?
		releaseBuffer();
		vadContainer.resize(1);

		unsigned int sizeOfPrimitive = sizeOfOGLType(primitiveType);
		unsigned int size_per_vertex = (vertexArraySize/numVertices);

		//If this is true it means we only have one element per vertex, because it is just a primitive
		if(size_per_vertex == sizeOfPrimitive) {
			vadContainer[0].size = 1; //Hence 1
		} else {
			vadContainer[0].size = size_per_vertex/sizeOfPrimitive; //Otherwise we figure out 
		}

		vadContainer[0].attributeIndex = attributeIndex;
		vadContainer[0].type = primitiveType;
		vadContainer[0].normalized = normalized;
		vadContainer[0].startOffset = startOffset;
		vadContainer[0].stride = stride;

		buffer_target = target;
		buffer_size = vertexArraySize;
		buffer_usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if(buffer_ID == 0) {
			glGenBuffers(1, &buffer_ID);
		}

		glBindBuffer(buffer_target, buffer_ID);

		// If we use persistent buffers we will make it resident in GPU memory and save a literal pointer to it for use in shaders.
		if(use_persistent) {
			glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, vertexArraySize, vertexArray, 0);
			/* In the future: glMapBufferRange with GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT . After that we should have a pointer that we can memcpy into. */
			glGetBufferParameterui64vNV(buffer_target, GL_BUFFER_GPU_ADDRESS_NV, &buffer_gpu_ptr);
			glMakeBufferResidentNV(buffer_target, GL_READ_ONLY);
		} else {
				glBufferData(buffer_target, buffer_size, vertexArray, buffer_usage);
		}
}

void H3D::VertexBufferContainer::buildBufferObjectInterleavedAttributes(GLenum target, void* vertexArray, unsigned int vertexArraySize, 
	std::vector<VertexAttributeDescription> VADs, bool dynamic /*= false*/) {
		//First clean up any old data... ..
		//Do we need to do this? Reuse buffer?
		releaseBuffer();
		vadContainer = VADs;

		//This should hopefully be the same for all VADs.
		//size_per_vertex = vadContainer[0].stride;
		buffer_target = target; 
		buffer_size = vertexArraySize;
		buffer_usage = (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		if(buffer_ID == 0) {
			glGenBuffers(1, &buffer_ID);
		}

		glBindBuffer(buffer_target, buffer_ID);

		// If we use persistent buffers we will make it resident in GPU memory and save a literal pointer to it for use in shaders.
		if(use_persistent) {
			glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, vertexArraySize, vertexArray, 0);
			glGetBufferParameterui64vNV(buffer_target, GL_BUFFER_GPU_ADDRESS_NV, &buffer_gpu_ptr);
			glMakeBufferResidentNV(buffer_target, GL_READ_ONLY);
		} else {
				glBufferData(buffer_target, buffer_size, vertexArray, buffer_usage);
		}
}

void H3D::VertexBufferContainer::updateBufferObject(void* vertexArray, unsigned int vertexArraySize) {
	//Make sure we aren't writing more data than we have room for

	//Fast version
	if(!use_persistent && vertexArraySize == buffer_size) {
		//We do the orphaning technique, described here: https://www.opengl.org/wiki/Buffer_Object_Streaming#Buffer_re-specification
		glBindBuffer(buffer_target, buffer_ID);
		glBufferData(buffer_target, buffer_size, NULL, buffer_usage);
		glBufferData(buffer_target, buffer_size, vertexArray, buffer_usage);
	} else { //Safe, slow version
		if(vertexArraySize < buffer_size) {
			if(vadContainer.size() > 1) {
				buildBufferObjectInterleavedAttributes(buffer_target, vertexArray, vertexArraySize, vadContainer, (buffer_usage==GL_DYNAMIC_DRAW) ? true : false);
			} else {
				buildBufferObjectSingleAttribute(vadContainer[0].type, vertexArray, 
					vertexArraySize, vadContainer[0].size, vadContainer[0].normalized, vadContainer[0].attributeIndex, buffer_target, vadContainer[0].stride, (buffer_usage == GL_DYNAMIC_DRAW) ? true : false);
			}
		}
	}
}