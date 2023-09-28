#include <cstdio>
#include <cmath>
#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>

#include <string>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
static const char* vShader = "										\n\
#version 330														\n\
layout (location = 0) in vec3 pos;									\n\
																	\n\
uniform mat4 model;													\n\
																	\n\
void main()															\n\
{																	\n\
	gl_Position = model * vec4(pos, 1.0);							\n\
}";

static const char* fShader = "										\n\
#version 330														\n\
out vec4 color;														\n\
																	\n\
void main()															\n\
{																	\n\
	color = vec4(1.0, 0.5, 0.5, 1.0);								\n\
}";

void createTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void addShader(GLuint programId, const char* shaderCode, GLenum shaderType)
{
	GLuint shaderId = glCreateShader(shaderType);
	const GLchar *code = shaderCode;

	GLint codeLength = strlen(shaderCode);

	glShaderSource(shaderId, 1, &code, &codeLength);
	glCompileShader(shaderId);

	GLint result;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		GLchar eLog[1024];
		glGetShaderInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error compiling the " + std::to_string(shaderType) + "shader:" + std::string(eLog));
	}

	glAttachShader(programId, shaderId);
}

void compileShaders()
{
	shader = glCreateProgram();

	if(!shader)
	{
		throw std::runtime_error("Error creating shader program!");
	}

	addShader(shader, vShader, GL_VERTEX_SHADER);
	addShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = {0};

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error linking program: " + std::string(eLog));
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error validating program: " + std::string(eLog));
	}

	uniformModel = glGetUniformLocation(shader, "model");

}

int main() 
{
	if (!glfwInit())
	{
		printf("GLFW Initialization failed!");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Setup GLFW window properties
	// OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core profile -> no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
	if(!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Bind current OpenGL context to window
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if(glewInit() != GLEW_OK)
	{
		printf("GLEW initialize failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);

	try
	{
		createTriangle();
		compileShaders();
	}
	catch (const std::runtime_error &e)
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}
	

	while (!glfwWindowShouldClose(mainWindow))
	{
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if(abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		curAngle += 0.1f;
		if(curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (sizeDirection)
		{
			curSize += 0.001f;

		}
		else
		{
			curSize -= 0.001f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(curAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return EXIT_SUCCESS;
}