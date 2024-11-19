#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	float m_Fov;

	Camera(glm::vec3 startPosition, float fov);

	void SetPosition(glm::vec3 point);

	void Translate(glm::vec3 amount);

	void SetDirection(glm::vec3 newDirection);

	void DirectionLook(glm::vec3 point);

	void RotateAround(glm::vec3 point, float alpha, float beta);

	glm::vec3 ScreenToWorld(glm::vec2 screenPos);

	void ViewportToRay(glm::vec2 viewPos, glm::vec4& origin, glm::vec4& direction);

	glm::mat4 Proj();

	glm::mat4 View();

	glm::vec3 Right();

private:
	/*Scene View Data */
	/* projection matrix*/
	glm::mat4 proj;

	/* view matrix */
	glm::mat4 view;

};