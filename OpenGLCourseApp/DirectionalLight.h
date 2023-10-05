#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
	DirectionalLight();
	DirectionalLight(
		GLuint shadowWidth, GLuint shadowHeight, 
		GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat aIntensity, GLfloat dIntensity, 
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLuint ambientIntensityLocation, GLuint colorLocation,
					GLuint diffuseIntensityLocation, GLuint directionLocation) const;

	glm::mat4 CalculateLightTransform();
private:
	glm::vec3 direction;
};

