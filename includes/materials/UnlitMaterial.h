#pragma once

#include "Material.h"
#include "../Texture.h"
#include "../Color.h"

class UnlitMaterial : public Material
{
private:
	Color mColor;

	Texture m_Texture;

public:
	UnlitMaterial(Shader* shader, Texture texture = Texture::White());

	void Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model) override;

	void SetColor(Color color);
	Color& GetColor();
};