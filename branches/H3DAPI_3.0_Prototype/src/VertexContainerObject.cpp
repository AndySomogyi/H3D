#include <H3D/VertexContainerObject.h>

using namespace H3D;

template<typename T>
H3D::VertexContainerObject<T>::VertexContainerObject()
	: buffer_id(0), objectGPUPointer(0)	{

	if(GLEW_EXT_direct_state_access && GL_NV_vertex_buffer_unified_memory) {
		persistentlyMapped = true;
	} else {
		persistentlyMapped = false;
	}
}


template<typename T>
H3D::VertexContainerObject<T>::~VertexContainerObject() {
	if(buffer_id != 0) {
		if(use_bindless) {
			glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glDeleteBuffersARB(1, &buffer_id);
			buffer_id = 0;
		} else {
			glDeleteBuffersARB(1, &buffer_idbuffer_id);
			buffer_id = 0;
		}
	}
}

template<typename T>
GLuint64 H3D::VertexContainerObject<T>::CreateTriangles(GLenum target, std::vector<T> vertexData, GLsizei elementsPerStride) {
	
	bufferStride = (elementsPerStride * sizeof(T));
	bufferSize = (vertexData.size() * sizeof(T));

	if(buffer_id == 0) {
		glGenBuffersARB(1, &buffer_id);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

	if(isDynamic->getValue()) {
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData.data(), GL_STREAM_DRAW);
	} else {
		glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexData.data(), GL_STATIC_DRAW);
	}

	if(persistentlyMapped) {
		glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &objectGPUPointer);
		glMakeBufferResidentNV(GL_ARRAY_BUFFER, GL_READ_ONLY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return objectGPUPointer;
	}

	return static_cast<GLuint64>(buffer_id);
}