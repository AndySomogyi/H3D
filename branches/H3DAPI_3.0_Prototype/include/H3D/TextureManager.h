#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <H3D/TextureProperties.h>
#include <H3D/FlyweightManager.h>
#include <H3D/TextureHandle.h>
#include <H3D/TextureArrayContainer.h>
#include <vector>
#include <map>
#include <H3D/H3DImageLoaderNode.h>

namespace H3D {
	class TextureManager {
	private:
		class X3DTextureNode;

		TextureManager();
		TextureManager(const TextureManager&); //Unimplemented because singleton.
		void operator=(const TextureManager&); //Unimplemented because singleton.

	public:

		/// Thrown by renderImage () if the dimensions of an image are
		/// invalid, e.g. not a power of 2.
		H3D_API_EXCEPTION( InvalidTextureDimensions );

		/// Thrown when an OpenGL error occurs while setting up
		/// the texture.
		H3D_API_EXCEPTION( OpenGLTextureError );

		/// Thrown when we can't find any matching pixel type when calling PixelComponentType.
		H3D_API_EXCEPTION( UnsupportedPixelComponentType );

		/// Thrown when we stumble upon an unsupported pixel type.
		H3D_VALUE_EXCEPTION(Image::PixelType, UnsupportedPixelType );

		//Singleton getter...
		static TextureManager& getInstance() {
			static TextureManager texMgr;
			return texMgr;
		}

	public:
		~TextureManager();

		/*	
		Remove/DeleteTexture <- Honestly not sure if this is necessary. They all get deleted when the application ends.
		*/
		void removeTexture(TextureHandle& inHandle);

		GLuint getTexture(const TextureHandle& inHandle);

		/*
		We create a texture handle. Then we try to find the right container for the texture handle based on ... the parameters.
		We do this so that textures with identical properties can be bundled together.
		*/
		bool createTextureHandle(GLenum texture_target, int texWidth, int texHeight, int texDepth, GLint internalFormat,
			GLenum pixelFormat, GLenum pixelComponentType, TextureProperties* properties, 
			bool forcePowerOfTwo, TextureHandle& outHandle);

		/*
		Bind the handle that you access with textureHandle, then fill it with data from the image
		*/
		bool buildTexture(TextureHandle inHandle, Image* img);

		/*
		Bind the handle that you access with textureHandle, then update the content with data from the image
		*/
		void updateTextureData(TextureHandle inHandle, Image* img);

		/* 
		Returns a valid image ptr if we find any. So check for nullptr.
		*/
		Image* lookForDuplicateImage(std::string imageUrl);

		/*
		Whenever we load an image and don't find a duplicate (we compare URLs) 
		we insert it and the URL here for future reference.
		*/
		void insertNewImageURLPairing(std::string url, Image* image);

	private:
		std::vector<TextureArrayContainer> arrayContainer;
		std::map<std::string, Image*> imageDatabase;
	};
}

#endif