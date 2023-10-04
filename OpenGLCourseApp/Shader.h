#pragma once
#include <string>
#include <fstream>

#include <GL/glew.h>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class Shader
{
public:
	Shader();
	void CreateFromString(const char *vertexCode, const char *fragmentCode);
	void CreateFromFiles(const char *vertexLocation, const char *fragmentLocation);

	static std::string ReadFile(const char *fileLocation);

	GLuint GetProjectionLocation() const;
	GLuint GetModelLocation() const;
	GLuint GetViewLocation() const;
	GLuint GetAmbientIntensityLocation() const;
	GLuint GetAmbientColorLocation() const;
	GLuint GetDiffuseIntensityLocation() const;
	GLuint GetDirectionLocation() const;
	GLuint GetSpecularIntensityLocation() const;
	GLuint GetShininessLocation() const;
	GLuint GetEyePositionLocation() const;

	void SetDirectionalLight(DirectionalLight *directionalLight);
	void SetPointLights(PointLight *pLight, GLint lightCount);

	void UseShader() const;
	void ClearShader();

	~Shader();

private:

	GLuint shaderProgramId, uniformModel, uniformView, uniformProjection,
			uniformEyePosition, uniformSpecularIntensity, uniformShininess;

	int pointLightCount;

	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLight[MAX_POINT_LIGHTS];

	void CompileShader(const char *vertexCode, const char *fragmentCode);
	static void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);
};
