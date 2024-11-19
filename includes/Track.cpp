#include "Track.h"
#include "materials/LitMaterial.h"
#include "ModelLoader.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

Track::Track() : length(0), isDirty(true)
{

}

Track::~Track()
{
}

void Track::AddLine(glm::vec3 pointA, glm::vec3 pointB)
{
	curbs[0].push_back(pointA);
	curbs[1].push_back(pointB);
	length++;

	isDirty = true;
}

float Track::Length()
{
	return length;
}

Renderable& Track::RendereableObj()
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Shader* trakShader = new Shader("shaders/basic.vert", "shaders/basic.frag");
	LitMaterial* mat = new LitMaterial(trakShader);

	if (isDirty)
	{

		trackRenderable = Renderable();

		for (int i = 0; i < curbs[0].size(); i++)
		{
			Vertex v1, v2;
			v1.Position = curbs[0][i];
			v2.Position = curbs[1][i];

			glm::vec3 f1 = curbs[0][(i+1)];
			glm::vec3 r1 = glm::normalize(glm::cross(glm::vec3(0, 1, 0), (f1 - v1.Position)));
			v1.Normal = glm::vec3(0, 1, 0);//glm::normalize(glm::cross((f1 - v1.Position), (r1 - v1.Position)));

			glm::vec3 f2 = curbs[1][(i + 1)];
			glm::vec3 r2 = glm::normalize(glm::cross(glm::vec3(0, 1, 0), (f2 - v2.Position)));
			v2.Normal = glm::vec3(0, 1, 0);//glm::normalize(glm::cross((f2 - v2.Position), (r2 - v2.Position)));
			//v.UV = glm::vec2(x, y);

			v1.Tangent = glm::normalize(glm::cross(glm::vec3(0, 1, 0), (f1 - v1.Position)));
			v2.Tangent = glm::normalize(glm::cross(glm::vec3(0, 1, 0), (f2 - v2.Position)));

			v1.Normal = glm::normalize(glm::cross((f1 - v1.Position), v1.Tangent));
			v2.Normal = glm::normalize(glm::cross((f2 - v2.Position), v2.Tangent));

			vertices.push_back(v1);
			vertices.push_back(v2);


			//Renderable t1(testCube, mat);
			//t1.SetPosition(curbs[0][i]);
			//trackRenderable.AddChild(t1);
			//trackRenderable.AddChild(t2);
		}

		for (int i = 0; i < curbs[0].size() * 2; i++)
		{
			indices.push_back(i);
		}
	}

	Mesh m = Mesh(vertices, indices, GL_TRIANGLE_STRIP);

	trackRenderable = Renderable(m, mat);

	return trackRenderable;
}
