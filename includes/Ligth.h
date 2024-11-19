#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "FrameBuffer.h"

#include "BoundingBox.h"

class Ligth
{
public:
	FrameBuffer fbo;
	glm::vec3 position;
	glm::vec3 direction;
	float maxDistance;
	float ligthBias;

	Ligth(glm::vec3 direction = glm::vec3(-1, -1, -1), float maxDistance = 75.0f);
	~Ligth();

	glm::mat4 View();
	glm::mat4 Proj();

	glm::mat4 LigthMatrix();

	void SetViewMatrix(glm::mat4 proj);
	void SetProjMatrix(BoundingBox sceneBounds);

	void SetRotation(glm::mat4 rotation);

private:
	/* projection matrix*/
	glm::mat4 proj;

	/* view matrix */
	glm::mat4 view;

	glm::mat4 rotationMatrix;
};