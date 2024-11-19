#include "UnlitMaterial.h"

UnlitMaterial::UnlitMaterial(Shader* shader, Texture texture) : Material(shader), m_Texture(texture), mColor(1, 1, 1)
{
}

void UnlitMaterial::Apply(SceneData& sceneData, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
	GetShader()->Bind();
	m_Texture.Activate(1);

	GetShader()->SetUniformMat4f("uProj", proj);
	GetShader()->SetUniformMat4f("uView", view);
	GetShader()->SetUniformMat4f("uModel", model);

	GetShader()->SetUniform3f("uColor", 1, (float*)mColor);
	GetShader()->SetUniform1i("uAlbedoTexture", m_Texture.CurrentTextureUnit());
}

void UnlitMaterial::SetColor(Color color)
{
	mColor = color;
}

Color& UnlitMaterial::GetColor()
{
	return mColor;
}
