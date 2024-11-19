#include "Camera.h"

Camera::Camera(glm::vec3 startPosition, float fov) : position(startPosition), direction(-glm::normalize(startPosition)), m_Fov(fov)
{
	proj = glm::perspective(m_Fov, 16.0f / 9.0f, 0.1f, 300.0f);
	view = glm::lookAt(startPosition, startPosition + direction, glm::vec3(0.f, 1.f, 0.f));
}

void Camera::SetPosition(glm::vec3 point)
{
	position = point;
}

void Camera::Translate(glm::vec3 amount)
{
	position += amount;
}

void Camera::SetDirection(glm::vec3 newDirection)
{
	direction = newDirection;
}

void Camera::DirectionLook(glm::vec3 point)
{
	direction = glm::normalize(point - position);
}

void Camera::RotateAround(glm::vec3 point, float alpha, float beta)
{
	glm::vec3 right = glm::cross(direction, glm::vec3(0, 1, 0));
	glm::vec3 up = glm::cross(direction, right);

	glm::mat4 rotation;
	glm::quat rot = glm::angleAxis(alpha, up) * glm::angleAxis(beta, right);
	rotation = glm::rotate(glm::rotate(glm::mat4(1.f), alpha, up), beta, right);
	//rotation = glm::rotate(glm::mat4(1), beta, glm::vec3(0, 0, 1));


	glm::vec3 pToC = position - point;
	position = (rot * glm::vec4(pToC, 0.0f)) + glm::vec4(point, 1.0f);
}

glm::vec3 Camera::ScreenToWorld(glm::vec2 screenPos)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	glm::vec4 p;//(screenPos.x, screenPos.y, screenPos.z, 1.0f);
	p.x = -1 + (screenPos.x / vp[2]) * (2.0f);
	p.y = -1 + ((vp[3] - screenPos.y) / vp[3]) * (2.0f);
	p.z = 0;
	p.w = 1.0;

	glm::mat4 inv_view = glm::inverse(View());
	return inv_view * p;
}

void Camera::ViewportToRay(glm::vec2 viewPos, glm::vec4& origin, glm::vec4& direction)
{
	glm::mat4 proj_inv = glm::inverse(Proj());

	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	//origin = position;

	glm::vec4 p0;
	glm::vec4 p1;
	p1.x = p0.x = -1.0f + (viewPos.x / vp[2]) * (2.0f);
	p1.y = p0.z = -1.0f + ((vp[3] - viewPos.y) / vp[3]) * (2.0f);
	p1.z = 1;
	p0.z = -1;
	p1.w = p0.w = 1.0f;

	p0 = proj_inv * p0; p0 /= p0.w;
	p1 = proj_inv * p1; p1 /= p1.w;

	origin = p0;
	direction = glm::normalize(p1 - p0);
}

glm::mat4 Camera::Proj()
{
	return proj;
}

glm::mat4 Camera::View()
{
	view = glm::lookAt(position, position + direction, glm::vec3(0.f, 1.f, 0.f));
	return view;
}

glm::vec3 Camera::Right()
{
	return glm::cross(glm::vec3(0, 1, 0), direction);
}
