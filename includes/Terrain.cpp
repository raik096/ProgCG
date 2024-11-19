#include "Terrain.h"
#include "ModelLoader.h"

#include "materials/TerrainMaterial.h"
#include "materials/LitMaterial.h"

Terrain::Terrain() : terrainRenderable()
{
	Generate();
}

Terrain::~Terrain()
{
}

void Terrain::Generate()
{
	const int TERRAIN_SIZE = 256/2;
	std::vector<Vertex> terrainVertices;
	std::vector<unsigned int> terrainIndices;

	for (int y = 0; y < TERRAIN_SIZE + 1; y++)
	{
		for (int x = 0; x < TERRAIN_SIZE + 1; x++)
		{
			Vertex v;
			v.Position = glm::vec3(x, 0, y);

			glm::vec3 f = glm::vec3(x, 0, y + 1);
			glm::vec3 r = glm::vec3(x + 1, 0, y);
			v.Normal = glm::normalize(glm::cross((f - v.Position), (r - v.Position)));
			v.UV = glm::vec2(x, y);

			v.Tangent = r;

			terrainVertices.push_back(v);
		}
	}

	for (int i = 0; i < (TERRAIN_SIZE) * (TERRAIN_SIZE); i++)
	{
		unsigned int offset = (i / (TERRAIN_SIZE)) * (TERRAIN_SIZE + 1);
		unsigned int x = (i % TERRAIN_SIZE) + offset;

		terrainIndices.insert(terrainIndices.end(), { x + 1, x, x + TERRAIN_SIZE + 1, x + 1, x + TERRAIN_SIZE + 1, x + TERRAIN_SIZE + 2 });
	}

	Mesh terrainMesh(terrainVertices, terrainIndices);

	Shader* terrainShader = new Shader("shaders/basic.vert", "shaders/terrain.frag");

	Texture groundTexture("resources/Grass.png");
	Texture rockTexture("resources/Rock.png");
	Texture groundNormal("resources/GrassNormal.png");
	Texture rockNormal("resources/RockNormal.png");
	TerrainMaterial* terrainMaterial = new TerrainMaterial(terrainShader, groundTexture, rockTexture, groundNormal, rockNormal);
	terrainMaterial->SetShininess(1);

	terrainRenderable = Renderable(terrainMesh, terrainMaterial);
	//terrainRenderable.SetPosition(glm::vec3(-TERRAIN_SIZE / 2, -1.0f, -TERRAIN_SIZE / 2));
}

Renderable& Terrain::RendereableObj()
{
	return terrainRenderable;
}

glm::vec3 Terrain::GetPoint(glm::vec3 pos)
{
	return glm::vec3(pos.x, 0, pos.z);
}
