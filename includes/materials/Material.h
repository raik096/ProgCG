#pragma once

#include <glm/glm.hpp>

#include "../Shader.h"
#include "../SceneData.h"

class Material
{
private:
	Shader* m_Shader;


public:
	//Material(Shader* shader, Texture texture = Texture::White());
	Material(Shader* shader);

	virtual void Apply(SceneData& scene, glm::mat4 proj, glm::mat4 view, glm::mat4 model);

	Shader* GetShader();
	//Texture& GetTexture();
};