#pragma once
#include <GL/glew.h>
class Mesh
{
public:
	Mesh();

	void CreateMesh(const GLfloat *vertices, const unsigned int *indices, GLsizei numOfVertices, GLsizei numOfIndices);
	void RenderMesh() const;
	void ClearMesh();

	~Mesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

