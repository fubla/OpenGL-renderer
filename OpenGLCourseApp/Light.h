#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, 
		GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity);

	void UseLight(GLint ambientIntensityLocation, GLint colorLocation,
					GLint diffuseIntensityLocation, GLint directionLocation) const;

	~Light();

private:
	glm::vec3 color;
	GLfloat ambientIntensity;

	glm::vec3 direction;
	GLfloat diffuseIntensity;
};
