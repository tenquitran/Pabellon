#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PabellonApp;

//////////////////////////////////////////////////////////////////////////


Mesh::Mesh(const std::string& meshName, GLuint program, const Textures& textures, const aiMesh* pMesh)
	: m_textures(textures), m_meshName(meshName), m_program(program), m_vao{}, m_vbo{}, m_index{}, m_indexCount{},
	  m_texCoord{}, m_currentTexture{}, m_uTextureSampler{}, m_normal{}
{
	extractMeshData(pMesh);

	initializeTextureData();
}

Mesh::~Mesh()
{
	// TODO: delete other VBOs as required

	// WARNING: don't delete the texture object here; it isn't owned by this class.
#if 0
	if (0 != m_texture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_texture);
	}
#endif

	if (0 != m_index)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_index);
	}

	if (0 != m_texCoord)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);    // is this correct?
		glDeleteBuffers(1, &m_texCoord);
	}

	if (0 != m_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vbo);
	}

	// TODO: delete other VAOs as required
	if (0 != m_vao)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
	}
}

void Mesh::extractMeshData(const aiMesh* pMesh)
{
	if (!pMesh->HasPositions())
	{
		throw EXCEPTION_FMT(L"Mesh %S has no vertex coordinates", m_meshName.c_str());
	}

	// Extract vertex coordinates, indices and texture coordinates.

	const unsigned int VertexCount = pMesh->mNumVertices;

	std::vector<GLfloat> vertexCoords(VertexCount * 3);

	const bool HasTexCoords = pMesh->HasTextureCoords(0);    // we are interested only in the first set of texture coordinates

	m_hasTexCoords = HasTexCoords;

	std::vector<GLfloat> texCoords(VertexCount * 2);

	unsigned int vertexCurrent   = {};
	unsigned int texCoordCurrent = {};

	for (unsigned int i = 0; i < VertexCount; ++i)
	{
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].x;
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].y;
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].z;

		// TODO: extract normals

		if (HasTexCoords)
		{
			texCoords[texCoordCurrent++] = pMesh->mTextureCoords[0][i].x;
			texCoords[texCoordCurrent++] = pMesh->mTextureCoords[0][i].y;
		}
		else
		{
			texCoords[texCoordCurrent++] = 0.0f;
			texCoords[texCoordCurrent++] = 0.0f;
		}
	}

	// Extract vertex indices.

	std::vector<GLuint> indices;

	if (pMesh->HasFaces())
	{
		const unsigned int FaceCount = pMesh->mNumFaces;

		// NOTE: curves, for example, has 2 indices instead of 3.
		//indices.resize(FaceCount * 3);    // we assume that the model consists of triangles

		unsigned int indexCurrent = {};

		for (unsigned int f = 0; f < FaceCount; ++f)
		{
			const unsigned int IndexCount = pMesh->mFaces[f].mNumIndices;

#if 0
			// TODO: temp
			if (3 != IndexCount)
			{
				int tmp = 1;
			}

			assert(3 == IndexCount);
#endif

			for (unsigned int i = 0; i < IndexCount; ++i)
			{
				indices.push_back(pMesh->mFaces[f].mIndices[i]);
				//indices[indexCurrent++] = pMesh->mFaces[f].mIndices[i];
			}
		}
	}

	if (   vertexCoords.empty()
		|| indices.empty()
		|| texCoords.empty())
	{
		throw EXCEPTION(L"Model: one or more data sets are empty");
	}

	setupShaderData(vertexCoords, indices, texCoords);
}

void Mesh::setupShaderData(
	const std::vector<GLfloat>& vertexCoords, 
	const std::vector<GLuint>& indices, 
	const std::vector<GLfloat>& texCoords)
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Set up the vertex buffer.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexCoords.size() * sizeof(vertexCoords[0]), &vertexCoords[0], GL_STATIC_DRAW);

	const GLuint AttrVertexPos = 0;

	glVertexAttribPointer(AttrVertexPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(AttrVertexPos);

	if (m_hasTexCoords)
	{
		// Set up the texture coordinates buffer.

		const GLuint AttrTextureCoord = 1;

		glGenBuffers(1, &m_texCoord);
		glBindBuffer(GL_ARRAY_BUFFER, m_texCoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(AttrTextureCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(AttrTextureCoord);
	}

	// Set up the index buffer.

	m_indexCount = indices.size();

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
}

void Mesh::initializeTextureData()
{
	for (auto& texItr : m_textures)
	{
		// Specify the texture object settings.

		// TODO: temp; use only diffuse textures for now.
		if (aiTextureType_DIFFUSE != texItr.first)
		{
			continue;
		}

		glActiveTexture(GL_TEXTURE0);

		m_currentTexture = texItr.second;

		glBindTexture(GL_TEXTURE_2D, texItr.second);

#if 1
		//glGenerateMipmap(GL_TEXTURE_2D);    // generated when creating the texture using SOIL

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Set up the texture sampler.

	glUseProgram(m_program);

	m_uTextureSampler = glGetUniformLocation(m_program, "uSampler");
	if (-1 == m_uTextureSampler)
	{
		assert(false); throw std::runtime_error("Failed to get the texture sampler location");
	}
	glUniform1i(m_uTextureSampler, 0);

	glUseProgram(0);
}

void Mesh::render() const
{
	glBindVertexArray(m_vao);

	glBindTexture(GL_TEXTURE_2D, m_currentTexture);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
}
