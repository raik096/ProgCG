#include "Scene.h"
#include "Texture.h"

#include "ModelLoader.h"
#include "Input.h"
#include "materials/UnlitMaterial.h"

Scene::Scene() : sceneObjects(), sceneCamera(glm::vec3(4, 4, 4), 45.0f), m_SceneData(), 
				trackBall(sceneCamera, glm::vec3(0.0f), 12.0f), orbit(sceneCamera)
{
	isControllingLigth = false;

	m_SceneData.ligth = new Ligth();
	m_SceneData.lightDirection = glm::vec3(-1, -1, -1);
	m_SceneData.lightColor = new float[3] { 1.0f, 1.0f, 0.89f };
	m_SceneData.ambientColor = new float[3] { 0.22f, 0.32f, 0.43f };
	m_SceneData.enableShadows = true;

	Shader* depthShader = new Shader("shaders/depthmap.vert", "shaders/depthmap.frag");
	depthMapping = new Material(depthShader);

	Texture skyDomeTexture("resources/sky.jpg");
	Shader* skyDomeShader = new Shader("shaders/unlit");
	UnlitMaterial* skyDomeMat = new UnlitMaterial(skyDomeShader, skyDomeTexture);

	skybox = ModelLoader::Load("./models/skydome.glb", skyDomeMat);
	skybox.Scale(200);
}

void Scene::Add(Renderable& r)
{
	sceneObjects.push_back(&r);
}

void Scene::Start()
{
	m_SceneData.ligth->SetProjMatrix(CalculateSceneAABB());
	sceneCamera.SetPosition(glm::vec3(-5, 2, 3));

	for (Renderable* r : sceneObjects) 
	{
		r->Load();
	}
}

void Scene::Update()
{
	skybox.SetPosition(sceneCamera.position);

	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (isControllingLigth)
	{
		if (Input::GetMousePressed(0))
		{
			glm::vec2 mousePos = Input::MousePosition();
			trackBall.MouseDown(mousePos.x, mousePos.y);
		}

		if (Input::GetMouseReleased(0))
		{
			glm::vec2 mousePos = Input::MousePosition();
			trackBall.MouseUp(mousePos.x, mousePos.y);
		}

		if (trackBall.IsActive())
		{
			glm::vec2 mousePos = Input::MousePosition();
			trackBall.MouseMove(mousePos.x, mousePos.y);
			m_SceneData.ligth->SetRotation(trackBall.matrix());
		}
	}
	else {
		glm::vec2 mousePos = Input::MousePosition();
		float scroll = Input::GetMouseScroll();

		if (Input::GetMousePressed(0)) {
			orbit.MouseDown(0, mousePos.x, mousePos.y);
		}
		else if (Input::GetMousePressed(1))
		{
			orbit.MouseDown(1, mousePos.x, mousePos.y);
		}
		

		if (Input::GetMouseReleased(0)) {
			orbit.MouseUp(0,mousePos.x, mousePos.y);
		}
		else if (Input::GetMouseReleased(1))
		{
			orbit.MouseUp(1,mousePos.x, mousePos.y);
		}

		if(orbit.IsActive())
		{
			glm::vec2 mousePos = Input::MousePosition();
			orbit.Update(mousePos.x, mousePos.y, deltaTime);
		}
		else if (scroll != 0)
		{
			orbit.UpdateDistance(scroll * 100 * deltaTime);
		}
	}

	Input::SetCursorVisibility(!orbit.IsActive());
}

void Scene::Draw()
{
	if (m_SceneData.enableShadows)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		//Shadow pass
		m_SceneData.ligth->fbo.Bind();
		glViewport(0, 0, m_SceneData.ligth->fbo.Width(), m_SceneData.ligth->fbo.Heigth());
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		for (Renderable* r : sceneObjects) {

			r->Draw(m_SceneData, m_SceneData.ligth->Proj(), m_SceneData.ligth->View(), depthMapping);
		}

		m_SceneData.ligth->fbo.Unbind();

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	//Render pass
	glDepthMask(GL_FALSE);
	skybox.Draw(m_SceneData, sceneCamera.Proj(), sceneCamera.View());
	glDepthMask(GL_TRUE);

	for (Renderable* r : sceneObjects) {
		/*
		glUseProgram(r->GetShader().program);

		glm::vec3 lightDir = glm::normalize(lightDirection);
		glUniform3f(r->GetShader()["uLightDir"], lightDir.x, lightDir.y, lightDir.z);
		glUniform3fv(r->GetShader()["uLightColor"], 1, lightColor);
		glUniform3fv(r->GetShader()["uAmbientColor"], 1, ambientColor);
		*/

		r->Draw(m_SceneData, sceneCamera.Proj(), sceneCamera.View());
	}
}
void Scene::SetSkyBox(Renderable& obj)
{
	skybox = obj;
}

Camera& Scene::GetCamera()
{
	return sceneCamera;
}

BoundingBox Scene::CalculateSceneAABB()
{
	float left = FLT_MAX, bot = FLT_MAX, back = FLT_MAX;
	float right = -FLT_MAX, top = -FLT_MAX, front = -FLT_MAX;

	glm::vec3 center = glm::vec3(0);

	for (Renderable* r : sceneObjects)
	{
		if (r->GetBoundingBox().Left() < left)
			left = r->GetBoundingBox().Left();

		if (r->GetBoundingBox().Rigth() > right)
			right = r->GetBoundingBox().Rigth();

		if (r->GetBoundingBox().Bottom() < bot)
			bot = r->GetBoundingBox().Bottom();

		if (r->GetBoundingBox().Top() > top)
			top = r->GetBoundingBox().Top();

		if (r->GetBoundingBox().Back() < back)
			back = r->GetBoundingBox().Back();

		if (r->GetBoundingBox().Front() > front)
			front = r->GetBoundingBox().Front();

		center += r->GetBoundingBox().Center();
	}
	center /= sceneObjects.size();

	std::cout << right << " " << top << " " << front << std::endl;
	return BoundingBox(glm::vec3(right - left, top - bot, front - back), center);
}
