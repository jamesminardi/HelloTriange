#pragma once

#include <glad/glad.h> // Get OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	// program ID
	unsigned int id;

	// Constructor (reads and builds shader)
	Shader(const char* vertex_path, const char* fragment_path);

	// Activate the shader (use)
	void use();

	// Utility uniform functions
	void set(const std::string& name, bool value) const;
	void set(const std::string& name, int value) const;
	void set(const std::string& name, float value) const;
};
