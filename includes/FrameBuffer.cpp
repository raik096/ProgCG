#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(int width, int height)
{
	mWidth = width;
	mHeigth = height;

	//Fai cose per inizializzare fbo
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	//Color attachment
	m_Color = new Texture(mWidth, mHeigth, GL_RGB32F, GL_RGB, GL_NEAREST, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Color->GetID(), 0);

	//Depth attachment
	m_Depth = new Texture(mWidth, mHeigth, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_NEAREST, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_Depth->GetID(), 0);


	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	Check(status);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_ID);
	glDeleteTextures(1, &m_Color->GetID());
	glDeleteTextures(1, &m_Depth->GetID());
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int FrameBuffer::Width()
{
	return mWidth;
}

int FrameBuffer::Heigth()
{
	return mHeigth;
}

Texture& FrameBuffer::ColorTexture()
{
	return *m_Color;
}

Texture& FrameBuffer::DepthTexture()
{
	return *m_Depth;
}

void FrameBuffer::Check(int fboStatus)
{
	switch (fboStatus) {
	case GL_FRAMEBUFFER_COMPLETE:						std::cout << "FBO Complete \n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          std::cout << "FBO Incomplete: Attachment\n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:  std::cout << "FBO Incomplete: Missing Attachment\n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:      std::cout << "FBO Incomplete: Dimensions\n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:         std::cout << "FBO Incomplete: Formats\n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:         std::cout << "FBO Incomplete: Draw Buffer\n"; break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:         std::cout << "FBO Incomplete: Read Buffer\n"; break;
	case GL_FRAMEBUFFER_UNSUPPORTED:                    std::cout << "FBO Unsupported\n"; break;
	default:                                            std::cout << "Undefined FBO error\n"; break;
	}
}
