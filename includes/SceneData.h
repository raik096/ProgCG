#pragma once

#include <glm/ext.hpp>
#include "Ligth.h"

//Questa cosa esiste perchè se no creavo un loop di include tra Scene, Renderable, Material. Esisteva una soluzione migliore? boh probabile :)
struct SceneData
{
	glm::vec3 lightDirection;
	float* lightColor;
	float* ambientColor;

	Ligth* ligth;
	bool enableShadows;
};