#pragma once


namespace PabellonApp
{
	class Mesh
	{
	public:
		// Parameters: meshName - mesh name;
		//             program - GLSL program;
		//             textures - texture data (key: texture type; value: texture ID);
		//             pMesh - the assimp library mesh.
		// Throws: std::runtime_error, std::bad_alloc
		Mesh(const std::string& meshName, GLuint program, const Textures& textures, const aiMesh* pMesh);

		virtual ~Mesh();

		void render() const;

	private:
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		// Throws: Exception, std::bad_alloc
		void extractMeshData(const aiMesh* pMesh);

		// Note: containers with texture coordinates and normals can be empty.
		void setupShaderData(
			const std::vector<GLfloat>& vertexCoords,
			const std::vector<GLuint>& indices, 
			const std::vector<GLfloat>& texCoords,
			const std::vector<GLfloat>& normals);

		void initializeTextureData();

	private:
		// Texture objects. WARNING: our class doesn't own them, so don't delete them.
		const Textures m_textures;

		// Mesh name (useful for debugging).
		const std::string m_meshName;

		// GLSL program (cached).
		const GLuint m_program;

		GLuint m_vao;

		GLuint m_vbo;

		GLuint m_index;
		size_t m_indexCount;

		GLuint m_texCoord;    // texture coordinate buffer

		// TODO: temp: using only the first texture for now
		GLuint m_currentTexture;

		GLint m_uTextureSampler;

		GLuint m_normal;    // normal buffer
	};
}
