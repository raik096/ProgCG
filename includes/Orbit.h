#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"

class Orbit
{
public:
	Orbit(Camera& camera);
	~Orbit();

	void MouseDown(int actionType, float x, float y);
	void Update(float x, float y, float delta);;
	void MouseUp(int actionType, float x, float y);

	void UpdateDistance(float amount);

	float GetXAngle();
	float GetYAngle();
	float GetDistance();
	glm::vec3 GetTargetPoint();

	bool IsActive();
private:

	float lastFrame;
	float distance;

	float xCameraAngle;
	float yCameraAngle;

	glm::vec3 targetPoint;

	glm::vec2 oldPos;
	
	bool isRotating;
	bool isMoving;
	Camera& camera;

	void RotationMouseMove(float x, float y, float delta);
	void MoveMouseMove(float x, float y, float delta);
};