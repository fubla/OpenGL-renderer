#include "Window.h"
#include <cstdlib>
#include <stdexcept>


Window::Window() : width(800), height(600) {}

Window::Window(GLint windowWidth, GLint windowHeight) : width(windowWidth), height(windowHeight) {}

int Window::initialize()
{
	if (!glfwInit())
	{
		glfwTerminate();
		throw std::runtime_error("GLFW Initialization failed!");
	}

	// Setup GLFW window properties
	// OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core profile -> no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Get Buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Bind current OpenGL context to window
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialize failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
