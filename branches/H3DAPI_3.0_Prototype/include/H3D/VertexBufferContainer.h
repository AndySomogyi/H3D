#ifndef __VERTEXCONTAINEROBJECT_H__
#define __VERTEXCONTAINEROBJECT_H__

#include <vector>
#include <H3D/OGLUtil.h>
#include <H3D/VertexAttributeDescription.h>

namespace H3D {
	class VertexBufferContainer {

	static const GLenum persistentBufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	public:
		VertexBufferContainer(bool usingBindless = false);
		VertexBufferContainer(const VertexBufferContainer& rhs);

		VertexBufferContainer& operator=(const VertexBufferContainer& rhs);
		~VertexBufferContainer();

	private:
		void incrementRefCount() {
			++refCount;
		}


		/// Cleanup function. Called by destructor.
		void releaseBuffer();

	public:
		///************************************
		/// Method:    buildBufferObjectSingleAttribute
		/// FullName:  H3D::VertexBufferContainer::buildBufferObjectSingleAttribute
		/// Access:    public 
		/// Returns:   void
		/// 
		/// Description: Call this function to create an OpenGL buffer. It will be managed internally by this object.
		/// All size parameters are expected to be in number of bytes.
		/// 
		/// Parameter: GLenum target => Target for binding your buffer etc. For example: GL_VERTEX_ARRAY.
		/// Parameter: GLenum primitiveType => The corresponding OpenGL type for our base elements. For example: GL_FLOAT or GL_DOUBLE.
		/// Parameter: bool normalized => Whether we want the values that are sent in to OpenGL be normalized or not.
		/// Parameter: void* vertexArray => Array pointer to your vertex data. If you use vectors, you send in a ptr to the first element, a là: &vector[0].
		/// Parameter: unsigned int vertexArraySize => Length of the above array.
		/// Parameter: unsigned int numVertices => Between vertexArraySize and numVertices we can figure out a bunch of stuff that can be useful.
		/// Parameter: unsigned int stride => The byte offset between consecutive vertices. If it's a tightly packed buffer just leave it at 0.
		/// Parameter: unsigned int startOffset => Can be used if you don't want to start at the beginning of the buffer when rendering. Else just leave at 0.
		/// Parameter: bool dynamic => Hint for how often you're going to be updating this vertex buffer.
		///************************************
		void buildBufferObjectSingleAttribute(GLenum primitiveType, void* vertexArray, unsigned int vertexArraySize, unsigned int numVertices, bool normalized = GL_FALSE, 
			GLuint attributeIndex = 0, GLenum target = GL_ARRAY_BUFFER, unsigned int stride = 0, unsigned int startOffset = 0, bool dynamic = false);

		///************************************
		/// Method:    buildBufferObjectInterleavedAttributes
		/// FullName:  H3D::VertexBufferContainer::buildBufferObjectInterleavedAttributes
		/// Access:    public 
		/// Returns:   void
		/// 
		/// Description: Call this function to create an OpenGL buffer that contains several different attributes interleaved.
		/// 
		/// Parameter: GLenum target => Target for binding your buffer etc. For example: GL_VERTEX_ARRAY.
		/// Parameter: void* vertexArray => pointer to the array containing the vertex data. The data will be memcopied into OpenGL-space memory, 
		/// so you don't actually have to keep it around to keep the VBO data valid.
		/// 
		/// Parameter: unsigned int vertexArraySize => Size of the vertex array in bytes.
		/// Parameter: std::vector<VertexAttributeDescription> VADs => For each type of element you'll have to insert a VertexAttributeDescription. 
		///	Because it's interleaved you'll have to know what you're doing or you'll have undefined behavior on your hands.
		/// Parameter: bool dynamic => Hint for how often you're going to be updating this vertex buffer.
		///************************************
		void buildBufferObjectInterleavedAttributes(GLenum target,  void* vertexArray, unsigned int vertexArraySize, std::vector<VertexAttributeDescription> VADs, bool dynamic = false);

		/// Update the vertex data. It expects vertexArraySize in bytes, like everything else in this class.
		/// It does check the vertexArraySize to make sure that we aren't writing too much data, and if array is too large is fails silently.
		void updateBufferObject(void* vertexArray, unsigned int vertexArraySize);

		/// Used for bindless rendering if we use persistent buffers.
		inline GLuint64 getBufferGPUPointer(){
			return buffer_gpu_ptr;
		}

		/// Get the target for your buffer. This specifies what type of buffer it is. For example: GL_VERTEX_ARRAY or GL_ELEMENT_ARRAY.
		inline GLenum getBufferTarget() {
			return buffer_target;
		}
		/// Normal ID used for typical VBO rendering.
		inline GLuint getBufferID() {
			return buffer_ID;
		}

		/// Get the actual total size of the buffer in bytes.
		inline GLsizei getBufferSize() {
			return buffer_size;
		}

		/// Get one VertexAttributeDescription. Per default just the first (or only) one.
		inline VertexAttributeDescription& getVAD(unsigned int index = 0) {
			return vadContainer[index];
		}

		/// Get the vector containing all the VADs.
		inline std::vector<VertexAttributeDescription>& getVADContainer() {
			return vadContainer;
		}

		/// ID used to identify and compare against other layouts. Used for sorting inside renderer. Do ignore.
		inline unsigned int getVADLayout() {
			return vertexAttributeLayout;
		}

		/// Setter for ID used to identify and compare against other layouts. Used for sorting inside renderer. Do ignore.
		inline void setVADLayout(unsigned int val) {
			vertexAttributeLayout = val;
		}

		/// Which attribute slot it will be bound to when rendering.
		inline void setAttributeIndex(unsigned int val) {
			//If we only have one VAD, we want to set it to the value, and if we have several VADs, it's interleaved.... 
			// and they should all have the same value..? I think. Or val+i
			for(unsigned int i = 0; i < vadContainer.size(); ++i) {
				vadContainer[i].attributeIndex = val;
			}
		}


	private:
		/// Whether the buffer will be persistently mapped or not
		bool use_persistent;

		/// Hint for how exactly we're going to be using this buffer. Either GL_STATIC_DRAW or GL_DYNAMIC_DRAW
		GLenum buffer_usage;

		/// The buffer's ID inside OpenGL. Used when rendering normally.
		GLuint buffer_ID;

		/// If we make it persistent in memory, this is where the pointer will be stored
		GLuint64 buffer_gpu_ptr;

		/// Size of the entire buffer, in bytes
		GLuint buffer_size;

		/// For example: GL_VERTEX_ARRAY or GL_ELEMENT_ARRAY_BUFFER.
		GLenum buffer_target;

		/// Struct that contains all the data that is 
		/// needed to describe a vertex attribute for OpenGL.
		std::vector<VertexAttributeDescription> vadContainer;

		/// ID used to identify and compare against other layouts. Used for internal sorting.
		unsigned int vertexAttributeLayout;

		/// Simple way to make sure we don't delete the VBO until it's needed.
		unsigned int refCount;
	};
}

#endif