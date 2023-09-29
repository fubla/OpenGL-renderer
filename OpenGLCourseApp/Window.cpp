#include "Window.h"
#include <cstdlib>
#include <stdexcept>


Window::Window() :
	width(800),
	height(600),
	mouseFirstMoved(true),
	xChange(0.0f),
	yChange(0.0f)
{
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = false;
	}
}

Window::Window(GLint windowWidth, GLint windowHeight) :
	width(windowWidth),
	height(windowHeight),
	mouseFirstMoved(true),
	xChange(0.0f),
	yChange(0.0f)
{
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = false;
	}
}

void Window::initialize()
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
		glfwTerminate();
		throw std::runtime_error("GLFW window creation failed!");
	}

	// Get Buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Bind current OpenGL context to window
	glfwMakeContextCurrent(mainWindow);

	// Handle key and mouse input
	createCallBacks();
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		throw std::runtime_error("GLEW initialize failed!");
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);
}

GLfloat Window::getXChange()
{
	const GLfloat change = xChange;
	xChange = 0.0f;
	return change;
}

GLfloat Window::getYChange()
{
	const GLfloat change = yChange;
	yChange = 0.0f;
	return change;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}

void Window::createCallBacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			thisWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			thisWindow->keys[key] = false;
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (thisWindow->mouseFirstMoved)
	{
		thisWindow->lastX = xPos;
		thisWindow->lastY = yPos;
		thisWindow->mouseFirstMoved = false;
	}

	thisWindow->xChange = xPos - thisWindow->lastX;
	thisWindow->yChange = thisWindow->lastY - yPos;

	thisWindow->lastX = xPos;
	thisWindow->lastY = yPos;
}

