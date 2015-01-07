#ifndef __VERTEXCONTAINEROBJECT_H__
#define __VERTEXCONTAINEROBJECT_H__

#include <GL/glew.h>
#include <vector>

namespace H3D
{
	template<typename T>
	class VertexContainerObject
	{
		public:
		VertexContainerObject();
		~VertexContainerObject();

		//************************************
		// Method:    CreateTriangles
		// FullName:  H3D::VertexContainerObject<T>::CreateTriangles
		// Access:    public 
		// Returns:   GLuint64 <- Handle that OpenGL uses to access this buffer.
		// Qualifier:
		// Parameter: GLenum target <- For example GL_VERTEX_ARRAY or GL_ELEMENT_ARRAY_BUFFER.
		// Parameter: std::vector<T> vertexData	<- Your vertex data.
		// Parameter: GLsizei elementsPerStride	<- How many elements each vertex is supposed to be made up of. 
		// If it's a vertex struct, then you'd set elementsPerStride to 1, for example.
		//************************************
		GLuint64 CreateTriangles(GLenum target, std::vector<T> vertexData, GLsizei elementsPerStride);

		/// Based on whether this attribute is bindless or not
		/// it either returns the 64bit unsigned gpu addr or the normal vbo_id casted to a 64bit uint.
		inline GLuint64 getBufferID() {
			if(persistentlyMapped) {
				return buffer_gpu_ptr;
			}
			
			//Implicit cast from 32bit to 64bit. Shouldn't 
			return buffer_id;
		}

		private:
		//Whether the buffer will be persistently mapped or not
		bool persistentlyMapped;

		//Handle for our object that we'll create in the graphics API
		GLuint	buffer_id;

		//If we make it persistent in memory, this is where the pointer will be stored
		GLuint64 buffer_gpu_ptr;

		//Stride is the byte offset between two vertices
		GLsizei bufferStride;

		//Total buffer size
		GLsizei bufferSize;
	};
}

#endif