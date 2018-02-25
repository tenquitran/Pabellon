#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace PabellonApp;

//////////////////////////////////////////////////////////////////////////


TextureInfo::TextureInfo(aiTextureType type, GLuint texture, const std::string& fileName)
	: m_textureType(type), m_texture(texture), m_fileName(fileName)
{
}

TextureInfo::~TextureInfo()
{
	if (0 != m_texture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_texture);
	}
}
