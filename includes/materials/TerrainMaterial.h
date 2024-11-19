#pragma once

#include "Material.h"
#include "../Color.h"

class TerrainMaterial : public Material
{
private:
	Texture groundTexture;
	Texture groundNormal;
	Texture rockTexture;
	Texture rockNormal;

	Color mSpecularColor;
	float mGlossiness;
	float mShininess;

public:
	TerrainMaterial(Shader* shader, Texture groundTexture = Texture::White(), Texture rockTexture = Texture::White(), Texture groundNormal = Texture::White(), Texture rockNormal = Texture::White());

	void Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model) override;

	void SetSpecularColor(Color color);
	void SetShininess(float shine);
	void SetGlossiness(float gloss);
};