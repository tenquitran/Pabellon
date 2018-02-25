#pragma once


namespace PabellonApp
{
	// Key: texture type; value: texture ID.
	typedef std::multimap<aiTextureType, GLuint> Textures;

	// Key: texture type; value: texture information.
	// NOTE: If multitexturing is used, there can be several textures of the same type.
	typedef std::multimap<aiTextureType, std::unique_ptr<TextureInfo> > TextureCollection;


	class MaterialInfo
	{
	public:
		MaterialInfo(void);
		virtual ~MaterialInfo();

		// Add textures of the specified type.
		// Parameters: directoryPath - path to the directory containing the texture data files;
		//             fileNames - names of the files containing texture data;
		//             textureType - type of textures.
		// Returns: true on success, false otherwise.
		// Throws: std::bad_alloc
		bool addTextures(const std::string& directoryPath, const std::vector<std::string>& fileNames, aiTextureType textureType);

		Textures getTextures() const;

	private:
		// Textures that belong to the material.
		// Key: texture type; value: texture data.
		TextureCollection m_textures;
	};
}
