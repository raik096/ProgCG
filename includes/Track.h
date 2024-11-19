#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Renderable.h"

class Track
{
public:
	Track();
	~Track();

	void AddLine(glm::vec3 pointA, glm::vec3 pointB);
	float Length();

	Renderable& RendereableObj();

private:
	std::vector<glm::vec3> curbs[2];
	float length;

	bool isDirty;
	Renderable trackRenderable;

};