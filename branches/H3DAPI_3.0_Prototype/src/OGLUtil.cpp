#include <H3D/OGLUtil.h>

#include <iostream>

namespace H3D
{
	

	bool LogOGLErrors(std::string functionName, unsigned int functionCall /*= 0*/)
	{
		#ifdef HAVE_PROFILER
		static unsigned int timeSinceLast = 0;

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			unsigned int result = (functionCall - timeSinceLast);

			std::cout << gluErrorString(err) << " in function: " << functionName << std::endl;

			if(functionCall != 0) {
				timeSinceLast = functionCall;
			}

			return true;
		}
		#endif

		return false;
	}

	template <typename T>
	void bufferData(GLenum _target, const std::vector<T>& _data, GLenum _usage)
	{
		if(_data.size() > 0) 
		{
			glBufferData(_target, sizeof(T) * _data.size(), &*_data.cbegin(), _usage);
		}
		else 
		{
			glBufferData(_target, 0, nullptr, _usage);
		}
	}

	template <typename T>
	void bufferDataArray(GLenum _target, const T* _data, GLint _count, GLenum _usage)
	{
		if (_count > 0) 
		{
			glBufferData(_target, sizeof(T) * _count, _data, _usage);
		}
		else 
		{
			glBufferData(_target, 0, nullptr, _usage);
		}
	}

	template <typename T>
	GLuint newBufferFromVector(GLenum _target, const std::vector<T>& _data, GLenum _usage)
	{
		GLuint retVal = 0;
		glGenBuffers(1, &retVal);
		if (retVal == 0) 
		{
			return 0;
		}

		glBindBuffer(_target, retVal);
		bufferData(_target, _data, _usage);

		//assert(GLRenderer::GetApiError() == GL_NO_ERROR);

		return retVal;
	}

}