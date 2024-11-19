#pragma once
#include <GL/glew.h>
#include "Texture.h"

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void Bind();
	void Unbind();

	int Width();
	int Heigth();

	Texture& ColorTexture();
	Texture& DepthTexture();

private:
	int mWidth, mHeigth;

	GLuint m_ID;
	Texture* m_Color;
	Texture* m_Depth;

	void Check(int fboStatus);
};