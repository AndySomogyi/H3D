#include <H3D/VertexAttributeDescription.h>

H3D::VertexAttributeDescription::VertexAttributeDescription(const GLuint _attributeIndex, const GLint _size, const GLenum _type, 
	const GLboolean _normalized, const GLsizei _stride, const GLuint _startOffset) :
attributeIndex(_attributeIndex), size(_size), type(_type), normalized(_normalized), stride(_stride), startOffset(_startOffset) {
}

H3D::VertexAttributeDescription& H3D::VertexAttributeDescription::operator=(VertexAttributeDescription& rhs) {
	attributeIndex	= rhs.attributeIndex;		
	size						= rhs.size;	
	type						= rhs.type;
	normalized			= rhs.normalized;
	stride					= rhs.stride;
	startOffset			= rhs.startOffset;

	return (*this);
}

bool H3D::VertexAttributeDescription::operator==(const VertexAttributeDescription& rhs) {
	return (
		attributeIndex	== rhs.attributeIndex &&		
		size						== rhs.size &&	
		type						== rhs.type &&
		normalized			== rhs.normalized &&
		stride					== rhs.stride &&
		startOffset			== rhs.startOffset
		);
}