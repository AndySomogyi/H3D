#include <H3D/TextureArrayContainer.h>

using namespace H3D;

TextureArrayContainer::TextureArrayContainer(GLenum _texture_target, int _texWidth, int _texHeight, int _texDepth, GLint _internalFormat,
	GLenum _pixelFormat, GLenum _pixelComponentType, TextureProperties* _properties, bool _forcePowerOfTwo, bool sparse /* = false */, unsigned int _maxTextureCount /* = 32 */)
	:	texture_target(_texture_target),
	texWidth(_texWidth),
	texHeight(_texHeight),
	texDepth(_texDepth),
	internalFormat(_internalFormat),
	pixelFormat(_pixelFormat),
	pixelComponentType(_pixelComponentType),
	texture_properties(_properties),
	maxTextureCount(_maxTextureCount),
	forcePowerOfTwo(_forcePowerOfTwo) {
		texArrayID = 0;
}

TextureArrayContainer::~TextureArrayContainer() {
	//Make sure we free all the textures.
	for(unsigned int i = 0; i < texHandles.size(); ++i) {
		glDeleteTextures(1, &texHandles[i]);
	}

	if(texArrayID != 0) {
		glDeleteTextures(1, &texArrayID);
	}
}

GLuint TextureArrayContainer::getArrayID() {
	return texArrayID;
}


GLuint TextureArrayContainer::getTextureHandle(const TextureHandle& handle) {
	return texHandles[handle.slice];
}

void TextureArrayContainer::removeTexture(TextureHandle& handle) {
	handle.ogl_texture_id = 0;
	handle.parentHandle = 0;
	handle.slice = 0;

	if(texHandles[handle.slice] != 0) {
		glDeleteTextures(1, &texHandles[handle.slice]);
	}
}

//Returns false if it's full
bool TextureArrayContainer::createTextureHandle(TextureHandle& outHandle) {
	unsigned int index = texHandles.size();

	if(index < maxTextureCount) {
		GLuint texID = 0;
		glGenTextures(1, &texID);
		glBindTexture(texture_target, texID);

		texHandles.push_back(texID);
		outHandle.ogl_texture_id = texID;
		outHandle.slice = index;

		return true;
	}

	return false;
}

/*
target

Specifies the target texture. Must be GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, 
GL_TEXTURE_1D_ARRAY, GL_PROXY_TEXTURE_1D_ARRAY, GL_TEXTURE_RECTANGLE, 
GL_PROXY_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 
GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 
GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, or GL_PROXY_TEXTURE_CUBE_MAP. 
*/
void TextureArrayContainer::buildTexture(TextureHandle handle, Image* imageData) {
	//Bind the correct texture
	glActiveTexture(texHandles[handle.ogl_texture_id]);

	switch(texture_target) {

	case GL_TEXTURE_1D: {
		build1DTexture(handle, imageData);
						}

	case GL_PROXY_TEXTURE_1D: {
		build1DTexture(handle, imageData);
							  }

	case GL_TEXTURE_2D: {
		build2DTexture(handle, imageData);
						}

	case GL_PROXY_TEXTURE_2D: {
		build2DTexture(handle, imageData);
							  }

	case GL_TEXTURE_1D_ARRAY: {
		build2DTexture(handle, imageData);
							  }

	case GL_PROXY_TEXTURE_1D_ARRAY: {
		build2DTexture(handle, imageData);
									}

	case GL_TEXTURE_RECTANGLE: {
		build2DTexture(handle, imageData);
							   }

	case GL_PROXY_TEXTURE_RECTANGLE: {
		build2DTexture(handle, imageData);
									 }

	case GL_TEXTURE_CUBE_MAP_POSITIVE_X: {
		build2DTexture(handle, imageData);
										 }

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: {
		build2DTexture(handle, imageData);
										 }

	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: {
		build2DTexture(handle, imageData);
										 }

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: {
		build2DTexture(handle, imageData);
										 }

	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: {
		build2DTexture(handle, imageData);
										 }

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {
		build2DTexture(handle, imageData);
										 }

	case GL_PROXY_TEXTURE_CUBE_MAP: {
		build2DTexture(handle, imageData);
									}

	case GL_TEXTURE_3D: {
		build3DTexture(handle, imageData);
						}

	case GL_PROXY_TEXTURE_3D: {
		build3DTexture(handle, imageData);
							  }

	case GL_TEXTURE_2D_ARRAY:	{
		build3DTexture(handle, imageData);
								}

	case GL_PROXY_TEXTURE_2D_ARRAY: {
		build3DTexture(handle, imageData);
									}

	default: {
		//Report error... Throw?
			 }
	}
}

void TextureArrayContainer::build1DTexture(TextureHandle handle, Image* imageData) {
	const void* image_data = imageData->getImageData();

	if( texture_properties && texture_properties->generateMipMaps->getValue() ) {
		gluBuild1DMipmaps(  texture_target, 
			internalFormat,
			texWidth,
			pixelFormat,
			pixelComponentType,
			image_data );
	} else {
		H3DInt32 border_width = 
			texture_properties ? 
			texture_properties->borderWidth->getValue() : 0;

		if( border_width < 0 || border_width > 1 ) {
			//Possibly error report.
			border_width = 0;
		}

		glTexImage1D(texture_target, 0, internalFormat, texWidth, 
			border_width, pixelFormat, pixelComponentType, image_data);
	}
}

void TextureArrayContainer::build2DTexture(TextureHandle handle, Image* imageData) {
	const void* image_data = imageData->getImageData();

	if( texture_properties && texture_properties->generateMipMaps->getValue() ) {
		gluBuild2DMipmaps(  texture_target, 
			internalFormat,
			texWidth,
			texHeight,
			pixelFormat,
			pixelComponentType,
			image_data );
	} else {
		H3DInt32 border_width = 
			texture_properties ? 
			texture_properties->borderWidth->getValue() : 0;

		if( border_width < 0 || border_width > 1 ) {
			//Possibly error report.
			border_width = 0;
		}

		glTexImage2D(texture_target, 0, internalFormat, texWidth, 
			texHeight, border_width, pixelFormat, pixelComponentType, image_data);
	}
}

void TextureArrayContainer::build3DTexture(TextureHandle handle, Image* imageData) {
	const void* image_data = imageData->getImageData();

	if( texture_properties && texture_properties->generateMipMaps->getValue() ) {
#if( GLU_VERSION_1_3 )
		gluBuild3DMipmaps(  texture_target, 
			internalFormat,
			texWidth,
			texHeight,
			texDepth,
			pixelFormat,
			pixelComponentType,
			image_data);
#endif
	} else {
		H3DInt32 border_width = 
			texture_properties ? 
			texture_properties->borderWidth->getValue() : 0;

		if( border_width < 0 || border_width > 1 ) {
			//Possibly error report.
			border_width = 0;
		}

		glTexImage3D(texture_target, 0, internalFormat, texWidth, 
			texHeight, texDepth, border_width, pixelFormat, pixelComponentType, image_data);
	}
}