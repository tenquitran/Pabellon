#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PabellonApp;

//////////////////////////////////////////////////////////////////////////


MaterialInfo::MaterialInfo(void)
{
}

MaterialInfo::~MaterialInfo()
{
}

Textures MaterialInfo::getTextures() const
{
	Textures textures;

	for (const auto& itr : m_textures)
	{
		textures.emplace(itr.first, itr.second->m_texture);
	}

	return textures;
}

bool MaterialInfo::addTextures(const std::string& directoryPath, const std::vector<std::string>& fileNames, aiTextureType textureType)
{
	if (m_textures.cend() != m_textures.find(textureType))
	{
		return true;    // Texture of this type already exists: not an error.
	}

	for (const auto& itrFileName : fileNames)
	{
		std::string filePath;

		if (!directoryPath.empty())
		{
			filePath += directoryPath;
		}

		filePath += itrFileName;

		// Load an image file directly as a new OpenGL texture.

		GLuint texture = SOIL_load_OGL_texture(
			filePath.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | /*SOIL_FLAG_INVERT_Y |*/ SOIL_FLAG_NTSC_SAFE_RGB /*| SOIL_FLAG_COMPRESS_TO_DXT*/);

		if (0 == texture)
		{
			std::wcerr << L"SOIL: failed to load texture (file " << filePath.c_str() << L"): " << SOIL_last_result() << std::endl;
			assert(false); return false;
		}

		m_textures.emplace(textureType, std::make_unique<TextureInfo>(textureType, texture, itrFileName));
	}

	return true;
}
