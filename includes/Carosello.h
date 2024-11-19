#pragma once

#include "Terrain.h"
#include "Track.h"


class Carosello
{
public:
	Carosello(const char* objectFilePath);
	~Carosello();

	void Build();

	Renderable& RendereableObj();

private:
	Terrain terrain;
	Track track;

	Renderable caroselloRenderable;
};