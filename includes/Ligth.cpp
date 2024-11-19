#include "Ligth.h"

Ligth::Ligth(glm::vec3 direction, float maxDistance) : proj(), view(), fbo(2048, 2048),
						direction(glm::normalize(direction)), maxDistance(maxDistance), position(-direction * maxDistance/2.0f),
						ligthBias(0.005f)
{
	//In teoria questa dovrebbe essere grande abbastanza per coprire tutta la scena
	//TODO: capire che valori ficcarci qui dentro

	proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.f, maxDistance * 2.0f);
	view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	rotationMatrix = glm::mat4(1.0f);
}

Ligth::~Ligth()
{
	fbo.~FrameBuffer();
}

glm::mat4 Ligth::View()
{
	view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)) * rotationMatrix;
	return view;
}

glm::mat4 Ligth::Proj()
{
	proj = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.f, maxDistance*2.0f);
	return proj;
}

glm::mat4 Ligth::LigthMatrix()
{
	return Proj() * View();
}

void Ligth::SetViewMatrix(glm::mat4 viewMatrix)
{
	view = viewMatrix;
}

void Ligth::SetProjMatrix(BoundingBox sceneBounds)
{
	proj = glm::ortho(-sceneBounds.Size().x*2, sceneBounds.Size().x * 2, -sceneBounds.Size().y * 2, sceneBounds.Size().y * 2, 0.f, maxDistance * 2.0f);
}

void Ligth::SetRotation(glm::mat4 rotation)
{
	rotationMatrix = rotation;

	View();
	glm::vec3 rotatedPos = glm::inverse(rotation) * glm::vec4(position, 0);
	direction = glm::normalize(-rotatedPos);
}