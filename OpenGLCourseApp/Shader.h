#pragma once
#include <string>
#include <fstream>

#include <GL/glew.h>

class Shader
{
public:
	Shader();
	void CreateFromString(const char *vertexCode, const char *fragmentCode);
	void CreateFromFiles(const char *vertexLocation, const char *fragmentLocation);

	std::string ReadFile(const char *fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();

	void UseShader();
	void ClearShader();

	~Shader();

private:
	GLuint shaderProgramId, uniformModel, uniformProjection;
	void CompileShader(const char *vertexCode, const char *fragmentCode);
	void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);
};
