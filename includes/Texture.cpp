#include "Texture.h"

#include "../vendor/stb_image.h"

#include <cassert>

Texture::Texture(std::string path, GLint format) : m_ID(0), tu(0), m_Width(0), m_Height(0), m_Components(0)
{
	unsigned char* data;
	//stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Components, 0);

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	int channels;
	switch (m_Components) {
	case 1: channels = GL_RED; break;
	case 3: channels = GL_RGB; break;
	case 4: channels = GL_RGBA; break;
	default: assert(0);
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, channels, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(int width, int heigth, GLint format, GLenum components, GLint minificationFilter, GLint magnificationFilter, bool mipmaps) : m_ID(0), tu(0), m_Width(width), m_Height(heigth), m_Components(components)
{
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, m_Components, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

	if(mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture()
{
	//glDeleteTextures(1, &m_ID);
}

void Texture::Activate(int textureUnit)
{
	tu = textureUnit;
	glActiveTexture(GL_TEXTURE0 + tu);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

GLuint& Texture::GetID()
{
	return m_ID;
}

GLuint Texture::CurrentTextureUnit()
{
	return tu;
}

Texture Texture::White()
{
	return Texture("resources/white.png");
}

Texture Texture::NormalFlat()
{
	return Texture("resources/normalFlat.png");
}
