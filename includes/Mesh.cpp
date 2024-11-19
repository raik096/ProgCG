#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, GLenum mode) : meshMode(mode)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &(vertices[0]), GL_STATIC_DRAW);

	/*			 Element 1							Element 2					*/
	/* | {Position: vec3, Normal: vec3} | {Position: vec3, Normal: vec3} | ... */
	/*Stride:  SizeOf(Vertex) */
	/*Offset:    0,     offsetof(Vertex, Normal) = 3*SizeOf(Float)  */

	//Vertex: Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, UV));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Tangent));

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &(indices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	indicesCount = indices.size();

	float xMin = FLT_MAX, yMin = FLT_MAX, zMin = FLT_MAX;
	float xMax = -FLT_MAX, yMax = -FLT_MAX, zMax = -FLT_MAX;
	for (Vertex v : vertices)
	{
		if (v.Position.x < xMin)
			xMin = v.Position.x;
		if (v.Position.x > xMax)
			xMax = v.Position.x;

		if (v.Position.y < yMin)
			yMin = v.Position.y;
		if (v.Position.y > yMax)
			yMax = v.Position.y;

		if (v.Position.z < zMin)
			zMin = v.Position.z;
		if (v.Position.z > zMax)
			zMax = v.Position.z;
	}
	glm::vec3 center = glm::vec3(xMin, yMin, zMin) +
						glm::vec3(xMin, yMin, zMax) +
						glm::vec3(xMin, yMax, zMin) +
						glm::vec3(xMin, yMax, zMax) +
						glm::vec3(xMax, yMin, zMin) +
						glm::vec3(xMax, yMin, zMax) +
						glm::vec3(xMax, yMax, zMin) +
						glm::vec3(xMax, yMax, zMax);

	center /= 8;
	boundingBox = BoundingBox(glm::vec3(xMax-xMin, yMax-yMin, zMax - zMin), center);
}

Mesh::Mesh() : VAO(0), VBO(0), IBO(0), indicesCount(0)
{
}

Mesh::~Mesh()
{
}