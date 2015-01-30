#ifndef __ELEMENTBUFFERCONTAINER_H__
#define __ELEMENTBUFFERCONTAINER_H__

#include <GL/glew.h>
#include <vector>

namespace H3D {
	class ElementBufferContainer {
	static const GLenum persistentBufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	public:
		ElementBufferContainer(bool usePersistent = false);
		ElementBufferContainer(const ElementBufferContainer& rhs);
		~ElementBufferContainer();

		ElementBufferContainer& operator=(const ElementBufferContainer& rhs);

	private:
		void releaseBuffer();
		void inline incrementRefCount() {
			++refCount;
		}

	public:
		///************************************
		/// Method:    buildBufferObject
		/// FullName:  H3D::ElementBufferContainer::buildBufferObject
		/// Access:    public 
		/// Returns:   void
		/// Qualifier:
		/// 
		/// Parameter: const void* elementArray => Array pointer to your element data. 
		/// If you use vectors, you send in a ptr to the first element, a là: &vector[0].
		/// 
		/// Parameter: unsigned int arraySize => Array size in bytes. 
		/// If you use vectors it's vector.size() * sizeof(T) where T is the same as the T you've specified for your vector.
		/// 
		/// Parameter: GLenum primitiveType => The corresponding OpenGL type for our base elements. For example: GL_BYTE or GL_UNSIGNED_INT.
		///************************************
		void buildBufferObject(const void* elementArray, unsigned int arraySize, unsigned int indexCount, GLenum primitiveType);

		/// The OGL handle to the buffer. Bind when rendering elements.
		GLuint getBufferID();

		/// Used for bindless rendering.
		GLuint64 getBufferGPUPtr();

		/// Self explanatory. Fetch buffer size, in bytes.
		GLsizei getBufferSize();

		/// Get the corresponding OpenGL type for our base elements. For example: GL_BYTE or GL_UNSIGNED_INT.
		GLenum getPrimitiveType();

		GLuint getIndexCount();

	private:
		GLuint64 buffer_gpu_ptr;
		GLenum buffer_target;
		GLenum primitive_type;
		GLuint buffer_id;
		GLuint index_count;
		GLsizei buffer_size; //Size of the entire buffer in bytes
		bool use_persistent;

		/// Simple way to make sure we don't delete the buffer until it's needed.
		unsigned int refCount;
	};
}

#endif