#include "Shader.h"


Shader::Shader() : shaderProgramId(0), uniformModel(0), uniformProjection(0) {}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	const std::string vertexString = ReadFile(vertexLocation);
	const std::string fragmentString = ReadFile(fragmentLocation);

	const char *vertexCode = vertexString.c_str();
	const char *fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if(!fileStream.is_open())
	{
		throw std::runtime_error("Failed to read " + std::string(fileLocation) + "!");
	}

	std::string line;
	while(!fileStream.eof())
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

	if(!shaderProgramId)
	{
		throw std::runtime_error("Error creating " + std::to_string(shaderProgramId) + " program!");
	}

	AddShader(shaderProgramId, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderProgramId, fragmentCode, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = {0};

	glLinkProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error linking program: " + std::string(eLog));
	}

	glValidateProgram(shaderProgramId);
	glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &result);
	if(!result)
	{
		glGetProgramInfoLog(shaderProgramId, sizeof(eLog), nullptr, eLog);
		throw std::runtime_error("Error validating program: " + std::string(eLog));
	}

	uniformModel = glGetUniformLocation(shaderProgramId, "model");
	uniformView = glGetUniformLocation(shaderProgramId, "view");
	uniformProjection = glGetUniformLocation(shaderProgramId, "projection");
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

void Shader::UseShader() const
{
	glUseProgram(shaderProgramId);
}


void Shader::ClearShader()
{
	if(shaderProgramId != 0)
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
	const GLchar *code = shaderCode;

	const GLint codeLength = strlen(shaderCode);

	glShaderSource(shaderId, 1, &code, &codeLength);
	glCompileShader(shaderId);

	GLint result;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if(!result)
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
