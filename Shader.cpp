#include "Shader.h"

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
	// 1. retrieve the vertex/fragment source code from file path
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream vertex_shader_file;
	std::ifstream fragment_shader_file;

	// Ensure if stream objects can throw exceptions
	vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vertex_shader_file.open(vertex_path);
		fragment_shader_file.open(fragment_path);
		std::stringstream vertex_shader_stream, fragment_shader_stream;

		// read file's buffer contents into streams
		vertex_shader_stream << vertex_shader_file.rdbuf();
		fragment_shader_stream << fragment_shader_file.rdbuf();

		// close file handlers
		vertex_shader_file.close();
		fragment_shader_file.close();

		// convert stream into string
		vertex_code   = vertex_shader_stream.str();
		fragment_code = fragment_shader_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_FOUND_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vertex_shader_code   = vertex_code.c_str();
	const char* fragment_shader_code = fragment_code.c_str();


	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char info_log[512];

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
	glCompileShader(vertex);

	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
	glCompileShader(fragment);

	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}


	// shader program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	// print linking errors if any
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 512, nullptr, info_log);
		std::cout << "ERROR:SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// delete shaders as they're linked in the program and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::set(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::set(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
