#ifndef __VERTEXATTRIBUTEDESCRIPTION_H__
#define __VERTEXATTRIBUTEDESCRIPTION_H__

#include <gl/glew.h>

namespace H3D {
	/// Simple class to define an array of generic vertex attribute data.
	class VertexAttributeDescription {
	public:
		VertexAttributeDescription(const GLuint attributeIndex = 0, const GLint size = 0, const GLenum type = GL_FLOAT, 
			const GLboolean normalized = GL_FALSE, const GLsizei stride = 0, const GLuint startOffset = 0);

		VertexAttributeDescription& operator=(VertexAttributeDescription& rhs);

		//This shouldn't be necessary, it should just per default work, but it seems like it doesn't.
		bool operator==(const VertexAttributeDescription& rhs);
	
		///	Specifies the index of the generic vertex attribute to be modified.
		GLuint attributeIndex;

		///	Specifies the number of components per generic vertex attribute. Must be 1, 2, 3, 4. 
		///	Additionally, the symbolic constant GL_BGRA is accepted by glVertexAttribPointer.
		GLint size;
		
		///	Specifies the data type of each component in the array. The symbolic constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, 
		///	and GL_UNSIGNED_INT are accepted by glVertexAttribPointer and glVertexAttribIPointer. 
		///	Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and 
		///	GL_UNSIGNED_INT_10F_11F_11F_REV are accepted by glVertexAttribPointer. 
		///	GL_DOUBLE is also accepted by glVertexAttribLPointer and is the only token accepted by the type parameter for that function. 
		GLenum type;

		///	For glVertexAttribPointer, specifies whether fixed-point data values should be normalized (GL_TRUE) 
		///	or converted directly as fixed-point values (GL_FALSE) when they are accessed.
		GLboolean normalized;

		///	Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, 
		///	the generic vertex attributes are understood to be tightly packed in the array.
		GLsizei stride;

		///	Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the 
		///	buffer currently bound to the GL_ARRAY_BUFFER target.
		GLuint startOffset;
	};
}
#endif 