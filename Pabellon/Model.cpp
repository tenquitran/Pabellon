#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace CommonLibOgl;
using namespace PabellonApp;

//////////////////////////////////////////////////////////////////////////

// Types of texture files to load.
const Model::TextureTypes Model::TexTypes = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR };

//////////////////////////////////////////////////////////////////////////


Model::Model(void)
	: m_program{}, /*m_unMvp(-1),*/ m_unView(-1), m_unModelView(-1), m_unProjection(-1), m_scaleFactor(1.0f)
{
}

Model::~Model()
{
}

bool Model::initialize(GLuint program, const std::string& filePath, const std::unique_ptr<Camera>& spCamera)
{
	m_program = program;
	m_modelFilePath = filePath;
	m_pCamera = spCamera.get();

	if (m_modelFilePath.empty())
	{
		std::wcerr << L"Model file path is empty\n";
		assert(false); return false;
	}

	const std::string::size_type pos = m_modelFilePath.rfind('//');
	if (std::wstring::npos != pos)
	{
		m_modelDirectoryPath = m_modelFilePath.substr(0, pos + 1);
	}

	Assimp::Importer importer;
	const aiScene *pScene = importer.ReadFile(m_modelFilePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (   !pScene 
		|| pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE 
		|| !pScene->mRootNode)
	{
		std::wcerr << L"Failed to read file " << m_modelFilePath.c_str() << L": " << importer.GetErrorString() << '\n';
		assert(false); return false;
	}

	// Extract data for all materials in the scene.
	if (!loadMaterials(pScene))
	{
		std::wcerr << L"Failed to load one or more materials\n";
		assert(false); return false;
	}

	// Extract data for all meshes in the scene.
	if (!loadMeshes(pScene, pScene->mRootNode))
	{
		std::wcerr << L"Failed to load one or more meshes\n";
		assert(false); return false;
	}

	if (!initializeMatrices())
	{
		std::wcerr << L"Failed to initialize matrices\n";
		assert(false); return false;
	}

	return true;
}

bool Model::initializeMatrices()
{
#if 0
	m_unMvp = glGetUniformLocation(m_program, "mvp");
	if (-1 == m_unMvp)
	{
		std::wcerr << L"Model: failed to get the MVP uniform location\n";
		assert(false); return false;
	}
#else
	
	m_unView = glGetUniformLocation(m_program, "View");
	if (-1 == m_unView)
	{
		std::wcerr << L"Model: failed to get the Model uniform location\n";
		assert(false); return false;
	}

	m_unModelView = glGetUniformLocation(m_program, "ModelView");
	if (-1 == m_unModelView)
	{
		std::wcerr << L"Model: failed to get the ModelView uniform location\n";
		assert(false); return false;
	}

	m_unProjection = glGetUniformLocation(m_program, "Projection");
	if (-1 == m_unProjection)
	{
		std::wcerr << L"Model: failed to get the Projection uniform location\n";
		assert(false); return false;
	}
#endif

	updateMatrices();

	return true;
}

void Model::updateMatrices()
{
	assert(m_program);
#if 0
	assert(-1 != m_unMvp);
#else
	assert(-1 != m_unView);
	assert(-1 != m_unModelView);
	assert(-1 != m_unProjection);
#endif

	glUseProgram(m_program);

	m_projection = m_pCamera->getProjectionMatrix();

	m_view = m_pCamera->getViewMatrix();
	
	m_model = m_pCamera->getModelMatrix();

	// Translate.
	m_model *= glm::translate(glm::mat4(1.0f), glm::vec3(m_translation.x, m_translation.y, m_translation.z));

	// Rotate.
	m_model *= glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationDegrees.x), glm::vec3(1.0f, 0.0f, 0.0));    // X axis
	m_model *= glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationDegrees.y), glm::vec3(0.0f, 1.0f, 0.0));    // Y axis
	m_model *= glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationDegrees.z), glm::vec3(0.0f, 0.0f, 1.0));    // Z axis

	// Scale.
	m_model *= glm::scale(glm::mat4(1.0f), glm::vec3(m_scaleFactor, m_scaleFactor, m_scaleFactor));

	m_mvp = m_projection * m_view * m_model;

#if 0
	glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(m_mvp));
#else
	glUniformMatrix4fv(m_unView,       1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(m_unModelView,  1, GL_FALSE, glm::value_ptr(m_view * m_model));
	glUniformMatrix4fv(m_unProjection, 1, GL_FALSE, glm::value_ptr(m_projection));
#endif

	glm::mat4 modelView = m_pCamera->getModelViewMatrix();

	// WARNING: we are using the fact that there are no non-uniform scaling. If this will change, use the entire 4x4 matrix.
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelView)));

	// TODO: turn on
