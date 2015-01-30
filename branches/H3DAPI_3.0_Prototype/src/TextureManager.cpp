#include <H3D/TextureManager.h>

#include <H3D/ResourceResolver.h>
#include <H3D/X3DTextureNode.h>

using namespace H3D;

TextureManager::TextureManager() {

}

TextureManager::~TextureManager() {
	for(std::map<std::string, Image*>::iterator it = imageDatabase.begin(); it != imageDatabase.end(); ++it) {
		delete it->second;
	}
}

void TextureManager::removeTexture(TextureHandle& inHandle) {
	arrayContainer[inHandle.parentHandle].removeTexture(inHandle);
}

GLuint TextureManager::getTexture(const TextureHandle& inHandle) {
	return arrayContainer[inHandle.parentHandle].getTextureHandle(inHandle);
}

bool TextureManager::createTextureHandle(GLenum texture_target, int texWidth, int texHeight, 
	int texDepth, GLint internalFormat, GLenum pixelFormat, GLenum pixelComponentType, 
	TextureProperties* properties, bool forcePowerOfTwo, TextureHandle& outHandle) {

		for(unsigned int i = 0; i < arrayContainer.size(); ++i) { 
			//This is ultrabad, we need to handle this more elegantly somehow.
			if(	texWidth == arrayContainer[i].texWidth &&
				texHeight == arrayContainer[i].texHeight &&
				texDepth == arrayContainer[i].texDepth &&
				internalFormat == arrayContainer[i].internalFormat &&
				pixelFormat == arrayContainer[i].pixelFormat &&
				pixelComponentType == arrayContainer[i].pixelComponentType &&
				forcePowerOfTwo == arrayContainer[i].forcePowerOfTwo &&
				texture_target == arrayContainer[i].texture_target &&
				*properties == *arrayContainer[i].texture_properties) {
					//Fill outHandle with an opengl texture ID and the correct array slice it's placed in
					arrayContainer[i].createTextureHandle(outHandle);
					outHandle.parentHandle = i;

					return true;
			}
		}

		//If we've reached this point we haven't found any matching texture array container.
		//So we create one that does match.
		unsigned int index = arrayContainer.size();
		arrayContainer.push_back(TextureArrayContainer(texture_target, texWidth, texHeight,
			texDepth, internalFormat, pixelFormat, pixelComponentType, properties, forcePowerOfTwo));
		//Get the new container and create an ogl handle for this texture
		arrayContainer[index].createTextureHandle(outHandle);
		outHandle.parentHandle = index;

		return true;
}

bool TextureManager::buildTexture(TextureHandle inHandle, Image* img) {
	//Fill up ...
	arrayContainer[inHandle.parentHandle].buildTexture(inHandle, img);
	return true;
}

Image* TextureManager::lookForDuplicateImage(std::string imageUrl) {
	std::map<std::string, Image*>::iterator it = imageDatabase.find(imageUrl);
	if(it != imageDatabase.end()) {	
		return it->second;
	}

	//Nullptr preferrably but not sure if we want to support C++11 yet.
	return 0;
}

void TextureManager::insertNewImageURLPairing(std::string url, Image* image) {
	std::pair<std::string, Image*> pairing(url, image);
	imageDatabase.insert(pairing);
}