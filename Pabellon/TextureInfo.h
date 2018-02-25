#pragma once


namespace PabellonApp
{
	// Data about textures.
	struct TextureInfo
	{
	public:
		TextureInfo(aiTextureType type, GLuint texture, const std::string& fileName);

		virtual ~TextureInfo();

	private:
		TextureInfo(const TextureInfo&) = delete;
		TextureInfo& operator=(const TextureInfo&) = delete;

	public:
		// Texture type.
		aiTextureType m_textureType;

		// OpenGL texture buffer.
		GLuint m_texture;

		// Name of the file containing texture data (useful for debugging).
		std::string m_fileName;
	};
}