#if 0
	glUniformMatrix4fv(m_unNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
#endif

	glUseProgram(0);
}

void Model::translateX(GLfloat diff)
{
	m_translation.x += diff;

	updateMatrices();
}

void Model::translateY(GLfloat diff)
{
	m_translation.y += diff;

	updateMatrices();
}

void Model::translateZ(GLfloat diff)
{
	m_translation.z += diff;

	updateMatrices();
}

void Model::rotateX(GLfloat angleDegrees)
{
	m_rotationDegrees.x += angleDegrees;

	updateMatrices();
}

void Model::rotateY(GLfloat angleDegrees)
{
	m_rotationDegrees.y += angleDegrees;

	updateMatrices();
}

void Model::rotateZ(GLfloat angleDegrees)
{
	m_rotationDegrees.z += angleDegrees;

	updateMatrices();
}

void Model::scale(GLfloat amount)
{
	m_scaleFactor += amount;

	updateMatrices();
}

bool Model::loadMaterials(const aiScene* pScene)
{
#if 0
	if (!pScene->HasTextures())
	{
		return;
	}
#endif
	
	// When loading mesh data later, we'll be interested in the texture to mesh correspondence 
	// to speed up the texture search.

	const unsigned int MeshCount = pScene->mNumMeshes;

	for (MeshIndex meshIndex = 0; meshIndex < MeshCount; ++meshIndex)
	{
		const aiMesh *pMesh = pScene->mMeshes[meshIndex];
		if (!pMesh)
		{
			std::wcerr << L"loadMaterials: mesh is NULL";
			assert(false); return false;
		}

		const MaterialIndex MaterialIndex = pMesh->mMaterialIndex;

		const aiMaterial *pMaterial = pScene->mMaterials[MaterialIndex];
		if (!pMaterial)
		{
			std::wcerr << L"loadMaterials: material is NULL\n";
			assert(false); return false;
		}

		MaterialItr materialItr = m_materials.find(MaterialIndex);

		if (m_materials.cend() == materialItr)
		{
			std::pair<MaterialItr, bool> texturePair = m_materials.emplace(MaterialIndex, std::make_unique<MaterialInfo>());

			materialItr = texturePair.first;
		}

		// Enumerate material's textures by types.

		for (auto textureType : TexTypes)
		{
			const unsigned int TextureCount = pMaterial->GetTextureCount(textureType);

			std::vector<std::string> texFileNames;

			if (TextureCount > 0)
			{
				texFileNames.resize(TextureCount);
			}

			for (unsigned int texIndex = 0; texIndex < TextureCount; ++texIndex)
			{
				aiString textureFileName;
				pMaterial->GetTexture(textureType, texIndex, &textureFileName);

				texFileNames[texIndex] = textureFileName.C_Str();

				std::wcout << L"Texture file: " << textureFileName.C_Str() << '\n';
			}

			if (texFileNames.empty())
			{
				if (aiTextureType_DIFFUSE == textureType)
				{
					std::wcout << L"No diffuse textures\n";
				} 
				else if (aiTextureType_SPECULAR == textureType)
				{
					std::wcout << L"No specular textures\n";
				}
				else
				{
					std::wcout << L"No textures\n";
					assert(false);    // new texture type?
				}
			}
			else if (!materialItr->second->addTextures(m_modelDirectoryPath, texFileNames, textureType))
			{
				return false;
			}
		}
	}

	return true;
}

bool Model::loadMeshes(const aiScene* pScene, const aiNode* pNode)
{
	// Process all meshes of the node.

	const unsigned int MeshCount = pNode->mNumMeshes;

	std::vector<GLuint> meshTextures;

	for (unsigned int mesh = 0; mesh < MeshCount; ++mesh)
	{
		// Get info on all materials and textures of the mesh.

		const aiMesh *pMesh = pScene->mMeshes[pNode->mMeshes[mesh]];

		const aiString meshName = pMesh->mName;

		const unsigned int MaterialIndex = pMesh->mMaterialIndex;

		MaterialItrConst materialItr = m_materials.find(MaterialIndex);
		if (m_materials.cend() == materialItr)
		{
			// TODO: log the material name, etc.
			std::wcout << L"Material not found\n";
			return false;
		}

		m_meshes.emplace_back(std::make_unique<Mesh>(meshName.C_Str(), m_program, materialItr->second->getTextures(), pMesh));
	}

	// Recursively process node's children.

	const unsigned int ChildCount = pNode->mNumChildren;

	for (unsigned int i = 0; i < ChildCount; ++i)
	{
		if (!loadMeshes(pScene, pNode->mChildren[i]))
		{
			return false;
		}
	}

	return true;
}

void Model::render() const
{
	//glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(m_mvp));

	for (const auto& itr : m_meshes)
	{
		itr->render();
	}
}
