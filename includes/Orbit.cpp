#include "Orbit.h"

Orbit::Orbit(Camera& camera): camera(camera)
{
	isRotating = false;
	isMoving = false;
	distance = 10;
	xCameraAngle = 0;
	yCameraAngle = 45;
	targetPoint = glm::vec3(0, 0, 0);
	oldPos = glm::vec3(0, 0, 0);

	camera.DirectionLook(targetPoint);
}

Orbit::~Orbit()
{
}

void Orbit::MouseDown(int actionType, float x, float y)
{
	oldPos = glm::vec2(x, y);
	
	if (actionType == 1)
		isMoving = true;
	else
		isRotating = true;
}

void Orbit::Update(float x, float y, float delta)
{
	if (isMoving)
		MoveMouseMove(x, y, delta);
	else
		RotationMouseMove(x, y, delta);
}

void Orbit::MouseUp(int actionType, float x, float y)
{
	if (actionType == 1)
		isMoving = false;
	else
		isRotating = false;
}

void Orbit::MoveMouseMove(float x, float y, float delta)
{

	float fMovement = (y - oldPos.y) * delta;
	float lMovement = (x - oldPos.x) * delta;

	glm::vec3 fwd = camera.direction;
	fwd.y = 0;
	fwd = glm::normalize(fwd);

	glm::vec3 movement = (fwd * fMovement) + (camera.Right() * lMovement);

	targetPoint += movement;
	//targetPoint.x -= fMovement;
	//targetPoint.z += lMovement;

	camera.Translate(movement);
	oldPos = glm::vec2(x, y);
}

void Orbit::RotationMouseMove(float x, float y, float delta)
{
	xCameraAngle = (x - oldPos.x) * 10 * delta;
	yCameraAngle = -(y - oldPos.y) * 10 * delta;

	camera.RotateAround(targetPoint, glm::radians(xCameraAngle), glm::radians(yCameraAngle));
	camera.DirectionLook(targetPoint);

	oldPos = glm::vec2(x, y);
}

void Orbit::UpdateDistance(float amount)
{
	distance -= amount;
	if (distance < 0.1f) distance = 0.1f;
	if (distance > 100) distance = 100;

	glm::vec3 camPos = targetPoint - (camera.direction * distance);
	camera.SetPosition(camPos);
}

float Orbit::GetXAngle()
{
	return xCameraAngle;
}

float Orbit::GetYAngle()
{
	return yCameraAngle;
}

float Orbit::GetDistance()
{
	return distance;
}

glm::vec3 Orbit::GetTargetPoint()
{
	return targetPoint;
}

bool Orbit::IsActive()
{
	return isRotating || isMoving;
}
