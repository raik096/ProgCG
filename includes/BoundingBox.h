#pragma once

#include <glm/glm.hpp>

class BoundingBox
{
public:
	BoundingBox(glm::vec3 size = glm::vec3(0.0f), glm::vec3 center = glm::vec3(0.0f));
	~BoundingBox();

	glm::vec3 Center();

	float Left();
	float Rigth();
	float Top();
	float Bottom();
	float Front();
	float Back();

	glm::vec3& Size();

private:
	glm::vec3 m_Size;
	glm::vec3 m_Center;

};
