#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../vendor/imgui/imgui.h"

#include <vector>

#include "SceneData.h"
#include "Renderable.h"
#include "Camera.h"

#include "BoundingBox.h"
#include "Trackball.h"
#include "Orbit.h"


class Scene
{
private:
	Camera sceneCamera;

	std::vector<Renderable*> sceneObjects;
	Renderable skybox;

	Trackball trackBall;
	Orbit orbit;

	float lastFrame;

public:

	//Scene informations
	SceneData m_SceneData;

	Material* depthMapping; //Non dovrebbe stare qua da spostare, TODO

	bool isControllingLigth;

	Scene();

	void Add(Renderable& r);

	void Start();
	void Update();
	void Draw();

	void SetSkyBox(Renderable& obj);

	Camera& GetCamera();
	BoundingBox CalculateSceneAABB();
};