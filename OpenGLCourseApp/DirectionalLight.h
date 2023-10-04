#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, 
		GLfloat dIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLint ambientIntensityLocation, GLint colorLocation,
					GLint diffuseIntensityLocation, GLint directionLocation) const;
private:
	glm::vec3 direction;
};

