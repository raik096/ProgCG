#pragma once
#include "Renderable.h"

class Terrain
{
public:
	Terrain();
	~Terrain();

	void Generate();
	

	Renderable& RendereableObj();

	glm::vec3 GetPoint(glm::vec3 pos);

private:
	Renderable terrainRenderable;
};