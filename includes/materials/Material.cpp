#include "Material.h"

Material::Material(Shader* shader) : m_Shader(shader)
{
}

void Material::Apply(SceneData& scene, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
{
	GetShader()->Bind();
	//GetTexture().Activate(1);

	GetShader()->SetUniformMat4f("uProj", proj);
	GetShader()->SetUniformMat4f("uView", view);
	GetShader()->SetUniformMat4f("uModel", model);
	//glUniformMatrix4fv(GetShader()["uProj"], 1, GL_FALSE, &proj[0][0]);
	//glUniformMatrix4fv(GetShader()["uView"], 1, GL_FALSE, &view[0][0]);
	//glUniformMatrix4fv(GetShader()["uModel"], 1, GL_FALSE, &model[0][0]);

}

Shader* Material::GetShader()
{
	return m_Shader;
}
/*
Texture& Material::GetTexture()
{
	return m_Texture;
}
*/