#include "Trackball.h"

Trackball::Trackball(Camera& camera, glm::vec3 center, float radius) : camera(camera), center(center), radius(radius)
{
	lastP = glm::vec3(0.0f);
	currP = glm::vec3(0.0f);

	trackballActive = false;

	translationMatrix = glm::translate(glm::mat4(1.0f), center);
	rotationMatrix = glm::mat4(1.0f);
}

Trackball::~Trackball()
{
}

void Trackball::MouseDown(float x, float y)
{
	glm::vec3 int_point;
	if (SphereIntersection(x, y, int_point)) {
		lastP = currP = int_point;
		trackballActive = true;
	}
}

void Trackball::MouseMove(float x, float y)
{
	if (!trackballActive) return;

	if (SphereIntersection(x, y, currP)) {
		glm::vec3 rotationVector = glm::cross(glm::normalize(lastP), glm::normalize(currP));

		if (glm::length(rotationVector) > 0.01f)
		{
			float alpha = glm::asin(glm::length(rotationVector));
			glm::mat4 delta_rot = glm::rotate(glm::mat4(1.f), alpha, rotationVector);

			rotationMatrix = delta_rot * rotationMatrix;

			lastP = currP;
		}
	}

}

void Trackball::MouseUp(float x, float y)
{
	trackballActive = false;
}

glm::mat4 Trackball::matrix()
{
	return translationMatrix * rotationMatrix;
}

bool Trackball::IsActive()
{
	return trackballActive;
}

bool Trackball::SphereIntersection(float x, float y, glm::vec3& int_p)
{
	glm::vec4 origin, direction;
	camera.ViewportToRay(glm::vec2(x, y), origin, direction);

	glm::mat4 view_frame = glm::inverse(camera.View());

	origin = view_frame * origin;
	direction = view_frame * direction;

	bool hit = Intersection::Sphere(origin, direction, center, radius, int_p);
	if (hit)
		int_p -= center;

	return hit;
}
