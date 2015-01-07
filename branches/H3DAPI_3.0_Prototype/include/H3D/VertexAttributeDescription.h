#ifndef __VERTEXATTRIBUTEDESCRIPTION_H__
#define __VERTEXATTRIBUTEDESCRIPTION_H__

#include <gl/glew.h>

namespace H3D {
	/// Simple class to describe a vertex attribute.
	/// Useful to sort on.
	class VertexAttributeDescription {
	public:
		VertexAttributeDescription();

		VertexAttributeDescription& operator=(VertexAttributeDescription& rhs);

		//This shouldn't be necessary, it should just per default work, but it doesn't.
		bool operator==(const VertexAttributeDescription& rhs);
	
		GLenum primitiveType;
		GLint attributeSize;
		GLuint bufferIndex;
		bool normalized;
		GLsizei stride;
		GLuint elementCount;
		GLuint bufferID; //If we're using non-bindless we need the buffer ID to bind
		GLenum target; //Target buffer type, GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER for example
	};
}
#endif 