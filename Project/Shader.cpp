#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility>
#include <GL/glew.h>
#include <GL/gl.h>

Shader::Shader(const char* vertexPath, const char* fragPath)
{
	GLint is_succeed = 1;

	std::ifstream stream(fragPath);
	std::string file;
	std::stringstream f_shader_stream;

	std::ifstream v_stream(vertexPath);
	std::string vfile;
	std::stringstream v_shader_stream;

	if (stream.is_open())
	{
		f_shader_stream << stream.rdbuf();
		stream.close();
		file = f_shader_stream.str();
	}
	else
	{
		throw std::runtime_error("Failed to open fragment shader file");
	}

	const char* f_shader_code = file.c_str();

	if (v_stream.is_open())
	{
		v_shader_stream << v_stream.rdbuf();
		v_stream.close();
		vfile = v_shader_stream.str();
	}
	else
	{
		throw std::runtime_error("Failed to open vertex shader file");
	}

	const char* v_shader_code = vfile.c_str();

	/*
	 * Create & Compile fragment shader.
	 * Check the compile status.
	 */
	//const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &f_shader_code, 0);
	glCompileShader(fragmentShaderId);

	glGetProgramiv(fragmentShaderId, GL_COMPILE_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(fragmentShaderId);
		throw std::runtime_error("failed to compile fragment");
	}

	/*
	 * Create & Compile vertex shader.
	 * Check the compile status.
	 */
	//const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &v_shader_code, 0);
	glCompileShader(vertexShaderId);

	glGetProgramiv(vertexShaderId, GL_COMPILE_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(vertexShaderId);
		throw std::runtime_error("failed to compile vertex");
	}

	/*
	 * 1.Create program, attach shader to the program.
	 * 2.Link & Set use.
	 */
	programId = glCreateProgram();
	glAttachShader(programId, fragmentShaderId);
	glAttachShader(programId, vertexShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);

	/*
	 * Set the ucolor & aposition uniform variable.
	 */
	 //ucolor = glGetUniformLocation(program, "color");
	 //aposition = glGetAttribLocation(program, "position");

	glGetProgramiv(programId, GL_LINK_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(vertexShaderId);
		glDeleteShader(fragmentShaderId);
		throw std::runtime_error("failed to link");
	}

	glEnable(GL_DEPTH_TEST);


	/*
	 * Done all stuff, delete both vertex and fragment shader.
	 */
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	unsigned int uniformBlockIndex = glGetUniformBlockIndex(programId, "Matrices");
	glUniformBlockBinding(programId, uniformBlockIndex, 0);
}

Shader::Shader(const char* computeShader)
{
	std::ifstream stream(computeShader);
	std::string file;
	std::stringstream shader_stream;

	if (stream.is_open())
	{
		shader_stream << stream.rdbuf();
		stream.close();
		file = shader_stream.str();
	}
	else
	{
		throw std::runtime_error("Failed to open compute shader file");
	}

	const char* v_shader_code = file.c_str();
	
	GLint is_succeed = 1;
	computeShaderId = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShaderId, 1, &v_shader_code, NULL);
	glCompileShader(computeShaderId);

	programId = glCreateProgram();
	glAttachShader(programId, computeShaderId);
	glLinkProgram(programId);

	glGetProgramiv(programId, GL_LINK_STATUS, &is_succeed);

	if(is_succeed == GL_FALSE)
	{
		throw std::runtime_error("compute shader link fail");
	}

	glDeleteShader(computeShaderId);
}

unsigned Shader::GetShaderId()
{
	return programId;
}

void Shader::Use()
{
	glUseProgram(programId);
}

void Shader::SendUniformMat(std::string uniformName, void* val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	float* valInFloat = static_cast<float*>(val);
	glUniformMatrix4fv(loc, 1, GL_TRUE, valInFloat);
}

void Shader::SendUniformInt(std::string uniformName, void* val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	int* valInInt = static_cast<int*>(val);
	glUniform1i(loc, *valInInt);
}

void Shader::SendUniformInt(std::string uniformName, int val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	glUniform1i(loc, val);
}

void Shader::SendUniformFloat(std::string uniformName, void* val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	float* valInFloat = static_cast<float*>(val);
	glUniform1f(loc, *valInFloat);
}

void Shader::SendUniformFloat(std::string uniformName, float val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	glUniform1f(loc, val);
}

void Shader::SendUniformVec3(std::string uniformName, void* val) const
{
	const unsigned loc = glGetUniformLocation(programId, uniformName.c_str());
	float* valInFloat = static_cast<float*>(val);
	glUniform3f(loc, valInFloat[0], valInFloat[1], valInFloat[2]);
}

Shader::~Shader()
{
	glDeleteProgram(programId);
}
