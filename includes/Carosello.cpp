#include "Carosello.h"

#define NANOSVG_IMPLEMENTATION
#include "../vendor/nanosvg/src/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "../vendor/nanosvg/src/nanosvgrast.h"

#include "path.h"

void SampleCurve(const NSVGpath* path, double delta, std::vector<glm::vec3>& samples_pos, std::vector<glm::vec3>& samples_tan)
{
	std::vector<glm::vec3> controlPoints;
	for (int i = 0; i < path->npts; i += 1)
		controlPoints.push_back(glm::vec3(path->pts[i * 2], 0, path->pts[i * 2 + 1]));
	bezier_path::regular_sampling(controlPoints, 0.1f, samples_pos, samples_tan);
}


Carosello::Carosello(const char* objectFilePath) : caroselloRenderable()
{
	track = Track();
	terrain = Terrain();

	struct NSVGimage* image;
	NSVGrasterizer* rast = ::nsvgCreateRasterizer();

	image = nsvgParseFromFile(objectFilePath, "px", 96);
	for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
	{
		if (std::string(shape->id).find("track") != std::string::npos)
		{
			int ip = 0;
			std::vector<glm::vec3> samples_pos, samples_tan;
			SampleCurve(shape->paths, 0.1f, samples_pos, samples_tan);

			for (unsigned int i = 0; i < samples_pos.size(); ++i) {
				glm::vec3 d = glm::vec3(-samples_tan[i].z, 0, samples_tan[i].x);
				d = glm::normalize(d);
				track.AddLine(terrain.GetPoint(samples_pos[i] + d * 2.f) + glm::vec3(0, 0.2f, 0), terrain.GetPoint(samples_pos[i] - d * 2.f) + glm::vec3(0, 0.2f, 0));
			}
		}
	}

	caroselloRenderable.AddChild(terrain.RendereableObj());
	caroselloRenderable.AddChild(track.RendereableObj());
}

Carosello::~Carosello()
{
}

void Carosello::Build()
{

}

Renderable& Carosello::RendereableObj()
{
	return caroselloRenderable;
}
