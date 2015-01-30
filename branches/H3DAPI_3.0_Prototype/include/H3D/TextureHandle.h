#ifndef __TEXTUREHANDLE_H__
#define __TEXTUREHANDLE_H__

namespace H3D {
		//Do I need more than this? 8byte
		struct TextureHandle {
			TextureHandle() 
			: ogl_texture_id(0), slice(0), parentHandle(0)
			{}

			GLuint ogl_texture_id;		// The actual texture ID. in the future prolly removed because they wont have an individual oglID, only an index into the parent
			unsigned char slice;		// <- This needed to keep track of where in the array it is.
			unsigned char parentHandle; // <- This needed? Yes.
		};
}
#endif