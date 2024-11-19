#pragma once
#include <glm/glm.hpp>
#include <algorithm>

class Intersection
{
public:
	static bool Sphere(glm::vec3 o, glm::vec3 d, glm::vec3 center, float radius, glm::vec3& point);
};