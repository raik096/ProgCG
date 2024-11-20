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
    shader.SetInt("uTexture", textureUnit);
}

//MODELS --------------------------------------
void DrawModel(std::vector<renderable>& obj, shader& shader, const glm::mat4& transform)
{
	for(renderable r : obj)
	{
		r.bind();
		shader.SetMatrix4x4("uModel", transform);

		glDrawElements(r.mode, r().count, r().itype, 0);
	}
}

#endif