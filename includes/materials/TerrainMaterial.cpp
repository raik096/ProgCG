#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(Shader* shader, Texture groundTexture, Texture rockTexture, Texture groundNormal, Texture rockNormal) : Material(shader),
		groundTexture(groundTexture), rockTexture(rockTexture), groundNormal(groundNormal), rockNormal(rockNormal), mSpecularColor(0, 0, 0), mShininess(20), mGlossiness(5)
{
}
void TerrainMaterial::Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
	GetShader()->Bind();
	groundTexture.Activate(1);
	rockTexture.Activate(2);
	groundNormal.Activate(3);
	rockNormal.Activate(4);

	GetShader()->SetUniformMat4f("uProj", proj);
	GetShader()->SetUniformMat4f("uView", view);
	GetShader()->SetUniformMat4f("uModel", model);

	GetShader()->SetUniform3f("uViewPos", view[3][0], view[3][1], view[3][3]);
	glm::vec3 lightDir = glm::normalize(sceneData.lightDirection);
	GetShader()->SetUniform3f("uLightDir", sceneData.ligth->direction.x, sceneData.ligth->direction.y, sceneData.ligth->direction.z);
	GetShader()->SetUniform3f("uLightColor", 1, sceneData.lightColor);
	GetShader()->SetUniform3f("uAmbientColor", 1, sceneData.ambientColor);

	//std::cout << mColor.R() << ", " << mColor.B() << ", " << mColor.G() << "\n";
	GetShader()->SetUniform3f("uSpecularColor", 1, (float*)mSpecularColor);
	GetShader()->SetUniform1f("uShininess", mShininess);
	GetShader()->SetUniform1f("uGlossiness", mGlossiness);

	GetShader()->SetUniform1i("uGroundTexture", groundTexture.CurrentTextureUnit());
	GetShader()->SetUniform1i("uRockTexture", rockTexture.CurrentTextureUnit());
	GetShader()->SetUniform1i("uGroundNormalMap", groundNormal.CurrentTextureUnit());
	GetShader()->SetUniform1i("uRockNormalMap", rockNormal.CurrentTextureUnit());

	//Shadow map sar� sempre nella texture unit #0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneData.ligth->fbo.DepthTexture().GetID());
	GetShader()->SetUniform1i("uShadowMap", 0);
	GetShader()->SetUniform1i("uShadowEnable", sceneData.enableShadows);
	GetShader()->SetUniform1f("uBias", sceneData.ligth->ligthBias);
	GetShader()->SetUniformMat4f("uLightMatrix", sceneData.ligth->LigthMatrix());
}

void TerrainMaterial::SetSpecularColor(Color color)
{
	mSpecularColor = color;
}

void TerrainMaterial::SetShininess(float shine)
{
	mShininess = shine;
}

void TerrainMaterial::SetGlossiness(float gloss)
{
	mGlossiness = gloss;
}
