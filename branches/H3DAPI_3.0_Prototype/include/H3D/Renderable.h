#ifndef __RENDERABLE_H__
#define __RENDERABLE_H__

#include <H3D/VertexBufferContainer.h>
#include <H3D/ElementBufferContainer.h>
#include <GL/glew.h>
#include <H3D/H3DTypes.h>

namespace H3D {
	/// Struct used to draw one object consisting of several VBOs, one IBO and some other stuff
	struct Renderable {
		std::vector<VertexBufferContainer> VBOs;
		ElementBufferContainer IBO;
		Matrix4f transform;
		GLenum renderMode;
		unsigned int totalRenderStateHandle;
		unsigned int vertexAttributeLayout;
	};
}
#endif