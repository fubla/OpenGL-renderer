#include "Material.h"
Material::Material() :
	specularIntensity(0.0f),
	shininess(0.0f)
{}

Material::Material(GLfloat sIntensity, GLfloat shininess) :
	specularIntensity(sIntensity),
	shininess(shininess)
{}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shininessLocation, shininess);
}

Material::~Material() {}
