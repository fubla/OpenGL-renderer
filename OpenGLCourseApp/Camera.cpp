﻿#include "Camera.h"

#include <GLFW/glfw3.h>


Camera::Camera() = default;

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat moveSpeed, GLfloat turnSpeed) :
	position(position),
	worldUp(up),
	yaw(yaw),
	pitch(pitch),
	moveSpeed(moveSpeed),
	turnSpeed(turnSpeed)
{
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	update();
}

void Camera::keyControl(const bool* keys, GLfloat deltaTime)
{
	const GLfloat velocity = moveSpeed * deltaTime;

	if(keys[GLFW_KEY_W])
	{
		position += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
	}

	if(keys[GLFW_KEY_S])
	{
		position -= glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
	}

	if(keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}

	if(keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}

	if(keys[GLFW_KEY_SPACE])
	{
		position += worldUp * velocity;
	}

	if(keys[GLFW_KEY_LEFT_CONTROL])
	{
		position -= worldUp * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if(pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if(pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

Camera::~Camera()
{
	
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize((glm::cross(front, worldUp)));
	up = glm::normalize(glm::cross(right, front));
}
