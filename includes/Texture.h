#pragma once
#include <GL/glew.h>
#include <string>

class Texture
{
public:
	Texture(std::string path, GLint format = GL_COMPRESSED_RGBA_ARB);
	Texture(int width, int heigth, GLint format, GLenum components, GLint minificationFilter, GLint magnificationFilter, bool mipmaps = false);
	~Texture();

	void Activate(int tu);
	GLuint& GetID();
	GLuint CurrentTextureUnit();

	static Texture White();
	static Texture NormalFlat();

private:
	unsigned int m_ID;
	GLuint tu;

	int m_Width;
	int m_Height;
	int m_Components;
};
