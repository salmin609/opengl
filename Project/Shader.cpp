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

	const char* f_shader_code = file.c_str();

	if (v_stream.is_open())
	{
		v_shader_stream << v_stream.rdbuf();
		v_stream.close();
		vfile = v_shader_stream.str();
	}

	const char* v_shader_code = vfile.c_str();

	/*
	 * Create & Compile fragment shader.
	 * Check the compile status.
	 */
	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &f_shader_code, 0);
	glCompileShader(fragment_shader);

	glGetProgramiv(fragment_shader, GL_COMPILE_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(fragment_shader);
		throw std::runtime_error("failed to compile fragment");
	}

	/*
	 * Create & Compile vertex shader.
	 * Check the compile status.
	 */
	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &v_shader_code, 0);
	glCompileShader(vertex_shader);

	glGetProgramiv(vertex_shader, GL_COMPILE_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(vertex_shader);
		throw std::runtime_error("failed to compile vertex");
	}

	/*
	 * 1.Create program, attach shader to the program.
	 * 2.Link & Set use.
	 */
	shaderId = glCreateProgram();
	glAttachShader(shaderId, fragment_shader);
	glAttachShader(shaderId, vertex_shader);
	glLinkProgram(shaderId);
	glUseProgram(shaderId);

	/*
	 * Set the ucolor & aposition uniform variable.
	 */
	 //ucolor = glGetUniformLocation(program, "color");
	 //aposition = glGetAttribLocation(program, "position");

	glGetProgramiv(shaderId, GL_LINK_STATUS, &is_succeed);
	if (is_succeed == GL_FALSE)
	{
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		throw std::runtime_error("failed to link");
	}

	glEnable(GL_DEPTH_TEST);


	/*
	 * Done all stuff, delete both vertex and fragment shader.
	 */
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

unsigned Shader::GetShaderId()
{
	return shaderId;
}
