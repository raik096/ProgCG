#pragma once

#include <glm/ext.hpp>

#include "Mesh.h"
#include "materials/Material.h"

class Renderable
{
private:
	
	Material* m_Material;

	std::vector<Mesh> m_Meshes;
	std::vector<Renderable> m_Childrens;
	
	glm::mat4 position;
	glm::mat4 rotation;
	glm::mat4 scale;
	glm::mat4 model;

	bool dirtyMatrix;

	BoundingBox boundingBox;
	// transformation matrix
	//glm::mat4 transform;

	void RecalculateBoundingBox();

public:
	Renderable();
	
	Renderable(Mesh mesh, Material* material);

	Renderable(std::vector<Mesh> meshes, Material* material);

	void AddChild(const Renderable& child);

	void SetPosition(glm::vec3 point);

	glm::vec3 GetPosition();

	void Translate(glm::vec3 amount);

	void Scale(float amount);

	void SetRotation(glm::vec3 rotationAngles);

	void SetRotationMatrix(glm::mat4 rotMatrix);

	void Rotate(glm::vec3 axis, float angle);

	virtual void Load() {};

	void Draw(SceneData& scene, glm::mat4 proj, glm::mat4 view, Material* overrideMat, glm::mat4 parentModel = glm::mat4(1));

	void Draw(SceneData& scene, glm::mat4 proj, glm::mat4 view, glm::mat4 parentModel = glm::mat4(1));

	Material* GetMaterial();

	BoundingBox& GetBoundingBox();
	
};