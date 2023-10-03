#include "Light.h"

Light::Light() :
	color(glm::vec3(1.0f)),
	ambientIntensity(1.0f)
{}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity) :
	color(glm::vec3(red, green, blue)),
	ambientIntensity(aIntensity)
{}

void Light::UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation) const
{
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
}

Light::~Light() = default;
