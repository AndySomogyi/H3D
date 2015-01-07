#include <H3D/PrototypeShader.h>

#include <string>
#include <fstream>

#include <H3DUtil/Console.h>

using namespace H3D;

H3D::PrototypeShader::PrototypeShader(std::string _vertexShader, std::string _fragmentShader) {
	usingBindless = false;
	usesViewProj = true;
	isValid = false; 
	vertexShaderString = _vertexShader; 
	fragmentShaderString = _fragmentShader;
	program_ID = 0;
	viewProjUniformLocation = 0;
	worldMatrixUniformLocation = 0;

	uniforms.resize(0);
	uniforms.push_back(UniformDescription(UniformDescription::fmat4x4, "ViewProjection", 0, 1, true, nullptr));
	uniforms.push_back(UniformDescription(UniformDescription::f1, "timeVar", 1, 1, true, nullptr));
}

H3D::PrototypeShader::~PrototypeShader() {
	if(program_ID != 0) {
		glDeleteProgram(program_ID);
	}
}

void H3D::PrototypeShader::traverseSG(TraverseInfo& ti) {

	bool shaderCompilationSucceeded = false;

	//viewProjection = val;

	//So we make sure that the shader is actually compiled
	if(!isValid) {
		shaderCompilationSucceeded = initializeShader(vertexShaderString, fragmentShaderString);
	}

	//If shader compilation succeeded, or it's already been compiled successfully before
	if(shaderCompilationSucceeded || isValid) {
		uniforms[0].location = glGetUniformLocation(program_ID, "ViewProjection");
		uniforms[1].location = glGetUniformLocation(program_ID, "timeVar");

		if(!usingBindless) {
			worldMatrixUniformLocation = glGetUniformLocation(program_ID, "World");
		}

		ti.getCurrentRenderstate().shader.uniforms = uniforms;
		ti.getCurrentRenderstate().shader.programID = program_ID;

		isValid = true;
	} else {
		//If we get here, the shader has not been compiled successfully and we don't want to add the current
		isValid = false;
	}
}

bool H3D::PrototypeShader::initializeShader(std::string _vertexShaderString, std::string _fragmentShaderString) {
	if(program_ID == 0) {
		//Read our shaders into the appropriate buffers
		std::string vertexSource;

		if(usingBindless) {
			vertexSource += "#define USE_BINDLESS";
		}

		vertexSource += readFileFromUrl(_vertexShaderString);

		std::string fragmentSource;
		fragmentSource += readFileFromUrl(_fragmentShaderString);

		//Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//Send the vertex shader source code to GL
		//Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = (const GLchar *)vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		//Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::string infoLog(maxLength, ' ');
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			//We don't need the shader anymore.
			glDeleteShader(vertexShader);

			//Use the infoLog as you see fit.
			std::cout <<	"Error when compiling vertexShader in PrototypeShader.cpp."
				"\nOpenGL error: " << infoLog << std::endl;

			//In this simple program, we'll just leave
			return false;
		}

		//Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		//Send the fragment shader source code to GL
		//Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar *)fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		//Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::string infoLog(maxLength, ' ');
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			//We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			//Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			//Use the infoLog as you see fit.
			std::cout <<	"Error when compiling fragmentShader in PrototypeShader.cpp."
				"\nOpenGL error: " << infoLog << std::endl;

			//In this simple program, we'll just leave
			return false;
		}

		//Vertex and fragment shaders are successfully compiled.
		//Now time to link them together into a program.
		//Get a program object.
		GLuint program = glCreateProgram();

		//Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		//Link our program
		glLinkProgram(program);

		//Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if(isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::string infoLog(maxLength, ' ');
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			//We don't need the program anymore.
			glDeleteProgram(program);
			//Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			//Use the infoLog as you see fit.
			std::cout <<	"Error when linking program in PrototypeShader.cpp."
				"\nOpenGL error: " << infoLog << std::endl;

			//In this simple program, we'll just leave
			return false;
		}

		//Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

		//Finally, save program ID
		program_ID = program;

		return true;
	}

	return false;
}

void H3D::PrototypeShader::printShaderLog(GLuint shaderID) {
	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);

	if(isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);


		//Use the infoLog as you see fit.

		//In this simple program, we'll just leave
		return;
	}
}

GLuint H3D::PrototypeShader::getProgram()
{
	return program_ID;
}

std::string H3D::PrototypeShader::readFileFromUrl(const std::string& url)
{
	std::ifstream inStream(url);

	std::string retString((std::istreambuf_iterator<char>(inStream)),
		std::istreambuf_iterator<char>());

	return retString;
}

GLuint H3D::PrototypeShader::getMatrixUniformLocation()
{
	return worldMatrixUniformLocation;
}
