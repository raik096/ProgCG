#ifndef CG_UTILITIES
#define CG_UTILITIES

#include <iostream>
#include <string>

#include "renderable.h"
#include "shaders.h"
#include "texture.h"

//TEXTURES ------------------------------------
texture LoadTexture(std::string texPath)
{
	texture texture;
	GLuint id = texture.load(texPath, 0);
	if (id == 0) {
		std::cerr << "Errore nel caricamento della texture " << texPath  << std::endl;
	}
	else {
		std::cout << "Texture "<< texPath <<" caricata correttamente" << std::endl;
	}

    return texture;
}


//SHADERS --------------------------------------
void BindTexture(shader shader, std::string uniformName, texture texture, uint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    shader.SetInt(uniformName, textureUnit);
}

void BindTextureId(shader shader, std::string uniformName, GLint textureId, uint textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureId);
    shader.SetInt(uniformName, textureUnit);
}

//MODELS --------------------------------------
void DrawModel(std::vector<renderable>& obj, shader& shader, const glm::mat4& transform)
{
	for (int i = 0; i < obj.size(); i++)
	{
		obj[i].bind();
		shader.SetMatrix4x4("uModel", transform * obj[i].transform);
		
		BindTextureId(shader, "uTexture", obj[i].mater.base_color_texture, 1);
		glDrawElements(obj[i]().mode, obj[i]().count, obj[i]().itype, 0);	
	}
		
	/*
	for(renderable r : obj)
	{
		r.bind();
		shader.SetMatrix4x4("uModel", r.transform * transform);
		
		BindTextureId(shader, "uTexture", r.mater.base_color_texture, 0);
		glDrawElements(r.mode, r().count, r().itype, 0);
	}
	*/
}

#endif