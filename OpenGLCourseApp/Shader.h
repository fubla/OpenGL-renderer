#pragma once
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader
{
public:
	Shader();
	void CreateFromString(const char *vertexCode, const char *fragmentCode);
	void CreateFromFiles(const char *vertexLocation, const char *fragmentLocation);
	void CreateFromFiles(const char *vertexLocation, const char *geometryLocation, const char *fragmentLocation);

	void Validate();

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
	GLuint GetOmniLightPosLocation() const;
	GLuint GetFarPlaneLocation() const;

	void SetDirectionalLight(DirectionalLight *directionalLight);
	void SetPointLights(PointLight *pLight, GLuint lightCount, unsigned textureUnit, unsigned offset);
	void SetSpotLights(SpotLight *sLight, GLuint lightCount, unsigned textureUnit, unsigned offset);
	void SetTexture(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4 *lTransform);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	void UseShader() const;
	void ClearShader();

	~Shader();

private:

	GLuint shaderProgramId, uniformModel, uniformView, uniformProjection,
			uniformEyePosition, uniformSpecularIntensity, uniformShininess,
			uniformTexture,
			uniformDirectionalLightTransform, uniformDirectionalShadowMap,
			uniformOmniLightPos, uniformFarPlane;

	GLuint uniformLightMatrices[6];
	int pointLightCount, spotLightCount;

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

	GLuint uniformSpotLightCount;

	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	struct
	{
		GLuint shadowMap;
		GLuint farPlane;
	} uniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	void CompileShader(const char *vertexCode, const char *fragmentCode);
	void CompileShader(const char *vertexCode, const char *geometryCode, const char *fragmentCode);
	static void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);

	void CompileProgram();
};
