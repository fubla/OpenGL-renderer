#include "Shader.h"


Shader::Shader() :
	shaderProgramId(0),
	uniformModel(0),
	uniformProjection(0),
	pointLightCount(0),
	spotLightCount(0)
{}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	const std::string vertexString = ReadFile(vertexLocation);
	const std::string fragmentString = ReadFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		throw std::runtime_error("Failed to read " + std::string(fileLocation) + "!");
	}

	std::string line;
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}


void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	shaderProgramId = glCreateProgram();

	if (!shaderProgramId)
	{
		throw std::runtime_error("Error creating " + std::to_string(shaderProgramId) + " program!");
	}

	AddShader(shaderProgramId, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderProgramId, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error linking program: " + std::string(eLog));
	}

	glValidateProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error validating program: " + std::string(eLog));
	}

	uniformModel = glGetUniformLocation(shaderProgramId, "model");
	uniformView = glGetUniformLocation(shaderProgramId, "view");
	uniformProjection = glGetUniformLocation(shaderProgramId, "projection");
	uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderProgramId, "directionalLight.base.color");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderProgramId, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderProgramId, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderProgramId, "directionalLight.base.diffuseIntensity");
	uniformSpecularIntensity = glGetUniformLocation(shaderProgramId, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderProgramId, "material.shininess");
	uniformEyePosition = glGetUniformLocation(shaderProgramId, "eyePos");

	uniformPointLightCount = glGetUniformLocation(shaderProgramId, "pointLightCount");

	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		char locBuf[100] = { '\0' };
		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].base.color", i);
		uniformPointLight[i].uniformColor = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderProgramId, locBuf);
	}

	uniformSpotLightCount = glGetUniformLocation(shaderProgramId, "spotLightCount");

	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
	{
		char locBuf[100] = { '\0' };
		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.base.color", i);
		uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderProgramId, locBuf);

		snprintf(locBuf, sizeof(locBuf), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderProgramId, locBuf);
	}


}

GLuint Shader::GetProjectionLocation() const
{
	return uniformProjection;
}

GLuint Shader::GetModelLocation() const
{
	return uniformModel;
}

GLuint Shader::GetViewLocation() const
{
	return uniformView;
}

GLuint Shader::GetAmbientIntensityLocation() const
{
	return uniformDirectionalLight.uniformAmbientIntensity;
}

GLuint Shader::GetAmbientColorLocation() const
{
	return uniformDirectionalLight.uniformColor;
}

GLuint Shader::GetDiffuseIntensityLocation() const
{
	return uniformDirectionalLight.uniformDiffuseIntensity;
}
GLuint Shader::GetDirectionLocation() const
{
	return uniformDirectionalLight.uniformDirection;
}

GLuint Shader::GetSpecularIntensityLocation() const
{
	return uniformSpecularIntensity;
}

GLuint Shader::GetShininessLocation() const
{
	return uniformShininess;
}

GLuint Shader::GetEyePositionLocation() const
{
	return uniformEyePosition;
}

void Shader::SetDirectionalLight(DirectionalLight* directionalLight)
{
	directionalLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColor,
		uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::SetPointLights(PointLight* pLight, GLuint lightCount)
{
	if (lightCount > MAX_POINT_LIGHTS)
	{
		lightCount = MAX_POINT_LIGHTS;
	}

	glUniform1i(uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++)
	{
		pLight[i].UseLight(
			uniformPointLight[i].uniformAmbientIntensity, uniformPointLight[i].uniformColor,
			uniformPointLight[i].uniformDiffuseIntensity, uniformPointLight[i].uniformPosition,
			uniformPointLight[i].uniformConstant, uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent);
	}
}

void Shader::SetSpotLights(SpotLight* sLight, GLuint lightCount)
{
	if (lightCount > MAX_SPOT_LIGHTS)
	{					 
		lightCount = MAX_SPOT_LIGHTS;
	}

	glUniform1i(uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++)
	{
		sLight[i].UseLight(
			uniformSpotLight[i].uniformAmbientIntensity, uniformSpotLight[i].uniformColor,
			uniformSpotLight[i].uniformDiffuseIntensity, uniformSpotLight[i].uniformPosition, uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear, uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);
	}
}

void Shader::UseShader() const
{
	glUseProgram(shaderProgramId);
}


void Shader::ClearShader()
{
	if (shaderProgramId != 0)
	{
		glDeleteProgram(shaderProgramId);
		shaderProgramId = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;
}

void Shader::AddShader(const GLuint programId, const char* shaderCode, const GLenum shaderType)
{
	const GLuint shaderId = glCreateShader(shaderType);
	const GLchar* code = shaderCode;

	const GLint codeLength = strlen(shaderCode);

	glShaderSource(shaderId, 1, &code, &codeLength);
	glCompileShader(shaderId);

	GLint result;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		GLchar eLog[1024];
		glGetShaderInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error compiling the " + std::to_string(shaderType) + " shader:" + std::string(eLog));
	}

	glAttachShader(programId, shaderId);
}


Shader::~Shader()
{
	ClearShader();
}
