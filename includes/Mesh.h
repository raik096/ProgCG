#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#include "BoundingBox.h"

struct Vertex
{
public:
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV;

	glm::vec3 Tangent;
};

class Mesh
{
public:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int IBO;

	GLenum meshMode;
	unsigned int indicesCount;

	BoundingBox boundingBox;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, GLenum meshMode = GL_TRIANGLES);
	Mesh(); //Non mi fa impazzire come cosa
	~Mesh();
};