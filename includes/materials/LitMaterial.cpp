#include "LitMaterial.h"
#include <GL/glew.h>

LitMaterial::LitMaterial(Shader* shader, Texture texture, Texture normalMap) : Material(shader),
m_Texture(texture), m_NormalMap(normalMap), mColor(1, 1, 1), mSpecularColor(1, 1, 1), mGlossiness(5), mShininess(0.5)
{
}

void LitMaterial::Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
	GetShader()->Bind();
	m_Texture.Activate(1);
	m_NormalMap.Activate(2);

	GetShader()->SetUniformMat4f("uProj", proj);
	GetShader()->SetUniformMat4f("uView", view);
	GetShader()->SetUniformMat4f("uModel", model);

	GetShader()->SetUniform3f("uViewPos", view[3][0], view[3][1], view[3][3]);
	glm::vec3 lightDir = glm::normalize(sceneData.lightDirection);
	GetShader()->SetUniform3f("uLightDir", sceneData.ligth->direction.x, sceneData.ligth->direction.y, sceneData.ligth->direction.z);
	GetShader()->SetUniform3f("uLightColor", 1, sceneData.lightColor);
	GetShader()->SetUniform3f("uAmbientColor", 1, sceneData.ambientColor);

	//std::cout << mColor.R() << ", " << mColor.B() << ", " << mColor.G() << "\n";
	GetShader()->SetUniform3f("uColor", mColor.R(), mColor.G(), mColor.B());
	GetShader()->SetUniform3f("uSpecularColor", 1, (float*)mSpecularColor);
	GetShader()->SetUniform1f("uShininess", mShininess);
	GetShader()->SetUniform1f("uGlossiness", mGlossiness);

	GetShader()->SetUniform1i("uAlbedoTexture", m_Texture.CurrentTextureUnit());
	GetShader()->SetUniform1i("uNormalMap", m_NormalMap.CurrentTextureUnit());

	//Shadow map sarà sempre nella texture unit #0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneData.ligth->fbo.DepthTexture().GetID());
	GetShader()->SetUniform1i("uShadowMap", 0);
	GetShader()->SetUniform1i("uShadowEnable", sceneData.enableShadows);
	GetShader()->SetUniform1f("uBias", sceneData.ligth->ligthBias);
	GetShader()->SetUniformMat4f("uLightMatrix", sceneData.ligth->LigthMatrix());
}

void LitMaterial::SetColor(Color color)
{
	mColor = color;//.Set(color.R(), color.G(), color.B());
}

Color& LitMaterial::GetColor()
{
	return mColor;
}

void LitMaterial::SetSpecularColor(Color color)
{
	mSpecularColor = color;
}

void LitMaterial::SetShininess(float shine)
{
	mShininess = shine;
}

void LitMaterial::SetGlossiness(float gloss)
{
	mGlossiness = gloss;
}

float* LitMaterial::Shininess()
{
	return &mShininess;
}
