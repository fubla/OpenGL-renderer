#pragma once
#include <GL/glew.h>

class Material
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shininess);

	void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

	~Material();
private:
	// TODO: Add ambient and diffuse intensities
	GLfloat specularIntensity;
	GLfloat shininess;

};
