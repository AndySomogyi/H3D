#ifndef __TEXTUREARRAYCONTAINER_H__
#define __TEXTUREARRAYCONTAINER_H__

#include <H3D/TextureProperties.h>
#include <H3D/TextureHandle.h>

namespace H3D {
	class TextureArrayContainer {
	friend class TextureManager; //Easier than getters and setters...

	public:
		TextureArrayContainer(GLenum texture_target, int texWidth, int texHeight, int texDepth, GLint internalFormat,
			GLenum pixelFormat, GLenum pixelComponentType, TextureProperties* properties, 
			bool forcePowerOfTwo = true, bool sparse = false, unsigned int maxTextureCount = 32);
		
		~TextureArrayContainer();

		GLuint getArrayID();
		GLuint getTextureHandle(const TextureHandle& handle);
		void removeTexture(TextureHandle& handle);

		//Returns false if it's full
		bool createTextureHandle(TextureHandle& outHandle);
		void buildTexture(TextureHandle handle, Image* imageData);

	private:
		void build1DTexture(TextureHandle handle, Image* imageData);
		void build2DTexture(TextureHandle handle, Image* imageData);
		void build3DTexture(TextureHandle handle, Image* imageData);

	private:
		/* All of the below values together define the 
		memory layout of the textures in this array
		and how they are created. */
		bool forcePowerOfTwo;
		int texWidth, texHeight, texDepth;
		GLint internalFormat;
		GLenum pixelFormat;
		GLenum pixelComponentType;
		GLenum texture_target;
		TextureProperties* texture_properties;
		
		GLuint texArrayID;					//OpenGL handle to the texture array. Currently not used!
		std::vector<GLuint> texHandles;     //OpenGL handles to the individual textures. Maybe removed in future, we'll see.
		unsigned int maxTextureCount;		//We need a max size to define how large the texture array can be in the future.
	};
}

#endif