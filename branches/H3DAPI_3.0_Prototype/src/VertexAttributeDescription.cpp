#include <H3D/VertexAttributeDescription.h>

H3D::VertexAttributeDescription::VertexAttributeDescription()
{
	attributeSize = 0;	
	bufferIndex = 0;		
	normalized = false;		
	stride = 0;	
	primitiveType = GL_FLOAT;
	bufferID = 0;
	target = 0;	
}

H3D::VertexAttributeDescription& H3D::VertexAttributeDescription::operator=(VertexAttributeDescription& rhs)
{
	attributeSize	= rhs.attributeSize;		
	bufferIndex		= rhs.bufferIndex;	
	normalized		= rhs.normalized;
	stride			= rhs.stride;
	primitiveType	= rhs.primitiveType;
	bufferID		= rhs.bufferID;
	target			= rhs.target;
	elementCount    = rhs.elementCount;

	return *this;
}



bool H3D::VertexAttributeDescription::operator==(const VertexAttributeDescription& rhs)
{
	return (
		rhs.attributeSize			== attributeSize			&&
		rhs.bufferIndex			== bufferIndex		&&
		rhs.normalized		== normalized	&&
		rhs.stride			== stride		&&
		rhs.primitiveType	== primitiveType); 
		/*
		&&
		rhs.bufferID		== bufferID &&
		rhs.target			== target
		*/
}