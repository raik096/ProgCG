#pragma once
#include "Intersection.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"

class Trackball
{
public:
	Trackball(Camera& camera, glm::vec3 center, float radius);
	~Trackball();

	void MouseDown(float x, float y);
	void MouseMove(float x, float y);
	void MouseUp(float x, float y);

	glm::mat4 matrix();

	bool IsActive();

private:
	Camera& camera;

	bool trackballActive;
	float last_mX, last_mY;
	float curr_mX, curr_mY;

	glm::vec3 lastP;
	glm::vec3 currP;

	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;

	float radius;
	glm::vec3 center;
	glm::vec3 get_trackball_vector(int x, int y);
	bool SphereIntersection(float x, float y, glm::vec3& int_p);
};