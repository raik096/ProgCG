#include "Intersection.h"

bool Intersection::Sphere(glm::vec3 o, glm::vec3 d, glm::vec3 center, float radius, glm::vec3& point)
{
	glm::vec3 oc = o - center;
	float A = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
	float B = 2 * glm::dot(d, oc);
	float C = glm::dot(oc, oc) - radius * radius;

	float dis = B * B - 4 * A * C;

	if (dis > 0) {
		float t0 = (-B - sqrt(dis)) / (2 * A);
		float t1 = (-B + sqrt(dis)) / (2 * A);
		float t = std::min<float>(t0, t1);
		point = o + glm::vec3(t * d[0], t * d[1], t * d[2]);
		return true;
	}
	return false;
}
