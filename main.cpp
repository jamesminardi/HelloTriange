#include <iostream>
#include <string>
#include <cstdint>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

GLFWwindow* win;

const char* vertex_shader_source = "#version 460 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"	ourColor = aColor;\n" // Set ourColor to the input color we got from the vertex data
	"}\0";

const char* fragment_shader_source = "#version 460 core\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"void main()\n"
	"{\n"
	"   FragColor = vec4(ourColor, 1.0);\n"
	"}\0";

unsigned int shader_program; // ID Reference to shader program containing defined vertex and fragment shaders

// gl:	configure buffer.
//	  - store vertex data in memory of graphics
//		card managed by buffer object VBO.
// --------------------------------------------
float vertices[] = {
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, 0.5f, 0.0f,  // top left
	0.5f, 0.5f, 0.0f,   // top right
	-0.5f, -0.5f, 0.0f, // bottom left
};

unsigned int indices[] = {
	0, 1, 2,
	0, 1, 3
};

float vertices_one[] = {
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
	0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // middle top
};

unsigned int indices_one[] = {
	0, 1, 2
};


void process_input(GLFWwindow* window);
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* msg);
int check_shader_error(unsigned int vertex_shader, bool is_shader_program);

/**
 * Initialized GLFW window and GLAD openGL functions
 */
int init()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Applied to next call of create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // For Debugging
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API); // ?
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw: window creation
	// --------------------
	// Width, Height, Monitor to fullscreen, Window to share resources with
	win = glfwCreateWindow(800, 600, "Hello, World!", nullptr, nullptr);
	if (!win)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	// Make GLFW call this function when window changes size
	glfwSetFramebufferSizeCallback(win, frame_buffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 0;
}


int shaders_init()
{
	const uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	// Similar to glGenBuffers, gives unique ID to vertexShader
	glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr); // Attach shader source code to shader object
	glCompileShader(vertex_shader);                                   // Compile vertex shader source code

	if (check_shader_error(vertex_shader, false) != 0)
	{
		return -1;
	}

	const uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	// Similar to glGenBuffers, gives unique ID to vertexShader
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr); // Attach shader source code to shader object
	glCompileShader(fragment_shader);                                     // Compile vertex shader source code

	if (check_shader_error(fragment_shader, false) != 0)
	{
		return -1;
	}

	// Shader Program
	// --------------
	shader_program = glCreateProgram(); // Creates program object and returns ID reference
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	if (check_shader_error(shader_program, true) != 0)
	{
		return -1;
	}

	glUseProgram(shader_program); // Every shader and rendering call now uses this program object (and thus the shaders)

	// Don't need shader objects after linking
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return 0;
}

int main(int argc, char** argv)
{
	// Init
	if (init() != 0)
	{
		return -1;
	}


	// GLSL: vertex & fragment shader setup
	// -------------------
	if (shaders_init() != 0)
	{
		return -1;
	}

	/*
	 * Vertex data has been given to GPU and told the GPU how to process the
	 * data within a vertex and fragment shader. Now must tell OpenGL how to
	 * interpret the vertex data in memory and how it should connect vertex data
	 * to vertex shader's attributes.
	 * Link the vertex data in memory to the shaders.
	 */
	// Generate unique ID for buffer object
	unsigned int vbo;
	glGenBuffers(1, &vbo); // Generate buffer object ID and store it in VBO. (Optionally >1 and give array as 2nd arg)
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// Linking Vertex Attributes
	// -------------------------

	// Vertices array:
	// 3 vertices, xyz each, each float is 4 bytes
	// 36 bytes total, 12B per vertex, 4 per attrib

	// 0: Vertex shader said position vertex attrib is at location=0 (Set location of vertex attrib to 0)
	// 3: Size of the vertex attrib (vec3=3 values)
	// GL_FLOAT: type of the data
	// GL_FALSE: Do we want to normalize the data? Not relevant rn
	// 3*sizeof(float): Space between consecutive vertex attribs (A set of position data is 3 times the size of a float away)
	//		-> Or give it 0, to let OpenGL determine stride but only works for tightly packed values
	// (void*)0: Offset of where position data begins in buffer. 0 for now
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0); // Give vertex attrib location

	// VBO is still bound so vertex attrib 0 is now associated with its vertex data

	// Vertex Array Object
	// Bound just like a vertex buffer object, and any subsequent
	// calls with be stored in the VAO. Reduces code duplication since
	// you only have to make those calls once, can just bind the 
	// corresponding VAO for the set of bindings. (Typically required too)
	// -------------------------------------------------------------------


	// ..:: Initialization code done only once unless object frequently changes ::..
	// 1. Bind VAO
	// 2. Copy vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_one), vertices_one, GL_STATIC_DRAW);
	// Give buffer object bound to GL_ARRAY_BUFFER the vertex data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_one), indices_one, GL_STATIC_DRAW);

	// 3. Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
	                      /* Stride:*/6 * sizeof(float),
	                      /* Offset:*/(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
	                      /* Stride:*/6 * sizeof(float),
	                      /* Offset:*/(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	/*
	 * First Generate/configure all VAOs (and required VBO and attrib pointers) and store for later.
	 * To draw, take the corresponding VAO, bind it, then daw the object and unbind the VAO again.
	 *
	 */

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(win)) // Checks internal close flag that is set in processInput()
	{
		// input
		process_input(win);

		// Rendering commands
		// ------------------
		// Sets clear color and clears screen in buffer
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f); // Sets context's clear color attrib
		glClear(GL_COLOR_BUFFER_BIT);         // Uses context's clear color attrib


		// Rendering commands ...
		// To draw object now, only have to use these with the VAO initialized:
		glUseProgram(shader_program);


		//const auto time_value = (float)glfwGetTime();
		//const float red_value = sin(time_value) / 2.0f + 0.5f;
		//const float green_value = sin(time_value * 1.3) / 2.0f + 0.5f;
		//const float blue_value = sin(time_value * 1.8) / 2.0f + 0.5f;
		//const GLint color = glGetUniformLocation(shader_program, "newColor");
		//glUniform4f(color, red_value, green_value, blue_value, 1.0f);

		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 6); // 0-Starting index, 3-# of vertices
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0); // Unbind the vertex array

		// Check call events and swap buffers
		glfwSwapBuffers(win);
		glfwPollEvents(); // If any events are triggered, call corresponding callback functions
	}

	glfwDestroyWindow(win);
	glfwTerminate();
	return 0;
}

/**
 * glfw: process input in the GLFW window
 */
void process_input(GLFWwindow* window)
{
	// Close the window if ESC is currently pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		std::cout << "Info: ESC Pressed" << std::endl;
		glfwSetWindowShouldClose(window, true); // Sets internal close flag
	}
}

/**
 * glfw: whenever the window size changes this callback function executes
 */
void frame_buffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void error_callback(const int error, const char* msg)
{
	std::string s;
	s = " [" + std::to_string(error) + "] " + msg;
	std::cerr << s << std::endl;
}

int check_shader_error(const unsigned int vertex_shader, const bool is_shader_program)
{
	int success;
	char info_log[512];
	if (!is_shader_program)
	{
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
			std::cout << "Error: Shader Compilation FAILED :: \n" << info_log << std::endl;
			return -1;
		}
	}
	else
	{
		glGetProgramiv(vertex_shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(vertex_shader, 512, nullptr, info_log);
			std::cout << "Error: Shader Program Linking FAILED :: \n" << info_log << std::endl;
			return -1;
		}
	}

	return 0;
}
