#include "Renderable.h"
#include <iostream>


void Renderable::RecalculateBoundingBox()
{
	float left = FLT_MAX, bot = FLT_MAX, back = FLT_MAX;
	float right = -FLT_MAX, top = -FLT_MAX, front = -FLT_MAX;

	glm::vec3 center = glm::vec3(0);

	for (Mesh m : m_Meshes)
	{
		if (m.boundingBox.Left() < left)
			left = m.boundingBox.Left();

		if (m.boundingBox.Rigth() > right)
			right = m.boundingBox.Rigth();

		if (m.boundingBox.Bottom() < bot)
			bot = m.boundingBox.Bottom();

		if (m.boundingBox.Top() > top)
			top = m.boundingBox.Top();

		if (m.boundingBox.Back() < back)
			back = m.boundingBox.Back();

		if (m.boundingBox.Front() > front)
			front = m.boundingBox.Front();

		center += m.boundingBox.Center();
	}
	center /= m_Meshes.size();
	boundingBox = BoundingBox(glm::vec3(right-left, top-bot, front-back), GetPosition() + center);
}

Renderable::Renderable(): m_Meshes(), m_Material(), m_Childrens(), dirtyMatrix(true),
position(glm::mat4(1.0f)), rotation(glm::mat4(1.0f)), scale(glm::mat4(1.0f)), model(glm::mat4(1.0f))
{
	RecalculateBoundingBox();
}

Renderable::Renderable(Mesh mesh, Material* material) : m_Material(material), m_Meshes({ mesh }), m_Childrens(), dirtyMatrix(true),
position(glm::mat4(1.0f)), rotation(glm::mat4(1.0f)), scale(glm::mat4(1.0f)), model(glm::mat4(1.0f))
{
	RecalculateBoundingBox();
}

Renderable::Renderable(std::vector<Mesh> meshes, Material* material) : m_Material(material), m_Meshes(meshes), m_Childrens(),
dirtyMatrix(true), position(glm::mat4(1.0f)), rotation(glm::mat4(1.0f)), scale(glm::mat4(1.0f)), model(glm::mat4(1.0f))
{
	RecalculateBoundingBox();
}

void Renderable::AddChild(const Renderable& child)
{
	m_Childrens.push_back(child);
}

void Renderable::SetPosition(glm::vec3 point)
{
	position = glm::translate(glm::mat4(1.0f), point);
	dirtyMatrix = true;
}

glm::vec3 Renderable::GetPosition()
{
	return glm::vec3(position[3]);
}

void Renderable::Translate(glm::vec3 amount)
{
	position = glm::translate(position, amount);
	dirtyMatrix = true;
}

void Renderable::Scale(float amount)
{
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(amount, amount, amount));
	dirtyMatrix = true;
}

void Renderable::SetRotation(glm::vec3 rotationAngles)
{
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngles.x), glm::vec3(1.0f, 0, 0));
	rotation = glm::rotate(rotation, glm::radians(rotationAngles.y), glm::vec3(0, 1.0f, 0));
	rotation = glm::rotate(rotation, glm::radians(rotationAngles.z), glm::vec3(0, 0, 1.0f));
	dirtyMatrix = true;
}

void Renderable::SetRotationMatrix(glm::mat4 rotMatrix)
{
	rotation = rotMatrix;
	dirtyMatrix = true;
}

void Renderable::Rotate(glm::vec3 axis, float angle)
{
	rotation = glm::rotate(rotation, glm::radians(angle), axis);
	dirtyMatrix = true;
}

void Renderable::Draw(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, Material* overrideMat, glm::mat4 parentModel)
{
	if (dirtyMatrix) {
		model = position * rotation * scale;
		dirtyMatrix = false;
	}

	if (overrideMat != nullptr)
	{
		overrideMat->Apply(sceneData, proj, view, (parentModel * model));

		for (Mesh& m : m_Meshes) {
			glBindVertexArray(m.VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.IBO);
			glDrawElements(m.meshMode, m.indicesCount, GL_UNSIGNED_INT, 0);
		}
	}

	for (Renderable& r : m_Childrens) {
		r.Draw(sceneData, proj, view, r.GetMaterial(), (parentModel * model));
	}
}

void Renderable::Draw(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 parentModel)
{
	Draw(sceneData, proj, view, m_Material, parentModel);

	/*
	if (dirtyMatrix) {
		model = position * rotation * scale;
		dirtyMatrix = false;
	}


	*/

	/* QUESTO PEZZO RIMANE PER CHIARIRE CHE STA SUCCEDENDO
	*
	glUseProgram(m_Shader.program);
	glUniformMatrix4fv(m_Shader["uProj"], 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(m_Shader["uView"], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(m_Shader["uModel"], 1, GL_FALSE, &(parentModel * model)[0][0]);
	*/

	/*
	m_Material->Apply(sceneData, proj, view, (parentModel * model));

	for (Mesh& m : m_Meshes) {
		glBindVertexArray(m.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.IBO);
		glDrawElements(GL_TRIANGLES, m.indicesCount, GL_UNSIGNED_INT, 0);
	}

	for (Renderable& r : m_Childrens) {
		r.Draw(sceneData, proj, view, model);
	}
	*/
}

Material* Renderable::GetMaterial()
{
	return m_Material;
}

BoundingBox& Renderable::GetBoundingBox()
{
	return boundingBox;
}
