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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// WARNING: don't delete the texture object here; it isn't owned by this class.
#if 0
	if (0 != m_texture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_texture);
	}
#endif

	if (0 != m_normal)
	{
		glDeleteBuffers(1, &m_normal);
	}

	if (0 != m_index)
	{
		glDeleteBuffers(1, &m_index);
	}

	if (0 != m_texCoord)
	{
		glDeleteBuffers(1, &m_texCoord);
	}

	if (0 != m_vbo)
	{
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

	const bool HasTexCoords = pMesh->HasTextureCoords(0);    // we are interested only in the first set of texture coordinates

	const bool HasNormals = pMesh->HasNormals();

	std::vector<GLfloat> vertexCoords(VertexCount * 3);

	std::vector<GLfloat> texCoords;
	if (HasTexCoords)
	{
		texCoords.resize(VertexCount * 2);
	}

	std::vector<GLfloat> normals;
	if (HasNormals)
	{
		normals.resize(VertexCount * 3);
	}

	unsigned int vertexCurrent   = {};
	unsigned int texCoordCurrent = {};
	unsigned int normalCurrent   = {};

	for (unsigned int i = 0; i < VertexCount; ++i)
	{
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].x;
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].y;
		vertexCoords[vertexCurrent++] = pMesh->mVertices[i].z;

		if (HasTexCoords)
		{
			texCoords[texCoordCurrent++] = pMesh->mTextureCoords[0][i].x;
			texCoords[texCoordCurrent++] = pMesh->mTextureCoords[0][i].y;
		}

		if (HasNormals)
		{
			normals[normalCurrent++] = pMesh->mNormals[i].x;
			normals[normalCurrent++] = pMesh->mNormals[i].y;
			normals[normalCurrent++] = pMesh->mNormals[i].z;
		}
	}

	// Extract vertex indices.

	std::vector<GLuint> indices;

	if (pMesh->HasFaces())
	{
		const unsigned int FaceCount = pMesh->mNumFaces;

		// Assuming that the model consists of triangles, reserve space for 3 indices per face.
		// Note that curves, for example, have 2 indices, so we need to shrink the container afterwards.
		indices.reserve(FaceCount * 3);

		for (unsigned int f = 0; f < FaceCount; ++f)
		{
			const unsigned int IndexCount = pMesh->mFaces[f].mNumIndices;
			assert(IndexCount < 4);

			for (unsigned int i = 0; i < IndexCount; ++i)
			{
				indices.push_back(pMesh->mFaces[f].mIndices[i]);
			}
		}

		indices.shrink_to_fit();
	}

	if (   vertexCoords.empty()
		|| indices.empty())    // if there are indeed no indices, we should change the rendering function accordingly
	{
		throw EXCEPTION(L"Model: no vertex coordinates and/or indices");
	}

	setupShaderData(vertexCoords, indices, texCoords, normals);
}

void Mesh::setupShaderData(
	const std::vector<GLfloat>& vertexCoords, 
	const std::vector<GLuint>& indices, 
	const std::vector<GLfloat>& texCoords,
	const std::vector<GLfloat>& normals)
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

	if (!texCoords.empty())
	{
		// Set up the texture coordinates buffer.

		glGenBuffers(1, &m_texCoord);
		glBindBuffer(GL_ARRAY_BUFFER, m_texCoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);

		const GLuint AttrTextureCoord = 1;
		glVertexAttribPointer(AttrTextureCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(AttrTextureCoord);
	}

	if (!normals.empty())
	{
		// Set up the normal buffer.

		glGenBuffers(1, &m_normal);
		glBindBuffer(GL_ARRAY_BUFFER, m_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);

		const GLuint AttrNormal = 2;
		glVertexAttribPointer(AttrNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(AttrNormal);
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
