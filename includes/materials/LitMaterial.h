#pragma once

#include "Material.h"

#include "../Color.h"

class LitMaterial : public Material
{
private:
	Texture m_Texture;
	Texture m_NormalMap;

	Color mColor;
	Color mSpecularColor;
	float mGlossiness;
	float mShininess;

public:
	LitMaterial(Shader* shader, Texture texture = Texture::White(), Texture normalMap = Texture::NormalFlat());

	void Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model) override;

	void SetColor(Color color);
	Color& GetColor();

	void SetSpecularColor(Color color);
	void SetShininess(float shine);
	void SetGlossiness(float gloss);

	float* Shininess();
};