#ifndef __PROTOTYPESHADER_H__
#define __PROTOTYPESHADER_H__

#include <GL/glew.h>
#include <H3D/Node.h>
#include <string>
#include <vector>

#include <H3D/OGLRenderStates.h>

namespace H3D {
	class PrototypeShader {
	public:
		PrototypeShader(std::string _vertexShader, std::string _fragmentShader);
		~PrototypeShader();

		virtual void traverseSG(TraverseInfo& ti);
		GLuint getProgram();
		GLuint getMatrixUniformLocation();

		inline void setBindlessUsage(bool val){
			usingBindless = val;
		}

	private:
		bool initializeShader(std::string vertexShader, std::string fragmentShader);
		void printShaderLog(GLuint shaderID);

		std::string readFileFromUrl(const std::string& url);

	private:
		bool usingBindless;
		bool usesViewProj;
		bool isValid;
		GLuint program_ID;
		GLuint viewProjUniformLocation;
		GLuint worldMatrixUniformLocation;
		std::string vertexShaderString;
		std::string fragmentShaderString;
		std::vector<UniformDescription> uniforms;
	};
}

#endif