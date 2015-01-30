#ifndef __OGLUTIL_H__
#define __OGLUTIL_H__

#include <GL/glew.h>
#include <vector>
#include <string>

namespace H3D
{
	//************************************
	// Method:    LogOGLErrors
	// FullName:  H3D::LogOGLErrors
	// Access:    public 
	// Returns:   bool
	// Qualifier: Simple function that calls glGetError and prints all error messages, if there are any. It returns true if any errors happened, so that could be useful.
	// Parameter: std::string functionName <- Send in a qualifying string so that we know where the errors happened.
	// Parameter: unsigned int functionCall <- Optional counter so that we know how how often the errors happen, or which frame it happened, depending entirely on how you're keeping count.
	//************************************
	bool LogOGLErrors(std::string functionName, unsigned int functionCall = 0);


	//************************************
	// Method:    sizeOfOGLType
	// FullName:  H3D::sizeOfOGLType
	// Access:    public 
	// Returns:   unsigned int
	// Description: Simple utility function for figuring out byte size of OGL primitive types
	// Parameter: GLenum type
	//************************************
	unsigned int sizeOfOGLType(GLenum type);

	template <typename T>
	void bufferData(GLenum _target, const std::vector<T>& _data, GLenum _usage);

	template <typename T>
	void bufferDataArray(GLenum _target, const T* _data, GLint _count, GLenum _usage);

	template <typename T>
	GLuint newBufferFromVector(GLenum _target, const std::vector<T>& _data, GLenum _usage);
}
#endif