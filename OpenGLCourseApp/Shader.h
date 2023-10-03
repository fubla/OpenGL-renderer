﻿#pragma once
#include <string>
#include <fstream>

#include <GL/glew.h>

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

	void UseShader() const;
	void ClearShader();

	~Shader();

private:
	GLuint shaderProgramId, uniformModel, uniformView, uniformProjection, uniformAmbientIntensity, uniformAmbientColor;
	void CompileShader(const char *vertexCode, const char *fragmentCode);
	static void AddShader(GLuint programId, const char* shaderCode, GLenum shaderType);
};
